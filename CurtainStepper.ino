#include <Stepper.h>
const int stepsPerRevolution = 205;
const int enablePin = D1;
Stepper myStepper(stepsPerRevolution, D3, D4, D5, D6);
int stepsPerTotal = stepsPerRevolution *11;
double counter = 0; //number of steps moved /10
float numOfSteps = 0;
int percentClosed = 0;

#include <sunset.h>
SunSet sun;
#define LATITUDE        52.898060
#define LONGITUDE       -1.308730
#define CST_OFFSET      0
#define DST_OFFSET      1


String sun2Console[2]; //0 sunrise, 1 sunset
bool triggered[2] = {true, true};

int changeStateCloud(String command);
int cloudRequest(String percentClosedRequest);

double sunriseTime;
double sunsetTime;

#define buttonCurtainPin  D0 // push button to open and close curtains
int buttonCurtain = 0;

//define states
#define open 1
#define closed 2
#define moving 3

int curtainState = open; //curtains set to open for intialisation
int oldCurtainState = open;

bool printBit;
hello

unsigned long currentTime;
unsigned long previousTime;
unsigned long lastDebounceTime;


void setup() {
  Serial.begin(9600);

  myStepper.setSpeed(60); //initialise Stepper

  RGB.brightness(0); // all of the way off (MAX DARK)

  pinMode(buttonCurtainPin, INPUT_PULLDOWN); //button to close curtain
  pinMode(enablePin, OUTPUT); //button to close curtain


  sun.setPosition(LATITUDE, LONGITUDE, DST_OFFSET);
  Particle.variable("Sunrise (HH:MM:SS)", sun2Console[0]);
  Particle.variable("Sunset  (HH:MM:SS)", sun2Console[1]);


  Particle.variable("State", curtainState);
  Particle.variable("Percent Closed", percentClosed);

  Particle.function("State Set", changeStateCloud);
  Particle.function("Move", cloudRequest);

  waitUntil(Particle.syncTimeDone);
  delay(500);
  sunriseTimeCheck();
  sunsetTimeCheck();
}


void loop() {
  requestCheck();
  timeUpdate();
  state();
}


void state() {
  //function check the state of curtains and returns via console
  switch (curtainState) {
    case open:
      openState();
      break;

    case closed:
      closedState();
      break;

    case moving:
      operateMotor();
      break;

    default:
      error();
      break;
  }
}
