#include "finger.h"
#include "Arduino.h"

/////////////////////////////////////////////////
/////////////// CONSTRUCTOR, INIT////////////////
/////////////////////////////////////////////////

finger::finger(int IN_1, int IN_2, int EN, int ch_a, int ch_b,
                         int hallEffectPin, float minHallEffectReadingG, float maxHallEffectReadingG) {
  _IN_1 = IN_1;
  _IN_2 = IN_2;
  _EN = EN;

  _input_ch[0] = ch_a;
  _input_ch[1] = ch_b;

  _hallEffectPin = hallEffectPin;
  _minHallEffectReadingG = minHallEffectReadingG;
  _maxHallEffectReadingG = maxHallEffectReadingG;

}

void finger::init() {
  // Make IN1 and IN2 output pins
  pinMode(_IN_1, OUTPUT);
  pinMode(_IN_2, OUTPUT);

  // Make all the encoder pins inputs
  for (int i = 0; i < 2; i++) {
    pinMode(_input_ch[i], INPUT_PULLUP);
  }

  // enable the hall effect input
  pinMode(_hallEffectPin, INPUT);
}

/////////////////////////////////////////////////
////////////// CONTROL  METHODS /////////////////
/////////////////////////////////////////////////

// grab the finger at a desired compression
void finger::graspObject(int desCompression){
  static int desPos = _curPos;
  const int increment = 10;

  // keep in bounds
  desCompression = 100*(desCompression > 100) + desCompression*(desCompression <= 100);

  int curCompression = finger::getHallEffectCompression();  

  if (curCompression < desCompression){
    desPos += increment;
  }
  else if (curCompression > desCompression){
    desPos -= increment;
  }

  finger::PID(desPos);

}

// drive forward until barely running into the object
bool finger::touchObject(){
  static int setPoint = 0;
  const int contactThreshold = 10; // percent;
  const int PIDIncrement = 10;

  int curCompression = finger::getHallEffectCompression();  
  bool contactFlag = curCompression > contactThreshold;
  
  if (!contactFlag){
    setPoint += PIDIncrement;
    finger::PID(setPoint);
    return false;
  }
  else{
    finger::stopMotor();
    return true;
  }    
}

void finger::PID(int desPos) {
  // desPos will be determined as a function of compression when using the hall
  // effect. It's useful to have a baseline PID function, though REMEMBER CURPOS
  // IS IN "TICKS"
  static int last_p_error = 0;
  static unsigned int last_time = millis();
  static int i_error = 0; // integral of error;
  static int stationaryCounter = 0;
  static int lastStationaryPosition = 0;

  // get timing info
  unsigned int now = millis();
  unsigned int dt = now - last_time;
  last_time = now;

  // find errors
  int p_error = desPos - _curPos;                // P;
  float d_error = (last_p_error - p_error) / dt; // D;
  i_error -= p_error * dt;                      // I;

  // reset for next time
  last_p_error = p_error;

  // Calculate commanded PWM (speed)
  float PwmCommand = _kp * p_error + _kd * d_error + _ki * i_error;

  if (PwmCommand > 255) {
    PwmCommand = 255;
  } else if (PwmCommand < -255) {
    PwmCommand = -255;
  }

  // increment if you've been stationary
  if (abs(p_error) <= 1) {
    stationaryCounter++;
  } else {
    stationaryCounter = 0;
  }

  // flush integral error if you've been stationary for a while
  if (stationaryCounter >= 15) {
    i_error = 0;
    lastStationaryPosition = desPos;
    finger::stopMotor(); // just hold the current position
    _PIDStationary = true;
    // in case desPos changes, reset all of this logic
    if (desPos != lastStationaryPosition) {
      stationaryCounter = 0;
      _PIDStationary = false;
    }
  }
  else
    finger::driveMotor(PwmCommand);
}

/////////////////////////////////////////////////
/////////// HALL EFFECT  METHODS ////////////////
/////////////////////////////////////////////////
void finger::readHallEffect(){
  _hallEffectReadingV = analogRead(_hallEffectPin);
  _hallEffectReadingG = (_hallEffectReadingV * _hallEffectResolution - _zeroField) / _hallEffectSensitivity;  // in Gauss;
}

