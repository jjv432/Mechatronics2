int yellow = 0b1;
int blue = 0b1000;
int red = 0b10;
int green = 0b100;

//int fullSteps[4] = {yellow, blue, red, green}; // tl black, tr green, bl blue, br red: yellow, blue, red, green
int fullSteps[4] = { blue, yellow, red, green}; // tl black, tr green, bl blue, br red: yellow, blue, red, green
int curPos = 10000;

void setup() {
  // put your setup code here, to run once:
  // motor
  DDRA = 0xff;
}

void loop() {
  // put your main code here, to run repeatedly:
  curPos--;

  PORTA = fullSteps[curPos % 4];
  delay(6);
}
