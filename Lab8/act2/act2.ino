int switch1val, switch2val;
int switch1oldval = 0;
int switch2oldval = 0;
volatile uint32_t ticks;
unsigned long switch1time, switch2time;
int dt = 10;


void setup() {
  // put your setup code here, to run once:
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);

  Serial.begin(9600);
  while (!Serial);
  delay(2000);
  gpt1_init(100);   // us

}

void loop() {
  // put your main code here, to run repeatedly:
  switch1val = digitalRead(0);
  switch2val = digitalRead(1);

  if (!switch1val && switch1oldval){ // falling edge on switch 1
    delay(dt);
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    switch1time = ticks;
    Serial.println(float((switch1time - switch2time) * .1));
  }
  else if (!switch1oldval && switch1val){
    delay(dt);
  }

  if (!switch2val && switch2oldval){ // falling edge on switch 1
    delay(dt);
    digitalWrite(3, HIGH);
    digitalWrite(2, LOW);
    switch2time = ticks;
    Serial.println(float((switch2time - switch1time) * .1));
  }
  else if (!switch2oldval && switch2val){
    delay(dt);
  }


  switch1oldval = switch1val;
  switch2oldval = switch2val;
  
}

void gpt1_isr() {
  GPT1_SR |= GPT_SR_OF3;  // clear all set bits
  ticks++;
  while (GPT1_SR & GPT_SR_OF1); // wait for clear
}

void gpt1_init(uint32_t us) {
  CCM_CCGR1 |= CCM_CCGR1_GPT(CCM_CCGR_ON) ;  // enable GPT1 module
  GPT1_CR = 0;
  GPT1_PR = 23;   // prescale+1
  GPT1_OCR1 = us - 1;  // compare
  GPT1_SR = 0x3F; // clear all prior status
  GPT1_IR = GPT_IR_OF1IE;
  GPT1_CR = GPT_CR_EN | GPT_CR_CLKSRC(1) ;// 1 ipg 24mhz  4 32khz
  attachInterruptVector(IRQ_GPT1, gpt1_isr);
  NVIC_ENABLE_IRQ(IRQ_GPT1);
}
