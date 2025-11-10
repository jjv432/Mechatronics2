# include "Arduino.h"
# include "myDCEncoder.h"

// This is a weird quirk of c++ and attachInterrupt
// Global static lookup for all encoder pins
myDCEncoder* encoder_lookup[NUM_DIGITAL_PINS] = {nullptr};

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
	static unsigned int last_p_error = 0;
	static unsigned int last_time = millis();
	static unsigned int i_error = 0; // integral of error

	// get timing info
	int now = millis();
	int dt = now - last_time;

	// find errors
	unsigned int p_error = _curPos - desPos;				// P
	float d_error = (p_error - last_p_error) / dt;			// D
	i_error += p_error * dt;								// I

	// reset for next time
	last_p_error = p_error;
	last_time = now;

	// Calculate commanded PWM (speed)
	float PwmCommand = _kp * p_error + _kd * d_error + _ki * i_error;

	myDCEncoder::driveMotor(PwmCommand);

}

void myDCEncoder::ticksToDeg(){

}

//* INCLUDE CONVERSION TO DEGREES IN HERE!
// Function called by the ISRs to update the current position of the motor
void myDCEncoder::updateCurState(){
	// an interrupt in the main script will call this. Based on the last channel that was interrupted, you'll know which direction you spun
	static int lastState = 0;
	int curState = 0;
	static int lastTime = millis();
	
	int curTime = millis();
	int dt = millis() - lastTime;
	lastTime = curTime;

	bool a = digitalRead(_input_ch[0]);
	bool b = digitalRead(_input_ch[1]);
	
	if (!a&&!b){
		curState = 0;
	}
	else if(a&&!b){
		curState = 1;
	}
	else if(a&&b){
		curState = 2;
	}
	else if(!a&&b){
		curState = 3;
	}
	
	// cw is 0, 1, 2, 3
	switch (curState){
		case 0:
		if (lastState == 3){
			_curPos ++;
			_curOmega = 1/dt;
		}
		else if (lastState == 1){
			_curPos --;
			_curOmega = -1/dt;
		}
		else if (lastState == 0){
			_curOmega = 0;
		}
		break;

		case 1:
		if (lastState == 0){
			_curPos ++;
			_curOmega = 1/dt;
		}
		else if (lastState == 2){
			_curPos --;
			_curOmega = -1/dt;
		}
		else if (lastState == 1){
			_curOmega = 0;
		}
		break;

		case 2:
		if (lastState == 1){
			_curPos ++;
			_curOmega = 1/dt;
		}
		else if (lastState == 3){
			_curPos --;
			_curOmega = -1/dt;
		}
		else if (lastState == 2){
			_curOmega = 0;
		}
		break;

		case 3:
		if (lastState == 2){
			_curPos ++;
			_curOmega = 1/dt;
		}
		else if (lastState == 0){
			_curPos --;
			_curOmega = -1/dt;
		}
		else if (lastState == 3){
			_curOmega = 0;
		}
		break;
	}

	lastState = curState;
}
