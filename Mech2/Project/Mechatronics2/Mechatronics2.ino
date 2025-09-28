int state = 0;
int curPos = 0;
int desPos = 0;
int buttonState = 0;
int lastButtonState = 0;
bool buttonPressed;
int dt = 200;
int closedSteps = 500; // 1 in == 300 steps

int yellow = 0b1;
int blue = 0b1000;
int red = 0b10;
int green = 0b100;

//int fullSteps[4] = {yellow, blue, red, green}; // tl black, tr green, bl blue, br red: yellow, blue, red, green
int fullSteps[4] = { blue, yellow, red, green}; // tl black, tr green, bl blue, br red: yellow, blue, red, green

void setup() {
  // put your setup code here, to run once:
  // motor
  DDRA = 0xff;

  //button
  DDRB = 0;
  PORTB = 0xff;
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = PINB & 0b1;

  switch (state) {
    case 0:  // not moving and open
      if (buttonPressed) {
        state = 1;
        buttonPressed = 0;
      }
      break;

    case 1:  // closing
      desPos = closedSteps;
      if (curPos == desPos) {
        state = 2;
      }
      break;

    case 2:  // closed
      if (buttonPressed) {
        state = 3;
        buttonPressed = 0;
      }
      break;
      
    case 3:  // opening
      desPos = 0;
      if (curPos == desPos) {
        state = 0;
      }
      break;
  }

  if (buttonState == 0 && lastButtonState == 1) {
    delay(dt);
    buttonPressed = !buttonPressed;
  } else if (buttonState == 1 && lastButtonState == 0)
    delay(dt);

  lastButtonState = buttonState;

  if (curPos < desPos) {
    curPos++;
  } else if (curPos > desPos) {
    curPos--;
  }

  PORTA = fullSteps[curPos % 4];
  delay(8);
}
