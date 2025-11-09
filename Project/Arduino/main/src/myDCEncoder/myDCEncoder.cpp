# include "Arduino.h"
# include "myDCEncoder.h"

myDCEncoder::myDCEncoder(int direction_pin, int ch_a, int ch_b, int ch_c, int ch_d)
{
	_direction_pin = direction_pin;

	_input_ch[0] = ch_a;
	_input_ch[1] = ch_b;
	_input_ch[2] = ch_c;
	_input_ch[3] = ch_d;

}

void myDCEncoder::init() {
	for (int i = 0; i <=4; i++){
		pinMode(_input_ch[i], INPUT_PULLUP);
	}
}

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
