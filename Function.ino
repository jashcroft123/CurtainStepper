void timeUpdate() {
  currentTime = millis();
  const int printStateInterval = 10 * 60 * 1000; //report state every 10 minutes

  if (currentTime - previousTime > printStateInterval) {
    printBit = true; //print state
    previousTime = currentTime;
  }
}

void openState () {
  if (printBit == true) {
    Particle.publish("Curtain State", String("Open"));
    printBit = false;
  }
}

void closedState () {
  if (printBit == true) {
    Particle.publish("Curtain State", String("Closed"));
    printBit = false;
  }
}

void error() {
  if (printBit == true) {
    Particle.publish("Curtain State", String("ERROR NO STATE"));
    printBit = false;
  }
}

void operateMotor() {
  if (numOfSteps == 0){
    if (oldCurtainState == closed)
        numOfSteps = percentClosed;
    else if (oldCurtainState == open)
        numOfSteps = 100 - percentClosed;
    numOfSteps=numOfSteps*stepsPerTotal/100;
  }

  digitalWrite(enablePin,HIGH);
  if (oldCurtainState == open) {
    myStepper.step(-1);
  }
  else if (oldCurtainState == closed) {
    myStepper.step(1);
  }
  counter = counter + 1;
  if (counter >= (numOfSteps)) {
    endOfMovement();
  }
}

void endOfMovement() {
  digitalWrite(enablePin,LOW);
  int percentMoved = (counter/(stepsPerRevolution*11))*100;

  if (oldCurtainState == open) {
    curtainState = closed;
    percentClosed = percentClosed + percentMoved;
    if (percentClosed > 100)
        percentClosed = 100;
  }
  else if (oldCurtainState == closed) {
    curtainState = open;
    percentClosed = percentClosed - percentMoved;
    if (percentClosed < 0)
        percentClosed = 0;
  }
  oldCurtainState = moving;
  String stateString = "Ending Movement";
  printBit = true;
  numOfSteps = 0;
  counter = 0;
}

void requestCheck() {
  buttonCurtain = digitalRead(buttonCurtainPin); //check button state
  if (currentTime - lastDebounceTime >= 500) {
    if (buttonCurtain == HIGH) {
      buttonRequest();
      lastDebounceTime = currentTime;
    }
  }
  if (curtainState != moving) {
    sunRequest();
  }
}

void buttonRequest() {
    //user requests state change using button
  if (curtainState != moving) {
      //if not moving start Moving
    Particle.publish("Curtain State", String("Started Moving"));
    oldCurtainState = curtainState;
    curtainState = moving;
  }
  else if (curtainState == moving) {
    //if already operating and button pressed stop
    endOfMovement();
  }
}


void sunRequest() {
  Particle.syncTime();
  int offset = Time.getDSTOffset();
  Time.zone(offset);
  static int first = true;


  static int currentDay;
  static int currentMonth = Time.month();
  static int currentYear = Time.year();
  double currentSunTime =  (Time.hour()) + Time.minute()/100.0+Time.second()/10000.0;


  if (currentDay < Time.day() || currentMonth < Time.month() || currentYear<Time.year()) {
       //check if sun rise/set to toggle open or closed
        sunriseTimeCheck();
        sunsetTimeCheck();
        currentDay = Time.day();
        currentMonth = Time.month();
        currentYear = Time.year();
        memset(triggered,false,2);
  }

  if (first == true){
      //checks time compared to sunset and rise times on first start up
      if (currentSunTime > sunriseTime)
        triggered[0] = true;
      if (currentSunTime > sunsetTime)
        triggered [1] = true;
      first = false;
  }


  if (currentSunTime > sunriseTime && triggered[0] == false && currentSunTime <12) {
    //open for morining
    triggered[0] = true;
    if (percentClosed > 0) {
      curtainState = closed;
      buttonRequest();
    }
  }

  if (currentSunTime > sunsetTime && triggered[1] == false) {
    //close for night
    triggered[1] = true;
    if (percentClosed < 100) {
      curtainState = open;
      buttonRequest();
    }
  }
}

double sunriseTimeCheck(){
    sun.setCurrentDate(Time.year(), Time.month(), Time.day());
    double sunrise = sun.calcSunrise();

    int sunriseHour = sunrise / 60;
    int sunriseMinute = ((sunrise / 60) - sunriseHour) * 60;
    float sunriseSeconds = ((((sunrise/60)- sunriseHour)*60)-(sunriseMinute))*60 +0.5;
    sunriseTime = sunriseHour + sunriseMinute/100.00 + sunriseSeconds/10000;

    write2Console(0,sunriseHour,sunriseMinute,sunriseSeconds);

    if (sunrise < 540)
        sunriseTime = 9;
}

double sunsetTimeCheck(){
    double sunset = sun.calcSunset();
    int sunsetHour = sunset / 60;
    int sunsetMinute = (((sunset / 60) - sunsetHour) * 60);
    float sunsetSeconds = ((((sunset/60)- sunsetHour)*60)-(sunsetMinute))*60 + 0.5;
    sunsetTime = sunsetHour + sunsetMinute/100.0 + sunsetSeconds/10000.0;

    write2Console(1,sunsetHour,sunsetMinute,sunsetSeconds);
}

void write2Console(int i, int sunHour, int sunMinute, int sunSeconds){
    sun2Console[i] = String(sunHour);
    sun2Console[i] += ":";

    if (sunMinute <10)
        sun2Console[i] += "0";
    sun2Console[i] += String(sunMinute);

    sun2Console[i] += ":";

    if (sunSeconds < 10)
        sun2Console[i] += "0";
    sun2Console[i] += String(sunSeconds);
}
