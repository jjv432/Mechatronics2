// GPT ISR test, run at 1mhz from 24mhz clock, test either GPT1 or GPT2
//  could do PWM with OCRn registers, but no GPT output pins on T4

volatile uint32_t ticks;
int lastInterruptUsed = 1;
int curInterruptUsed = 1;
int curTimer = 1;

int tickT1 = 0;
int tickT2 = 0;


void setup() {
  Serial.begin(9600);
  while (!Serial);
  delay(2000);
  gpt1_init(30000);   // us
  gpt2_init(70000);   // us
  pinMode(14, OUTPUT);
}

void loop() {

  if (tickT1 == 2){
    curTimer = 2;
  }
  else if(tickT2 == 2){
    curTimer = 1;
  }

  // turn on signal
  // care ab timer 1
  // if tmr 1 off, turn light off

  // care ab timer 2
  // when tmr 2 ends, care ab timer 2 again
  // turn on the led
  // when tmr 2 ends, turn off led
  // care ab timer 1
  // when timer 1 is over, turn light on 
}



void gpt1_isr() {
  GPT1_SR |= GPT_SR_OF3;  // clear all set bits

  tickT1 ++;

  if (tickT1 > 2)
    tickT1 = 0;

  lastInterruptUsed = curInterruptUsed;
  curInterruptUsed = 1;

  if (lastInterruptUsed == curInterruptUsed)
    digitalWrite(14, HIGH);
  else
    digitalWrite(14, LOW);
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

void gpt2_isr() {
  GPT2_SR |= GPT_SR_OF3;  // clear all set bits
  lastInterruptUsed = curInterruptUsed;
  curInterruptUsed = 2;

  tickT2 ++;
  if (tickT2 > 2)
    tickT2 = 0;


  if (lastInterruptUsed == curInterruptUsed)
    digitalWrite(14, HIGH);
  else
    digitalWrite(14, LOW);

  while (GPT2_SR & GPT_SR_OF1); // wait for clear
  asm volatile ("dsb");
}

void gpt2_init(uint32_t us) {
  CCM_CCGR0 |= CCM_CCGR0_GPT2_BUS(CCM_CCGR_ON) ;  // enable GPT2 module
  GPT2_CR = 0;
  GPT2_PR = 23;   // prescale+1
  GPT2_OCR1 = us - 1;  // compare
  GPT2_SR = 0x3F; // clear all prior status
  GPT2_IR = GPT_IR_OF1IE;
  GPT2_CR = GPT_CR_EN | GPT_CR_CLKSRC(1) ;// 1 ipg 24mhz  4 32khz
  attachInterruptVector(IRQ_GPT2, gpt2_isr);
  NVIC_ENABLE_IRQ(IRQ_GPT2);
}