// GPT ISR test, run at 1mhz from 24mhz clock, test either GPT1 or GPT2
//  could do PWM with OCRn registers, but no GPT output pins on T4

volatile uint32_t ticks;
int lastInterruptUsed = 1;
int curInterruptUsed = 1;
int curTimer = 1;

int period = 1;
bool timer1occured = 0;

bool timer2occured = 0;

int state = 0;
bool outputState = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  delay(2000);
  gpt1_init(30000);  // us
  gpt2_init(70000);  // us
  pinMode(14, OUTPUT);
}

void loop() {

  switch (state) {
    case 0: // period one, on
    outputState = 1;

    if (timer1occured){
      state = 1;

      // about to start counting using timer 2, so reset the count value now
      GPT2_CNT = 0;
      timer1occured = 0;
    }
    break;

    case 1: // period one, off
    outputState =0;

    if (timer2occured){
      state = 2;
      // about to start counting using timer 2, so reset the count value now
      GPT2_CNT = 0;
      timer2occured = 0;
    }
    

    break;

    case 2: // period two, on

    outputState = 1;

    if (timer2occured){
      state = 3;

      // about to start counting using timer 2, so reset the count value now
      GPT1_CNT = 0;
      timer2occured = 0;
    }

    case 3: // period two, off

    outputState = 0;

    if (timer1occured){

      state = 0;

      // about to start counting using timer 1, so reset the count value now
      GPT1_CNT = 0;
      timer1occured = 0;

    }

  }
}



void gpt1_isr() {
  timer1occured = 1;
  GPT1_SR |= GPT_SR_OF3;  // clear all set bits
}

void gpt1_init(uint32_t us) {
  CCM_CCGR1 |= CCM_CCGR1_GPT(CCM_CCGR_ON);  // enable GPT1 module
  GPT1_CR = 0;
  GPT1_PR = 23;        // prescale+1
  GPT1_OCR1 = us - 1;  // compare
  GPT1_SR = 0x3F;      // clear all prior status
  GPT1_IR = GPT_IR_OF1IE;
  GPT1_CR = GPT_CR_EN | GPT_CR_CLKSRC(1);  // 1 ipg 24mhz  4 32khz
  attachInterruptVector(IRQ_GPT1, gpt1_isr);
  NVIC_ENABLE_IRQ(IRQ_GPT1);
}

void gpt2_isr() {
  GPT2_SR |= GPT_SR_OF3;  // clear all set bits
  timer2occured = 1;
}

void gpt2_init(uint32_t us) {
  CCM_CCGR0 |= CCM_CCGR0_GPT2_BUS(CCM_CCGR_ON);  // enable GPT2 module
  GPT2_CR = 0;
  GPT2_PR = 23;        // prescale+1
  GPT2_OCR1 = us - 1;  // compare
  GPT2_SR = 0x3F;      // clear all prior status
  GPT2_IR = GPT_IR_OF1IE;
  GPT2_CR = GPT_CR_EN | GPT_CR_CLKSRC(1);  // 1 ipg 24mhz  4 32khz
  attachInterruptVector(IRQ_GPT2, gpt2_isr);
  NVIC_ENABLE_IRQ(IRQ_GPT2);
}