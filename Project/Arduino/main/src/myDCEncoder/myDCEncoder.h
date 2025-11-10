#ifndef myDCEncoder_h
#define myDCEncoder_h

#include "Arduino.h"

class myDCEncoder
{
  public:
    myDCEncoder(int IN_1, int IN_2, int EN, int ch_a, int ch_b, int ch_c, int ch_d);
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
    static myDCEncoder* _instances[4];  // static lookup table
    static void ISR_0();
    static void ISR_1();
    static void ISR_2();
    static void ISR_3();
    int _IN_1;
    int _IN_2;
    int _EN;
    int _input_ch[4];
};

#endif
