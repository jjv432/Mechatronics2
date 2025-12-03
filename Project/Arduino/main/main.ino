#include "src/finger/finger.h"  // doing it like this so git will keep up
#include "src/constants.h"
#include "src/functions.h"


// general
int state = 30;
char curObject = 'U';

void setup() {

  fingerA.init();

  initUIButton();

  attachInterrupt(digitalPinToInterrupt(fA_chA), ISR_motorA_chA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(fA_chB), ISR_motorA_chB, CHANGE);
  Serial.begin(9600);

  // TEST HALL EFFECT
  fingerA._minHallEffectReadingG = 0;  //MUST BE MAXIMUM ABSOLUTE VALUE OF STEADY STATE
  fingerA._maxHallEffectReadingG = -780.0;

  // TEST SINGEL FINGER PROGRESSION
  fingerA._curPos = 10;

  delay(1000);
}


// // TEST SINGLE FINGER PROGRESSION
void loop() {
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
}





// TEST HALL EFFECT

// void loop(){
//   // fingerA.readHallEffect();
//   Serial.println(fingerA.getHallEffectCompression());
//   // fingerA.readHallEffect();
//   // Serial.println(fingerA._hallEffectReadingG);
// }


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