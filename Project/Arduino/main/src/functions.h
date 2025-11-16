#ifndef functions_h
#define functions_h

#include "Arduino.h"

char identifyObjectIR(int inputPin);
void IR_ISR();
bool readUIButton(int buttonPin);
void initUIButton(int buttonPin);


#endif
