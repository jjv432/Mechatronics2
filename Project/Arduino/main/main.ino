#include "src/myStepper/myStepper.h"      // doing it like this so git will keep up
#include "src/myDCEncoder/myDCEncoder.h"  // doing it like this so git will keep up

// Change this so that all these pins are definitions up top
// Two encoders
myDCEncoder leftMotor(5, 6, 9, 2, 3);

// Global ISR wrappers
void ISR_leftA() {
  leftMotor.updateCurState();
}
void ISR_leftB() {
  leftMotor.updateCurState();
}


myStepper mS(1, 3, 2, 4);
int desPos = 1000;

void setup() {

  mS.init();
  leftMotor.init();

  attachInterrupt(digitalPinToInterrupt(2), ISR_leftA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), ISR_leftB, CHANGE);

}

void loop() {
  // Simple test: move back and forth between two positions
  desPos = 200;
  while (mS._curPos != desPos) {
    mS.goToDesPos(desPos);  // Move toward position 10
  }

  desPos = 0;

  while (mS._curPos != desPos) {
    mS.goToDesPos(desPos);  // Move toward position 10
  }
}