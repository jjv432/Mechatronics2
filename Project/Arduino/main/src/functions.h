#ifndef functions_h
#define functions_h

#include "Arduino.h"

char identifyObjectIR(int inputPin);
void IR_ISR();
bool readUIButton();
void initUIButton();
void ISR_motorA_chA();
void ISR_motorA_chB();

#endif
