#ifndef myDCEncoder_h
#define myDCEncoder_h

#include "Arduino.h"

class myDCEncoder {
public:
  myDCEncoder(int IN_1, int IN_2, int EN, int ch_a, int ch_b,
              int hallEffectPin);
  void init();
  void driveMotor(int PWM);
  void updateCurState();
  void PID(int desPos);
  void ticksToDeg();
  void stopMotor();
  void releaseMotor();
  void calibrateMotor();
  int _curPos = 0;   // in "ticks";
  int _curOmega = 0; // in tick/ms;
  float _kp = 10.0;
  float _ki = 0.00005;
  float _kd = 1.0;
  int _EN;
  int _IN_1;
  int _IN_2;
  int _hallEffectPin;
  float _hallEffectReading;
  float _hallEffectReadingG;

private:
  void updateCurPos();
  int _curState = 0;
  int _lastState = 0;
  bool _calibrated = 0;
  float _hallEffectVCC = 5.0;
  float _zeroField = _hallEffectVCC / 2;
  float _hallEffectSensitivity = .001;
  float _hallEffectResolution = _hallEffectVCC / 1023.0;

  int _input_ch[2];
};

#endif
