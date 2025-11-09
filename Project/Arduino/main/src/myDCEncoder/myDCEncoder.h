#ifndef myStepper_h
#define myStepper_h

#include "Arduino.h"

class myStepper
{
  public:
    myStepper(int direction_pin, int ch_a, int ch_b, int ch_c, int ch_d);
    void init();
    void updateCurState(int channelNumber);
    int _curPos = 0; // in "ticks"
    int _curOmega = 0; // in deg/s
  private:
    int _direction_pin;
    int[4] _input_ch;
};

#endif
