#include "src/finger/finger.h"  // doing it like this so git will keep up
#include "src/constants.h"
#include "src/functions.h"

// 0= normal operation, 1= print hall effect, 2= print UI button, 3= test calibration, 4 = test PID, 5 = test encoder direction, 6 = test motor direction, 7= pin test, 8 = test led

#define DEBUG_STATE 3
#define DEBUG_FINGER 'b'

static finger* curFinger = nullptr;

// general
int state = -31;
char curObject = 'U';

void setup() {

  //***** fingers
  fingerA.init();
  attachInterrupt(digitalPinToInterrupt(fA_chA), ISR_motorA_chA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(fA_chB), ISR_motorA_chB, CHANGE);

  fingerB.init();
  attachInterrupt(digitalPinToInterrupt(fB_chA), ISR_motorB_chA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(fB_chB), ISR_motorB_chB, CHANGE);

  fingerC.init();
  attachInterrupt(digitalPinToInterrupt(fC_chA), ISR_motorC_chA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(fC_chB), ISR_motorC_chB, CHANGE);

  // Make this inside the noise of each boundary
  fingerA._minHallEffectReadingG = -72;
  fingerA._maxHallEffectReadingG = -840;

  fingerB._minHallEffectReadingG = -58;
  fingerB._maxHallEffectReadingG = -710;

  fingerC._minHallEffectReadingG = -70;
  fingerC._maxHallEffectReadingG = -440;

  //**** Misc
  initUIButton();
  Serial.begin(9600);
  enableLED();

  if (DEBUG_STATE != 0) {
    switch (DEBUG_FINGER) {
      case 'a':
        curFinger = &fingerA;
        break;
      case 'b':
        curFinger = &fingerB;
        break;
      case 'c':
        curFinger = &fingerC;
        break;
    }
  }

  // Stop output bouncing
  delay(1000);
}


void loop() {
  if (DEBUG_STATE == 0) {
    loopNormal();
  } else if (DEBUG_STATE == 1) {
    loopHallEffect();
  } else if (DEBUG_STATE == 2) {
    loopUIButton();
  } else if (DEBUG_STATE == 3) {
    loopCalibrationTest();
  } else if (DEBUG_STATE == 4) {
    loopPIDTest();
  } else if (DEBUG_STATE == 5) {
    loopEncoderTest();
  } else if (DEBUG_STATE == 6) {
    loopMotorTest();
  } else if (DEBUG_STATE == 7) {
    loopPinTest();
  } else if (DEBUG_STATE == 8) {
    loopLEDTest();
  }
}


/////////////////////////////////////////////////
//////////////// NORMAL LOOP ////////////////////
/////////////////////////////////////////////////

void loopNormal() {

  const int dt1 = 1500;
  const int dt0 = 500;

  switch (state) {
    case -31: 
      delay(dt0);
      fingerA._calibrated = 0;
      fingerA._curPos = 0;
      state = -3;
      break;
    case -3:  // calibrate A
      delay(dt0);
      fingerA.calibrateMotor();
      if (fingerA._calibrated) {
        state = -21;
        Serial.println("A Calibrated");
        delay(dt1);
      }
      break;
    case -21: 
      delay(dt0);
      fingerB._calibrated = 0;
      fingerB._curPos = 0;
      state = -2;
      break;
    case -2:  // calibrate A
      delay(dt0);
      fingerB.calibrateMotor();
      if (fingerB._calibrated) {
        state = -11;
        Serial.println("B Calibrated");
        delay(dt1);
      }
      break;
    case -11: 
      delay(dt0);
      fingerC._calibrated = 0;
      fingerC._curPos = 0;
      state = -1;
      break;
    case -1:  // calibrate A
      delay(dt0);
      fingerC.calibrateMotor();
      if (fingerC._calibrated) {
        state = 0;
        Serial.println("C Calibrated");
        delay(dt1);
      }
      break;

    case 0:  // idle
      // open all fingers

      fingerA.PID(0);
      fingerB.PID(0);
      fingerC.PID(0);

      // reset variables
      curObject = 'U';

      //read the button
      if (readUIButton() == 1) {
        state = 20;
        Serial.println("State: Trying to touch object");
      }
      break;

    case 20:
      {
        bool aTouching = fingerA.touchObject();
        bool bTouching = fingerB.touchObject();
        bool cTouching = fingerC.touchObject();

        if (aTouching && bTouching && cTouching) {
          state = 30;
          Serial.println("State: Trying to grasp object");
          fingerA._flushGO = 1;
          fingerB._flushGO = 1;
          fingerC._flushGO = 1;
        }
        break;

      }  // barely touch the object with each finger


    case 30:
      {                             // fully grasp the object
        int graspCompression = 40;  // percent

        fingerA.graspObject(graspCompression);
        fingerB.graspObject(graspCompression);
        fingerC.graspObject(graspCompression);

        if (readUIButton()) {
          state = 50;
          Serial.println("State: Dropping object");
        }

        break;
      }

    case 50:  // drop the object

      // open all fingers
      fingerA.PID(150);
      fingerB.PID(150);
      fingerC.PID(150);
      Serial.print(fingerA._curPos);
      Serial.print("\t");
      Serial.print(fingerB._curPos);
      Serial.print("\t");
      Serial.print(fingerC._curPos);
      Serial.print("\t");

      // When they're all fully open
      if (fingerA._curOmega == 0 && fingerB._curOmega == 0 && fingerC._curOmega == 0) {
        state = 0;
        fingerA._flushI = 1;
        fingerA._flushTO = 1;
        fingerB._flushI = 1;
        fingerB._flushTO = 1;
        fingerC._flushI = 1;
        fingerC._flushTO = 1;
        Serial.println("State: Open grasp");
      }
      break;
  }

  // printState(state);
}

