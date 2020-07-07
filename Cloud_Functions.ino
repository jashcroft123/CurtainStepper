int cloudRequest(String percentClosedRequest){
    int request = percentClosedRequest.toInt();
    if (request >100)
        request = 100;
    else if (request < 0)
        request = 0;

    if(curtainState != moving){
        if (request < percentClosed) {
            numOfSteps = percentClosed - request;
            curtainState = closed;
        }
        else if (request>percentClosed){
            curtainState = open;
            numOfSteps = request - percentClosed;
        }
        numOfSteps = numOfSteps * stepsPerTotal/100;

        if (numOfSteps != 0)
        buttonRequest();
    }
    else{
        buttonRequest();
    }
    return request;
}

int changeStateCloud(String command) {
  command.toLowerCase();

  if(command == "open" || command == "1")
  {
    percentClosed = 0;
    oldCurtainState = curtainState;
    curtainState = open;
    return open;
  }

  else if (command =="closed" || command == "2"){
     percentClosed = 100;
     oldCurtainState = curtainState;
     curtainState = closed;
     return closed;
  }
  else return -1;
}
