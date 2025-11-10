#include "src/myStepper/myStepper.h"      // doing it like this so git will keep up
#include "src/myDCEncoder/myDCEncoder.h"  // doing it like this so git will keep up

// Change this so that all these pins are definitions up top
// Two encoders
#define motorA_IN1 5
#define motorA_IN2 6
#define motorA_EN 9   // pwm pin
#define motorA_chA 2  // encoder
#define motorA_chB 3  // encoder

#define stepperIN1 40
#define stepperIN2 41
#define stepperIN3 42
#define stepperIN4 43

myDCEncoder motorA(motorA_IN1, motorA_IN2, motorA_EN, motorA_chA, motorA_chB);

myStepper mS(stepperIN1, stepperIN3, stepperIN2, stepperIN4);

int desPos1 = 400;
int desPos2 = -400; // roughly 180 degrees
int desPos = desPos1;

int direction = 1;
bool state = 0;

void setup() {

  mS.init();
  motorA.init();

  attachInterrupt(digitalPinToInterrupt(motorA_chA), ISR_motorA_chA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(motorA_chB), ISR_motorA_chB, CHANGE);

  Serial.begin(9600);
  delay(2000);
}

void loop() {
  
  switch (state){
    case 0: 
    if (abs(motorA._curPos - desPos1) < 8){
      state = !state;
      desPos = desPos2;
    }
    break;

    case 1: 
    if (abs(motorA._curPos - desPos2) < 8){
      state = !state;
      desPos = desPos1;
    }
    break;

    
  }

  motorA.PID(desPos);

  Serial.println(motorA._curPos);
}

void ISR_motorA_chA() {
  motorA.updateCurState();
}
void ISR_motorA_chB() {
  motorA.updateCurState();
}