void printState(int curState) {
  static int lastState = 0;
  if (curState != lastState) {
    Serial.print(state);
    lastState = curState;
  }
}

/////////////////////////////////////////////////
//////////////// DEBUG LOOPS ////////////////////
/////////////////////////////////////////////////

// 1: PRINT HALL EFFECT

void loopHallEffect() {
  int compression = 0;
  int voltage = 0;
  float reading = 0;

  compression = curFinger->getHallEffectCompression();
  curFinger->readHallEffect();
  reading = curFinger->_hallEffectReadingG;
  voltage = curFinger->_hallEffectReadingV;

  Serial.print("Current compression (%): ");
  Serial.print(compression);
  Serial.print("\t");
  Serial.print("Current Reading (G): ");
  Serial.print(reading);
  Serial.print("\t");
  Serial.print("Current Reading (V): ");
  Serial.println(voltage);
}

// 2: PRINT UI BUTTON

void loopUIButton() {
  Serial.println(readUIButton());
}

// 3: TEST CALIBRATION

void loopCalibrationTest() {

  static int stateCalib = 0;
  switch (stateCalib) {
    case 0:
      curFinger->_calibrated = 0;
      delay(100);
      stateCalib = 1;
      break;
    case 1:
      delay(100);
      curFinger->calibrateMotor();
      if (curFinger->_calibrated == 1) {
        stateCalib = 2;
        Serial.println("Calibration Complete");
      }
      break;
    case 2:
      // delay(2000);
      break;
  }

  // Serial.println(curFinger->_curPos);
  // Serial.println(stateCalib);
}

// 4: TEST PID
void loopPIDTest() {

  const int maxPos = 400;
  const int minPos = 0;

  const int switchTime = 1500;
  static int desPos = maxPos;
  static unsigned int lastSwitchTime = 0;
  unsigned int dt = millis() - lastSwitchTime;

  if (dt > switchTime) {
    desPos = minPos * (desPos != minPos) + maxPos * (desPos != maxPos);
    lastSwitchTime = millis();
  }

  curFinger->PID(desPos);
  Serial.println(curFinger->_curPos);
}


// 5: TEST ENCODER DIRECTION

void loopEncoderTest() {
  Serial.print(curFinger->_curPos);
  Serial.print("\t");
  Serial.print(digitalRead(curFinger->_ch_a));
  Serial.print("\t");
  Serial.print(digitalRead(curFinger->_ch_b));
  Serial.print("\t");
  Serial.println("SWITCH CHA and CHB IF MOVING TOWARDS CENTER DECREASES CURPOS");
}

// 6: TEST MOTOR DIRECTION

void loopMotorTest() {

  const int commandPWM = 200;
  curFinger->driveMotor(commandPWM);

  Serial.print("Commanded PWM: ");
  Serial.print(commandPWM);
  Serial.print("\t");
  Serial.println("SWITCH IN1 and IN2 IF NOT MOVING TOWARDS CENTER");
}

// 7: PIN TEST

void loopPinTest() {
  static bool ran = false;
  int desPin = 23;
  if (ran == 0) {
    pinMode(desPin, INPUT_PULLUP);
    ran = 1;
  }
  Serial.println(digitalRead(desPin));
  delay(100);
}

// 8: TEST LED

void loopLEDTest() {
  static unsigned int state = 0;

  updateLED(state % 4);
  delay(1000);
  state++;
}
