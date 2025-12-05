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
#define fA_hallEffectPin A0

inline finger fingerA(fA_IN1, fA_IN2, fA_EN, fA_chA, fA_chB, fA_hallEffectPin);

// finger B
#define fB_IN1 6
#define fB_IN2 7
#define fB_EN 8
#define fB_chA 16
#define fB_chB 17
#define fB_hallEffectPin 11

inline finger fingerB(fB_IN1, fB_IN2, fB_EN, fB_chA, fB_chB, fB_hallEffectPin);

// finger C
#define fC_IN1 5
#define fC_IN2 4
#define fC_EN 3
#define fC_chA 19
#define fC_chB 18
#define fC_hallEffectPin 10

inline finger fingerC(fC_IN1, fC_IN2, fC_EN, fC_chA, fC_chB, fC_hallEffectPin);
// ...


#endif
