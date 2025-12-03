#include "src/finger/finger.h"  // doing it like this so git will keep up
#include "src/constants.h"
#include "src/functions.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DEBUG_STATE 0  // 0= normal operation, 1= print hall effect, 2= print UI button, 3= test calibration

// general
int state = 30;
char curObject = 'U';

void setup() {

  fingerA.init();

  initUIButton();

  attachInterrupt(digitalPinToInterrupt(fA_chA), ISR_motorA_chA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(fA_chB), ISR_motorA_chB, CHANGE);
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  // TEST HALL EFFECT
  fingerA._minHallEffectReadingG = 0;  //MUST BE MAXIMUM ABSOLUTE VALUE OF STEADY STATE
  fingerA._maxHallEffectReadingG = -780.0;

  // TEST SINGEL FINGER PROGRESSION
  fingerA._curPos = 10;

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
  }
}

/////////////////////////////////////////////////
//////////////// NORMAL LOOP ////////////////////
/////////////////////////////////////////////////
// TEST SINGLE FINGER PROGRESSION
void loopNormal() {
  Serial.println(state);

  switch (state) {
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

  printState(state);
}

void printState(int curState) {
  static int lastState = 0;
  if (curState != lastState) {
    lcd.print(state);
    lastState = curState;
  }
}

/////////////////////////////////////////////////
//////////////// DEBUG LOOPS ////////////////////
/////////////////////////////////////////////////

void loopCalibrationTest() {
  char desiredFinger = 'a';
  finger* curFinger = nullptr;

  switch (desiredFinger) {
    case 'a':
      curFinger = &fingerA;
      break;
  }

  static bool stateCalib = 0;
  switch (stateCalib) {
    case 0:
      curFinger->_calibrated = 0;
      delay(1000);
      stateCalib = 1;
      break;
    case 1:
      curFinger->calibrateMotor();
      break;
  }

  Serial.println(curFinger->_calibrated);
}

void loopUIButton() {
  Serial.println(readUIButton());
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





/*

void loop() {
  switch (state) {
    case 0:  // idle
      // open all fingers
      fingerA.PID(0);
      fingerB.PID(0);
      fingerC.PID(0);

      // reset variables
      curObject = 'U';

      //read the button
      if (readUIButton(buttonPin)) {
        state = 10;
      }
      break;

    case 10:  // Start identifying the objects

      // if the object is unidentified
      if (curObject == 'U') {
        curObject = identifyObjectIR(IRPin);
      } else
        state = 15;

      break;

    case 15:  // move the z
      // assuming this will be controlled externally
      //read the button
      if (readUIButton(buttonPin)) {
        state = 20;
      }
      break;

    case 20:  // barely touch the object with each finger
      bool aTouching = fingerA.touchObject();
      bool bTouching = fingerB.touchObject();
      bool cTouching = fingerC.touchObject();

      if (aTouching && bTouching && cTouching) {
        state = 30;
      }
      break;

    case 30:                            // fully grasp the object
      const int graspCompression = 50;  // percent

      fingerA.graspObject(graspCompression);
      fingerB.graspObject(graspCompression);
      fingerC.graspObject(graspCompression);

      // if they've all got a stable grasp on the object...
      if (fingerA._PIDStationary && fingerB._PIDStationary && fingerC._PIDStationary) {
        state = 40;
      }

      break;

    case 40:  // move the object to its drop location
      if (readUIButton(buttonPin)) {
        state = 50;
      }

      break;

    case 50:  // drop the object

      // open all fingers
      fingerA.PID(0);
      fingerB.PID(0);
      fingerC.PID(0);

      // When they're all fully open
      if (fingerA._PIDStationary && fingerB._PIDStationary && fingerC._PIDStationary) {
        state = 0;
      }
  }
}

 */