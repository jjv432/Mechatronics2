#ifndef finger_h
#define finger_h

#include "Arduino.h"

class finger {
public:
  finger(int IN_1, int IN_2, int EN, int ch_a, int ch_b, int hallEffectPin);

  // methods
  void init();
  void driveMotor(int PWM);
  void updateCurState();
  void PID(int desPos);
  void ticksToDeg();
  void stopMotor();
  void releaseMotor();
  void calibrateMotor();
  void readHallEffect();
  bool touchObject();
  unsigned int getHallEffectCompression();
  bool graspObject(int desCompression);

  // properties
  int _curPos = 0;   // in "ticks";
  int _curOmega = 0; // in tick/ms;
  float _kp = 0.5;
  float _ki = 0.000006;//1;
  float _kd = 0.0008;
  int _EN;
  int _IN_1;
  int _IN_2;
  int _hallEffectPin;
  float _hallEffectReadingV;
  float _hallEffectReadingG = -999;
  float _minHallEffectReadingG = 0;
  float _maxHallEffectReadingG = -400;
  bool _PIDStationary = false;
  bool _calibrated = false;
  bool _flushI = false;
  bool _flushTO = false;
  bool _flushGO = false;

  int _ch_a;
  int _ch_b;

private:
  static void updateISR();

  // methods
  void updateCurPos();

  // properties
  int _curState = 0;
  int _lastState = 0;
  
  float _hallEffectVCC = 5;
  float _zeroField = _hallEffectVCC / 2;
  float _hallEffectSensitivity = .001;
  float _hallEffectResolution = _hallEffectVCC / 1023.0;

  
};

#endif
