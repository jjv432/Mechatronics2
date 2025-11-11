#ifndef myDCEncoder_h
#define myDCEncoder_h

#include "Arduino.h"

class myDCEncoder
{
  public:
    myDCEncoder(int IN_1, int IN_2, int EN, int ch_a, int ch_b);
    void init();
    void driveMotor(int PWM);
    void updateCurState();
    void PID(int desPos);
    void ticksToDeg();
    int _curPos = 0; // in "ticks"
    int _curOmega = 0; // in tick/ms
    float _kp = 10.0;
    float _ki = 0.05;
    float _kd = 1.0;
  private:
    void updateCurPos();
    int _curState = 0;
    int _lastState = 0;
    int _IN_1;
    int _IN_2;
    int _EN;
    int _input_ch[2];
};

#endif
