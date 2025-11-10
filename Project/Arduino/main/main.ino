#include "src/myStepper/myStepper.h"      // doing it like this so git will keep up
#include "src/myDCEncoder/myDCEncoder.h"  // doing it like this so git will keep up

// Change this so that all these pins are definitions up top
// Two encoders
#define motorA_IN1 5
#define motorA_IN2 6
#define motorA_EN 9 // pwm pin
#define motorA_chA 2 // encoder
#define motorA_chB 3 // encoder

#define stepperIN1 40
#define stepperIN2 41
#define stepperIN3 42
#define stepperIN4 43

myDCEncoder motorA(motorA_IN1, motorA_IN2, motorA_EN, motorA_chA, motorA_chB);

myStepper mS(stepperIN1, stepperIN3, stepperIN2, stepperIN4);
int desPos = 1000;

void setup() {

  mS.init();
  motorA.init();

  attachInterrupt(digitalPinToInterrupt(motorA_chA), ISR_motorA_chA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(motorA_chB), ISR_motorA_chB, CHANGE);

  Serial.begin(9600);
}

void loop() {

  Serial.println(motorA._curPos);
  // Simple test: move back and forth between two positions
  // desPos = 200;
  // while (mS._curPos != desPos) {
  //   mS.goToDesPos(desPos);  // Move toward position 10
  // }

  // desPos = 0;

  // while (mS._curPos != desPos) {
  //   mS.goToDesPos(desPos);  // Move toward position 10
  // }
}

void ISR_motorA_chA() {
  motorA.updateCurState();
}
void ISR_motorA_chB() {
  motorA.updateCurState();
}