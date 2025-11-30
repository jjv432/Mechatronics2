#include "src/finger/finger.h"  // doing it like this so git will keep up
#include "src/functions.h"

// finger A
#define fingerA_IN1 53
#define fingerA_IN2 52
#define fingerA_EN 46  // pwm pin
#define fingerA_chA 18 // encoder
#define fingerA_chB 19 // encoder
#define fingerA_hallEffectPin A0

// // finger B
// #define fingerB_IN1 6
// #define fingerB_IN2 7
// #define fingerB_EN 8    // pwm pin
// #define fingerB_chA 9   // encoder
// #define fingerB_chB 10  // encoder
// #define fingerB_hallEffectPin 17

// // finger C
// #define fingerC_IN1 11
// #define fingerC_IN2 12
// #define fingerC_EN 13   // pwm pin
// #define fingerC_chA 14  // encoder
// #define fingerC_chB 15  // encoder
// #define fingerC_hallEffectPin 18


// finger general
float minHall = 0;
float maxHall = -900;

finger fingerA(fingerA_IN1, fingerA_IN2, fingerA_EN, fingerA_chA, fingerA_chB, fingerA_hallEffectPin, minHall, maxHall);
// finger fingerB(fingerB_IN1, fingerB_IN2, fingerB_EN, fingerB_chA, fingerB_chB, fingerB_hallEffectPin, minHall, maxHall);
// finger fingerC(fingerC_IN1, fingerC_IN2, fingerC_EN, fingerC_chA, fingerC_chB, fingerC_hallEffectPin, minHall, maxHall);

// // UI
// #define buttonPin 9

// // IR
// #define IRPin 30

// general
int state = 0;
char curObject = 'U';

void setup() {
  
  fingerA.init();
  // fingerB.init();
  // fingerC.init();

  // initUIButton(buttonPin);

  attachInterrupt(digitalPinToInterrupt(fingerA_chA), ISR_motorA_chA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(fingerA_chB), ISR_motorA_chB, CHANGE);
  Serial.begin(9600);
  // delay(1000);

  // TEST HALL EFFECT
  fingerA._minHallEffectReadingG = -61.0;
  fingerA._maxHallEffectReadingG = -564.0;
}


// TEST HALL EFFECT

void loop(){
  fingerA.readHallEffect();
  Serial.println(fingerA.getHallEffectCompression());
  // fingerA.readHallEffect();
  // Serial.println(fingerA._hallEffectReadingG);
}


// TEST CALIBRATION
/*

bool stateCalib = 0;
// testing
void loop(){

  switch (stateCalib){
    case 0: 
      fingerA._calibrated = 0;
      delay(1000);
      stateCalib = 1;
      break;
    case 1:
      fingerA.calibrateMotor();
      break;
  }
  
  // fingerA.PID(0);

  Serial.println(fingerA._calibrated);
 
}

*/

void ISR_motorA_chA() {
  fingerA.updateCurState();
}
void ISR_motorA_chB() {
  fingerA.updateCurState();
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