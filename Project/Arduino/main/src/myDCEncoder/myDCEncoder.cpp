# include "Arduino.h"
# include "myDCEncoder.h"

myDCEncoder::myDCEncoder(int IN_1, int IN_2, int EN, int ch_a, int ch_b, int ch_c, int ch_d)
{
	_IN_1 = IN_1;
	_IN_2 = IN_2;
	_EN = EN;

	_input_ch[0] = ch_a;
	_input_ch[1] = ch_b;
	_input_ch[2] = ch_c;
	_input_ch[3] = ch_d;

}

void myDCEncoder::init() {
	// Make IN1 and IN2 output pins
	pinMode(_IN_1, OUTPUT);
	pinMode(_IN_2, OUTPUT);

	// Make all the encoder pins inputs
	for (int i = 0; i <4; i++){
		pinMode(_input_ch[i], INPUT_PULLUP);
	}

	// attach the encoder pins to corresponding interrupts
	attachInterrupt(digitalPinToInterrupt(_input_ch[0]), myDCEncoder::ISR_0, RISING);
	attachInterrupt(digitalPinToInterrupt(_input_ch[1]), myDCEncoder::ISR_1, RISING);
	attachInterrupt(digitalPinToInterrupt(_input_ch[2]), myDCEncoder::ISR_2, RISING);
	attachInterrupt(digitalPinToInterrupt(_input_ch[3]), myDCEncoder::ISR_3, RISING);
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
	now = millis();
	int dt = now - last_time;

	// find errors
	unsigned int p_error = _curPos - desPos;						// P
	unsigned float d_error = (p_error - last_p_error) / dt;			// D
	i_error += p_error * dt;										// I

	// reset for next time
	last_p_error = p_error;
	last_time = now;

	// Calculate commanded PWM (speed)
	float PwmCommand = _kp * p_error + _kd * d_error + _ki * i_error;

	myDCEncoder::driveMotor(PwmCommand);

}


// ISRs for updating the current position
void ISR_0(){
	myDCEncoder::updateCurState(0);
}
void ISR_1(){
	myDCEncoder::updateCurState(1);
}
void ISR_2(){
	myDCEncoder::updateCurState(2);
}
void ISR_3(){
	myDCEncoder::updateCurState(3);
}

// Function called by the ISRs to update the current position of the motor
void myDCEncoder::updateCurState(int channelNumber){
	// an interrupt in the main script will call this. Based on the last channel that was interrupted, you'll know which direction you spun
	static int lastChannelNumber = 0;
	static int lastTime = millis();
	const float resolution = .5; // how many degrees per phase of the encoder
	
	curTime = millis();
	dt = millis() - lastTime;
	lastTime = curTime;

	// this is arbitrary, but imagine it goes:  0, 1, 2, 3 clockwise
	switch (channelNumber){
		case 0:
		if (lastChannelNumber == 3){
			_curPos ++;
			_curOmega = resolution/dt;
		}
		else if (lastChannelNumber == 1){
			_curPos --;
			_curOmega = -resolution/dt;
		}
		else if (lastChannelNumber == 0){
			_curOmega = 0;
		}
		break;

		case 1:
		if (lastChannelNumber == 0){
			_curPos ++;
			_curOmega = resolution/dt;
		}
		else if (lastChannelNumber == 2){
			_curPos --;
			_curOmega = -resolution/dt;
		}
		else if (lastChannelNumber == 1){
			_curOmega = 0;
		}
		break;

		case 2:
		if (lastChannelNumber == 1){
			_curPos ++;
			_curOmega = resolution/dt;
		}
		else if (lastChannelNumber == 3){
			_curPos --;
			_curOmega = -resolution/dt;
		}
		else if (lastChannelNumber == 2){
			_curOmega = 0;
		}
		break;

		case 3:
		if (lastChannelNumber == 2){
			_curPos ++;
			_curOmega = resolution/dt;
		}
		else if (lastChannelNumber == 0){
			_curPos --;
			_curOmega = -resolution/dt;
		}
		else if (lastChannelNumber == 3){
			_curOmega = 0;
		}
		break;
	}

}
