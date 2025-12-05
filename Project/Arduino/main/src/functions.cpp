#include "functions.h"
#include "Arduino.h"
#include "constants.h"


/////////////////////////////////////////////////
///////////////// UI FUNCTIONS //////////////////
/////////////////////////////////////////////////

void initUIButton(){
  pinMode(buttonPin, INPUT_PULLUP);
}

bool readUIButton(){
  int dt = 50;
  bool buttonState = digitalRead(buttonPin);
  static bool lastButtonState = 0;


  if (lastButtonState && !buttonState){ // falling edge
    delay(dt);
    lastButtonState = buttonState;
    return true; // button is pressed;
  }
  else if (!lastButtonState && buttonState){ // rising edge
    delay(dt);
    lastButtonState = buttonState;
    return false;
  }
  else if (lastButtonState == buttonState && buttonState == 0){
    return true;
  }
  else
    return false;
}


/////////////////////////////////////////////////
///////////////// IR FUNCTIONS //////////////////
/////////////////////////////////////////////////

static unsigned long lastInterruptTime = 0;
unsigned long curInterruptTime = 0;

char identifyObjectIR(int inputPin){
  // this function is a naive assumption that the output of the IR identification circuit
  // will be a 5Vpp signal with varying frequency.
  attachInterrupt(digitalPinToInterrupt(inputPin), IR_ISR, RISING);
  // may need to add a delay here to allow saturation/ config time

  // wait for the interrupt to update
  // should garuntee 2 interrupts to occur
  while (curInterruptTime == lastInterruptTime && lastInterruptTime != 0){}

  unsigned long dt = curInterruptTime - lastInterruptTime;

  // (d) Duck: 500Hz; (p) Pen: 1kHz; (t) Tetrahedron: 2 kHz
  // (d) Duck: 2000us; (p) Pen: 1000us; (t) Tetrahedron: 500us
  // gates: 
  //  (d): > 1500us:
  //  (p): <= 1500us, > 750us: 
  //  (t): <= 750us

  if (dt > 2500){
    return 'U'; // unknown;
  }
  else if (dt > 1500){
    return 'd';
  }
  else if(dt > 750){
    return 'p';
  }
  else
    return 't';

  // disable the interrupt to avoid interfering with encoders
  detachInterrupt(digitalPinToInterrupt(inputPin));
  lastInterruptTime = 0;
  curInterruptTime = 0;

}

void IR_ISR(){
  lastInterruptTime = curInterruptTime;
  curInterruptTime = micros();
}


/////////////////////////////////////////////////
///////////////// INTERRUPTS ////////////////////
/////////////////////////////////////////////////


void ISR_motorA_chA() {
  fingerA.updateCurState();
}
void ISR_motorA_chB() {
  fingerA.updateCurState();
}

void ISR_motorB_chA() {
  fingerB.updateCurState();
}
void ISR_motorB_chB() {
  fingerB.updateCurState();
}

void ISR_motorC_chA() {
  fingerC.updateCurState();
}
void ISR_motorC_chB() {
  fingerC.updateCurState();
}


/////////////////////////////////////////////////
//////////////// LED FUNCTIONS //////////////////
/////////////////////////////////////////////////

void enableLED(){
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
}
void updateLed(int state){
  switch(state){
    case 0: 
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, LOW);
      break;
    case 1: 
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);
      break;
    case 2: 
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, HIGH);
      break;
    default: 
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, LOW);
      break;
  }

}