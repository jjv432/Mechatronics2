#include "functions.h"
#include "Arduino.h"


/////////////////////////////////////////////////
////////////// HELPER FUNCTIONS /////////////////
/////////////////////////////////////////////////



/////////////////////////////////////////////////
///////////////// UI FUNCTIONS //////////////////
/////////////////////////////////////////////////
static bool lastButtonState = 1;
static unsigned long lastButtonTime = 0;
int dt = 20;

void initUIButton(int buttonPin){
  pinMode(buttonPin, INPUT_PULLUP);
}

bool readUIButton(int buttonPin){
  bool buttonState = digitalRead(buttonPin);

  // don't react if not enough time has passed
  if (millis() - lastButtonTime > dt){
    // look for a falling edge
    if (lastButtonState && !buttonState){
      return true; // button is pressed;
      lastButtonTime = millis();
    }
    // rising edge
    else if (!lastButtonState && !buttonState){
      lastButtonTime = millis();
      return false;
    }
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

}

void IR_ISR(){
  lastInterruptTime = curInterruptTime;
  curInterruptTime = micros();
}