unsigned int finger::getHallEffectCompression(){
  // map the compression to a percentage

  // update the reading
  finger::readHallEffect();

  int readingPercentage = map(_hallEffectReadingG, _minHallEffectReadingG, _maxHallEffectReadingG, 0., 100.);
  return readingPercentage;
}


/////////////////////////////////////////////////
/////////////// MOTOR METHODS  //////////////////
/////////////////////////////////////////////////

void finger::driveMotor(int PWM) {

  // Deal with switching directions
  if (PWM > 0) {
    digitalWrite(_IN_1, HIGH);
    digitalWrite(_IN_2, LOW);
  } else if (PWM <= 0) {
    digitalWrite(_IN_1, LOW);
    digitalWrite(_IN_2, HIGH);
  }

  // Set the speed
  analogWrite(_EN, abs(PWM));
}

void finger::stopMotor() {
 
  digitalWrite(_IN_1, HIGH);
  digitalWrite(_IN_2, HIGH);
  
  analogWrite(_EN, 255);
}

void finger::releaseMotor() {
  
  digitalWrite(_IN_1, LOW);
  digitalWrite(_IN_2, LOW);
  
  analogWrite(_EN, 0);
}

void finger::calibrateMotor() {

  static int lastPos = -999;

  static int state = 0;

  switch(state){
    // you just started calibrating
    case 0:
      finger::driveMotor(200);
      if (_curPos > 5){
        state = 1;
      }
      break;
    // now, omega has started to update and you can rely on it
    case 1: 
      finger::driveMotor(200);
      if (_curOmega == 0){
        state = 2;
      }
      lastPos = _curPos;
      break;
    case 2:
      _calibrated = true;
      finger::releaseMotor();
      break;
  }  
  
}

/////////////////////////////////////////////////
/////////////// ENCODER METHODS /////////////////
/////////////////////////////////////////////////


// Function called by the ISRs to update the current position of the motor
void finger::updateCurState() {
  // an interrupt in the main script will call this. Based on the last channel
  // that was interrupted, you'll know which direction you spun

  // read both channels of the encoder
  bool a = digitalRead(_input_ch[0]);
  bool b = digitalRead(_input_ch[1]);

  // update state info
  _lastState = _curState;

  // logic based on behavior of the enoder. See spec sheet
  if (!a && !b) {
    _curState = 0;
  } else if (a && !b) {
    _curState = 1;
  } else if (a && b) {
    _curState = 2;
  } else if (!a && b) {
    _curState = 3;
  }

  finger::updateCurPos();
}

// splitting this from the updateCurState is useful bc:
// at a minimum, can help increase readability
// if the interrupt is found to be taking too long (hasn't yet), a flag can be
// set in updateCurState which will be handled by updateCurPos. If the flag is
// true, run updateCurPos and set the flag to false
void finger::updateCurPos() {

  // organize time data
  static unsigned int lastTime = millis();
  unsigned int curTime = millis();
  unsigned int dt = curTime - lastTime;
  lastTime = curTime;

  // cw is 0, 1, 2, 3
  switch (_curState) {
  case 0:
    if (_lastState == 3) {
      _curPos++;
      _curOmega = 1 / dt;
    } else if (_lastState == 1) {
      _curPos--;
      _curOmega = -1 / dt;
    } else if (_lastState == 0) {
      _curOmega = 0;
    }
    break;

  case 1:
    if (_lastState == 0) {
      _curPos++;
      _curOmega = 1 / dt;
    } else if (_lastState == 2) {
      _curPos--;
      _curOmega = -1 / dt;
    } else if (_lastState == 1) {
      _curOmega = 0;
    }
    break;

  case 2:
    if (_lastState == 1) {
      _curPos++;
      _curOmega = 1 / dt;
    } else if (_lastState == 3) {
      _curPos--;
      _curOmega = -1 / dt;
    } else if (_lastState == 2) {
      _curOmega = 0;
    }
    break;

  case 3:
    if (_lastState == 2) {
      _curPos++;
      _curOmega = 1 / dt;
    } else if (_lastState == 0) {
      _curPos--;
      _curOmega = -1 / dt;
    } else if (_lastState == 3) {
      _curOmega = 0;
    }
    break;
  }
}

/////////////////////////////////////////////////
/////////////// HELPER METHODS //////////////////
/////////////////////////////////////////////////

void finger::ticksToDeg() {}
