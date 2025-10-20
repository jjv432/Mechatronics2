void setup() {
  // put your setup code here, to run once:
  DDRA = 0xff;
}

void loop() {
  // put your main code here, to run repeatedly:
  blinkLed(650);
}

void blinkLed(float frequency) {
  static bool state = 0;
  static float period = 1000 * 1000. / frequency;
  static unsigned long int last_time = 0;

  if ((micros() - last_time) >= period) {
    last_time = micros();
    state = !state;
  }

  PORTA = state;
}