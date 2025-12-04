#include "src/finger/finger.h"  // doing it like this so git will keep up
#include "src/constants.h"
#include "src/functions.h"
// #include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x27, 16, 2);

// 0= normal operation, 1= print hall effect, 2= print UI button, 3= test calibration, 4 = test PID, 5 = test encoder direction, 6 = test motor direction
#define DEBUG_STATE 2

// general
int state = -1;
char curObject = 'U';

void setup() {

  //***** fingers
  fingerA.init();
  attachInterrupt(digitalPinToInterrupt(fA_chA), ISR_motorA_chA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(fA_chB), ISR_motorA_chB, CHANGE);

  // Make this inside the noise of each boundary
  fingerA._minHallEffectReadingG = -65;
  fingerA._maxHallEffectReadingG = -1140;

  //**** Misc
  initUIButton();
  Serial.begin(9600);
  // lcd.init();
  // lcd.backlight();

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
  }else if (DEBUG_STATE == 5) {
    loopEncoderTest();
  }else if (DEBUG_STATE == 6) {
    loopMotorTest();
  }
}

/////////////////////////////////////////////////
//////////////// NORMAL LOOP ////////////////////
/////////////////////////////////////////////////

void loopNormal() {
  Serial.println(state);
  // Serial.println(fingerA._calibrated);
  // Serial.println(fingerA._curOmega);

  switch (state) {
    case -1:  // calibrate
      if (fingerA._calibrated == 0) {
        fingerA.calibrateMotor();
      }

      if (fingerA._calibrated) {
        state = 0;
      }
      break;
    case 0:  // idle
      // open all fingers

      fingerA.PID(0);

      // reset variables
      curObject = 'U';

      //read the button
      if (readUIButton()) {
        state = 20;
      }
      break;

    case 20:
      {
        bool aTouching = fingerA.touchObject();

        if (aTouching) {
          state = 30;
        }
        break;

      }  // barely touch the object with each finger


    case 30:
      {                             // fully grasp the object
        int graspCompression = 50;  // percent

        fingerA.graspObject(graspCompression);

        if (readUIButton()) {
          state = 50;
        }

        break;
      }

    case 50:  // drop the object

      // open all fingers
      fingerA.PID(0);

      // When they're all fully open
      if (fingerA._PIDStationary) {
        state = 0;
      }
      break;
  }

  // printState(state);
}

// void printState(int curState) {
//   static int lastState = 0;
//   if (curState != lastState) {
//     lcd.print(state);
//     lastState = curState;
//   }
// }

/////////////////////////////////////////////////
//////////////// DEBUG LOOPS ////////////////////
/////////////////////////////////////////////////

void loopPIDTest() {
  char desiredFinger = 'a';
  finger* curFinger = nullptr;

  switch (desiredFinger) {
    case 'a':
      curFinger = &fingerA;
      break;
  }

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

void loopCalibrationTest() {
  char desiredFinger = 'a';
  finger* curFinger = nullptr;

  switch (desiredFinger) {
    case 'a':
      curFinger = &fingerA;
      break;
  }

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
      if (curFinger->_calibrated == 1){
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

void loopUIButton() {
  Serial.println(readUIButton());
}

void loopEncoderTest() {
  char desiredFinger = 'a';

  int compression = 0;
  float reading = 0;

  finger* curFinger = nullptr;

  switch (desiredFinger) {
    case 'a':
      curFinger = &fingerA;
      break;
  }

  Serial.print(curFinger->_curPos);
  Serial.print("\t");
  Serial.println("SWITCH CHA and CHB IF MOVING TOWARDS CENTER DECREASES CURPOS");
}

void loopMotorTest() {
  char desiredFinger = 'a';

  int compression = 0;
  float reading = 0;

  finger* curFinger = nullptr;

  switch (desiredFinger) {
    case 'a':
      curFinger = &fingerA;
      break;
  }

  const int commandPWM = 100;
  curFinger->driveMotor(commandPWM);

  Serial.print("Commanded PWM: ");
  Serial.print(commandPWM);
  Serial.print("\t");  
  Serial.println("SWITCH IN1 and IN2 IF NOT MOVING TOWARDS CENTER");
}

void loopHallEffect() {
  char desiredFinger = 'a';

  int compression = 0;
  float reading = 0;

  finger* curFinger = nullptr;

  switch (desiredFinger) {
    case 'a':
      curFinger = &fingerA;
      break;
  }

  compression = curFinger->getHallEffectCompression();
  curFinger->readHallEffect();
  reading = curFinger->_hallEffectReadingG;

  Serial.print("Current compression (%): ");
  Serial.print(compression);
  Serial.print("\t");
  Serial.print("Current Reading (G): ");
  Serial.println(reading);
}