#ifndef myStepper_h
#define myStepper_h

#include "Arduino.h"

class myStepper
{
  public:
    myStepper(int direction_pin, int ch_a, int ch_b, int ch_c, int ch_d);
    void init();
    void driveMotor(int PWM);
    void PID(int desPos);
    void updateCurState(int channelNumber);
    int _curPos = 0; // in "ticks"
    int _curOmega = 0; // in deg/s
    float _kp = 1.0;
    float _ki = 1.0;
    float _kd = 1.0;
  private:
    void ISR_0();
    void ISR_1();
    void ISR_2();
    void ISR_3();
    int _IN_1;
    int _IN_2;
    int _EN;
    int[4] _input_ch;
};

#endif
