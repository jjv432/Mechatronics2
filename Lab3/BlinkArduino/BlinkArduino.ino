int frequency = 2;
int ledPinLocation = 1;  // PA1
unsigned long long startTime = 0;
int servoSlopTimeMS = 500;

void setup() {
  // put your setup code here, to run once:
  DDRA = 0xff;  // output for led
  DDRB = 0xff;  // for the pwm
  Serial.begin(9600);
  PORTB = 1;
}

void loop() {
  // put your main code here, to run repeatedly:

  PORTA = myFrequencyGenerator(frequency, ledPinLocation);

  if (millis() - startTime < servoSlopTimeMS) {
    myPWM(0);
  }
  else if (millis() - startTime < 2*servoSlopTimeMS) {
    myPWM(180);
  }
  else
    startTime = millis();
}

void myPWM(float angle) {

  const float period = 1000. * 1. / 50.;  //total period for 50 hz
  float onDuration = 575 + (angle / .1); // hand tuned values

  if (micros() % 20000 < onDuration)
    PORTB = 1;
  else
    PORTB = 0;
}

int myFrequencyGenerator(float freq, int pinLocation) {
  const float period = 1000 * 1 / freq;  // get the period in ms
  static bool state = 0;
  static unsigned long int last_time = 0;
  int reg;

  if (millis() - last_time >= period) {
    state = !state;
    last_time = millis();
  }

  reg = state << pinLocation;

  return reg;
}