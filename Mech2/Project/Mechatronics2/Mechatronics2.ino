int state = 0;
int curPos = 0;
int desPos = 0;
int buttonState = 0;
int lastButtonState = 0;
bool buttonPressed;
int dt = 200;
int closedSteps = 1000; // 1 in == 300 steps
int limsw;
int yellow = 0b1;
int blue = 0b1000;
int red = 0b10;
int green = 0b100;
const int trig_pin = 9;
const int echo_pin = 8;
float timing = 0.0;
float distance = 0.0;
//int fullSteps[4] = {yellow, blue, red, green}; // tl black, tr green, bl blue, br red: yellow, blue, red, green
int fullSteps[4] = { blue, yellow, red, green}; // tl black, tr green, bl blue, br red: yellow, blue, red, green

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // motor
  DDRA = 0xff;
  //button
  DDRB = 0;
  PORTB = 0xff;
  // Base Limit Switch:
  DDRC = 0;
  PORTC = 0xff;
  // add ultrasonic sensor here
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);
  digitalWrite(trig_pin, LOW);
}
void loop() {
// BITMASK
  limsw = (PINC & 1);
// Pulse Sensor
  sendTrigger();
  timing = pulseIn(echo_pin, HIGH);
  distance = (timing * 0.0343) / 2.0;
  buttonState = PINB & 0b1;
  // Detect Button
  if (buttonState == 0 && lastButtonState == 1) {
    delay(dt);
    buttonPressed = 1;
  } else {
    buttonPressed = 0;
  }
  lastButtonState = buttonState;
  // CYCLE OFF BUTTON PRESSES
  if (buttonPressed) {
    state++;
    if (state > 3) state = 1;
  }
  if (limsw == 1) { 
    //Actuate off distance
    if (distance <= 75 && distance >= 1) {
      switch (state) {
        case 1:
          desPos = 300;   // FINE TUNE THIS: VALUE IS LARGE
          break;
        case 2:
          desPos = 600;   // FINE TUNE THIS: VALUE IS MEDIUM
          break;
        case 3:
          desPos = 1000;  // FINE TUNE THIS: VALUE IS SMALL
          break;
      }
    } else {
      desPos = 0; 
    }
  } 
  else if (limsw == 0) { 
    // limit switch pressed → back off until released
    if (curPos > 0) {
      curPos--;  //1 step at a time
      PORTA = fullSteps[curPos % 4];
      delay(4);
    } else {
      // ONLY OPEN TILL OF LIMIT
      desPos = 0;
    }
    // STOP UNLESS RELEASED
    return;  
  }
  // MOVE TO DESPOS
  if (curPos < desPos) {
    curPos++;
  } else if (curPos > desPos) {
    curPos--;
  }

  PORTA = fullSteps[curPos % 4];
  delay(4);
// Serial Print Data
  Serial.println("Data:");
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm \n");
  Serial.println(limsw);
}
void sendTrigger() {
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
