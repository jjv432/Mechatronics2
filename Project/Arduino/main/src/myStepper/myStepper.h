#ifndef myStepper_h
#define myStepper_h

#include "Arduino.h"

class myStepper
{
  public:
    myStepper(int pin_a, int pin_b, int pin_c, int pin_d);
    void init();
    void goToDesPos(int desPos);
    int _pin_array[4];
    int _curPos;
};

#endif
