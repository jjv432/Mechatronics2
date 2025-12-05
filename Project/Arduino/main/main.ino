#include "src/finger/finger.h"  // doing it like this so git will keep up
#include "src/constants.h"
#include "src/functions.h"
// #include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x27, 16, 2);

// 0= normal operation, 1= print hall effect, 2= print UI button, 3= test calibration, 4 = test PID, 5 = test encoder direction, 6 = test motor direction, 7= pin test, 8 = test led
#define DEBUG_STATE 5
#define DEBUG_FINGER 'b'

static finger* curFinger = nullptr;


// general
int state = -1;
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
  // fingerA._minHallEffectReadingG = -65;
  // fingerA._maxHallEffectReadingG = -800;

  // fingerB._minHallEffectReadingG = -65;
  // fingerB._maxHallEffectReadingG = -800;

  //**** Misc
  initUIButton();
  Serial.begin(9600);
  enableLED();
  // lcd.init();
  // lcd.backlight();

  // Stop output bouncing
  delay(1000);

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

void loopLEDTest(){
  static unsigned int state = 0;

  updateLED(state % 4);
  delay(1000);
  state ++;
}
void loopPinTest() {
  noInterrupts();
  static bool ran = false;
  int desPin = 23;
  if (ran == 0) {

    pinMode(desPin, INPUT_PULLUP);
    ran = 1;
  }
  Serial.println(digitalRead(desPin));
  delay(100);
  interrupts();
}

/////////////////////////////////////////////////
//////////////// NORMAL LOOP ////////////////////
/////////////////////////////////////////////////

void loopNormal() {

  // Serial.println(state);


  switch (state) {
    case -1:  // calibrate
      if (fingerA._calibrated == 0) {
        fingerA.calibrateMotor();
      }

      if (fingerA._calibrated) {
        state = 0;
        delay(200);
        Serial.println("State: Calibrated, awaiting UI");
      }
      Serial.println("State: Uncalibrated");
      break;
    case 0:  // idle
      // open all fingers

      fingerA.PID(0);

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

        if (aTouching) {
          state = 30;
          Serial.println("State: Trying to grasp object");
          fingerA._flushGO = 1;
        }
        break;

      }  // barely touch the object with each finger


    case 30:
      {                             // fully grasp the object
        int graspCompression = 40;  // percent

        fingerA.graspObject(graspCompression);

        if (readUIButton()) {
          state = 50;
          Serial.println("State: Dropping object");
        }

        break;
      }

    case 50:  // drop the object

      // open all fingers
      fingerA.PID(150);
      Serial.println(fingerA._curPos);

      // When they're all fully open
      if (fingerA._curOmega == 0) {
        state = 0;
        fingerA._flushI = 1;
        fingerA._flushTO = 1;
        Serial.println("State: Open grasp");
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

void loopUIButton() {
  Serial.println(readUIButton());
}

void loopEncoderTest() {
  Serial.print(curFinger->_curPos);
  Serial.print("\t");
  Serial.print(digitalRead(curFinger->_ch_a));
  Serial.print("\t");
  Serial.print(digitalRead(curFinger->_ch_b));
  Serial.print("\t");
  Serial.println("SWITCH CHA and CHB IF MOVING TOWARDS CENTER DECREASES CURPOS");
}

void loopMotorTest() {


  const int commandPWM = 200;
  curFinger->driveMotor(commandPWM);

  Serial.print("Commanded PWM: ");
  Serial.print(commandPWM);
  Serial.print("\t");
  Serial.println("SWITCH IN1 and IN2 IF NOT MOVING TOWARDS CENTER");
}

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