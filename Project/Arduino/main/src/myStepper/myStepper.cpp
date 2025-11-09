# include "Arduino.h"
# include "myStepper.h"

myStepper::myStepper(int pin_a, int pin_b,int pin_c, int pin_d)
{
	_pin_array[0] = pin_a;
	_pin_array[1] = pin_b;
	_pin_array[2] = pin_c;
	_pin_array[3] = pin_d;
	_curPos = 0;
}

void myStepper::init() {
	for (int i = 0; i <=4; i++){
		pinMode(_pin_array[i], OUTPUT);
	}
}

void myStepper::goToDesPos(int desPos){
	digitalWrite(_pin_array[0], LOW);
	digitalWrite(_pin_array[1], LOW);
	digitalWrite(_pin_array[2], LOW);
	digitalWrite(_pin_array[3], LOW);
	

	if (_curPos < desPos){
		_curPos ++;	
	}
	else if (_curPos > desPos){
		_curPos --;
	}
	digitalWrite(_pin_array[_curPos%4], HIGH);
	delay(5);
}
