#ifndef constants.h
#define constants.h

#include "Arduino.h"
#include "finger/finger.h"

/////////////////////////////////////////////////
/////////////////// MISC ////////////////////////
/////////////////////////////////////////////////

// UI
#define buttonPin 9
#define IRPin 30

// LED
#define LED_RED 33
#define LED_GREEN 34



/////////////////////////////////////////////////
///////////////// FINGERS ///////////////////////
/////////////////////////////////////////////////

// NOTE: inline makes it so we can treat these values as global vars
// ENCODERS: 14&15, 16&17, 18&19
// MOTORS: 0-9
// HE: 10-12

// finger A
#define fA_IN1 52
#define fA_IN2 53
#define fA_EN 45
#define fA_chA 21
#define fA_chB 20
#define fA_hallEffectPin A1

inline finger fingerA(fA_IN1, fA_IN2, fA_EN, fA_chA, fA_chB, fA_hallEffectPin);

// finger B
#define fB_IN1 50
#define fB_IN2 51
#define fB_EN 44
#define fB_chA 19
#define fB_chB 18
#define fB_hallEffectPin A0

inline finger fingerB(fB_IN1, fB_IN2, fB_EN, fB_chA, fB_chB, fB_hallEffectPin);

// finger C
#define fC_IN1 47
#define fC_IN2 48
#define fC_EN 46
#define fC_chA 2
#define fC_chB 3
#define fC_hallEffectPin A2

inline finger fingerC(fC_IN1, fC_IN2, fC_EN, fC_chA, fC_chB, fC_hallEffectPin);
// ...


#endif
