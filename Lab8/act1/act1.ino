int switch1, switch2;
void setup() {
  // put your setup code here, to run once:
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:
  switch1 = digitalRead(0);
  switch2 = digitalRead(1);

  if (!switch1 && switch2){
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
  }
    
  else if (!switch2 && switch1){
    digitalWrite(3, HIGH);
    digitalWrite(2, LOW);
  }

  else{
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
  }
    

  


}
