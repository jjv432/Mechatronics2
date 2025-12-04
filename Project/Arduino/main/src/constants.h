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

// finger A
#define fA_IN1 53
#define fA_IN2 52
#define fA_EN 46
#define fA_chA 20
#define fA_chB 21
#define fA_hallEffectPin A0

inline finger fingerA(fA_IN1, fA_IN2, fA_EN, fA_chA, fA_chB, fA_hallEffectPin);

// ...


#endif
