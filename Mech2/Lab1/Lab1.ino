#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

int potval;
int Switch2 = 0;
int Switch3 = 0;
int Switch4 = 0;
int Switch5 = 0;

void setup() {
  // put your setup code here, to run once:
  DDRA = 0;
  PORTA = 0xff;

  DDRB = 0xFF;
  PORTB = 0;
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Switch2 = !(PINA & 0b1);
  Switch3 = !((PINA & 0b10) >> 1);
  Switch4 = !((PINA & 0b100) >> 2);
  Switch5 = !((PINA & 0b1000) >> 3);
  Serial.println(PINA & 0b10);
  potval = analogRead(A0);

  if (Switch2) {
    lcd.clear();
    PORTB = 1;
  }

  else if (!Switch2)
    PORTB = 0;

  if (Switch3) {
    lcd.clear();
    PORTB = 1;
    delay(500);
    PORTB = 0;
  }

  if (Switch4) {
    lcd.clear();
    lcd.print("Jack Vranicar");
  }

  if (Switch5) {
    lcd.clear();
    lcd.print(potval);
  }
}
