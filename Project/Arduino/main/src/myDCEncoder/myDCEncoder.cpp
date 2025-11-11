# include "Arduino.h"
# include "myDCEncoder.h"

myDCEncoder::myDCEncoder(int IN_1, int IN_2, int EN, int ch_a, int ch_b)
{
	_IN_1 = IN_1;
	_IN_2 = IN_2;
	_EN = EN;

	_input_ch[0] = ch_a;
	_input_ch[1] = ch_b;

}

void myDCEncoder::init() {
	// Make IN1 and IN2 output pins
	pinMode(_IN_1, OUTPUT);
	pinMode(_IN_2, OUTPUT);

	// Make all the encoder pins inputs
	for (int i = 0; i < 2; i++) {
    pinMode(_input_ch[i], INPUT_PULLUP);
	}
}

void myDCEncoder::driveMotor(int PWM){

	// Deal with switching directions
	if (PWM > 0){
		digitalWrite(_IN_1, HIGH);
		digitalWrite(_IN_2, LOW);
	}
	else if (PWM <= 0){
		digitalWrite(_IN_1, LOW);
		digitalWrite(_IN_2, HIGH);
	}

	// Set the speed
	analogWrite(_EN, abs(PWM));
}
void myDCEncoder::PID(int desPos){
	// desPos will be determined as a function of compression when using the hall effect. It's useful to have a baseline PID function, though
	// REMEMBER CURPOS IS IN "TICKS"
	static  int last_p_error = 0;
	static unsigned int last_time = millis();
	static float i_error = 0; // integral of error
	static int stationaryCounter = 0;
	static int lastStationaryPosition = 0;

	// get timing info
	unsigned int now = millis();
	unsigned int dt = now - last_time;
	last_time = now;

	// find errors
	int p_error = desPos - _curPos;							// P
	float d_error = (last_p_error - p_error) / dt;			// D
	i_error += -p_error * dt / 1000.;		 				// I

	// reset for next time
	last_p_error = p_error;	

	// Calculate commanded PWM (speed)
	float PwmCommand = _kp * p_error + _kd * d_error + _ki * i_error;

	if (PwmCommand > 255){
		PwmCommand = 255;
	}
	else if (PwmCommand < -255){
		PwmCommand = -255;
	}
	
	// increment if you've been stationary
	if (abs(p_error) <= 1){
		stationaryCounter ++;
	}
	else{
		stationaryCounter = 0;
	}

	// flush integral error if you've been stationary for a while
	if (stationaryCounter >= 15){
		i_error = 0;
		lastStationaryPosition = desPos;
		// in case desPos changes, reset all of this logic
		if (desPos != lastStationaryPosition){
			stationaryCounter = 0;
		}
	}

	myDCEncoder::driveMotor(PwmCommand);

}

void myDCEncoder::ticksToDeg(){

}


// Function called by the ISRs to update the current position of the motor
void myDCEncoder::updateCurState(){
	// an interrupt in the main script will call this. Based on the last channel that was interrupted, you'll know which direction you spun
	
	// read both channels of the encoder
	bool a = digitalRead(_input_ch[0]);
	bool b = digitalRead(_input_ch[1]);

	// update state info
	_lastState = _curState;

	// logic based on behavior of the enoder. See spec sheet 
	if (!a&&!b){
		_curState = 0;
	}
	else if(a&&!b){
		_curState = 1;
	}
	else if(a&&b){
		_curState = 2;
	}
	else if(!a&&b){
		_curState = 3;
	}
	
	myDCEncoder::updateCurPos();
}

// splitting this from the updateCurState is useful bc:
// at a minimum, can help increase readability
// if the interrupt is found to be taking too long (hasn't yet), a flag can be set in updateCurState which will be handled by
// updateCurPos. If the flag is true, run updateCurPos and set the flag to false
void myDCEncoder::updateCurPos(){

	// organize time data
	static unsigned int lastTime = millis();
	unsigned int curTime = millis();
	unsigned int dt = curTime - lastTime;
	lastTime = curTime;

	// cw is 0, 1, 2, 3
	switch (_curState){
		case 0:
		if (_lastState == 3){
			_curPos ++;
			_curOmega = 1/dt;
		}
		else if (_lastState == 1){
			_curPos --;
			_curOmega = -1/dt;
		}
		else if (_lastState == 0){
			_curOmega = 0;
		}
		break;

		case 1:
		if (_lastState == 0){
			_curPos ++;
			_curOmega = 1/dt;
		}
		else if (_lastState == 2){
			_curPos --;
			_curOmega = -1/dt;
		}
		else if (_lastState == 1){
			_curOmega = 0;
		}
		break;

		case 2:
		if (_lastState == 1){
			_curPos ++;
			_curOmega = 1/dt;
		}
		else if (_lastState == 3){
			_curPos --;
			_curOmega = -1/dt;
		}
		else if (_lastState == 2){
			_curOmega = 0;
		}
		break;

		case 3:
		if (_lastState == 2){
			_curPos ++;
			_curOmega = 1/dt;
		}
		else if (_lastState == 0){
			_curPos --;
			_curOmega = -1/dt;
		}
		else if (_lastState == 3){
			_curOmega = 0;
		}
		break;
	}

}