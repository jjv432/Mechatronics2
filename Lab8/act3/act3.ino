volatile bool timer1occured = 0;
volatile bool timer2occured = 0;
int t1 = 500;
int t2 = 2000; // seems to be the only one in control rn

int state = 0;
bool outputState = 0;

void setup() {
  // Serial.begin(9600);
  // while (!Serial);
  delay(200);
  gpt1_init(t1);  // us
  gpt2_init(t2);  // us
  pinMode(14, OUTPUT);
}

void loop() {

  digitalWrite(14, outputState);

  switch (state) {
    case 0:  // period one, on
      outputState = 1;

      if (timer1occured) {
        state = 1;
        reset_cnts();
      }
      break;

    case 1:  // period one, off
      outputState = 0;

      if (timer2occured) {
        state = 2;
        reset_cnts();
      }
      break;

    case 2:  // period two, on
      outputState = 1;

      if (timer2occured) {
        state = 3;
        reset_cnts();
      }
      break;

    case 3:  // period two, off
      outputState = 0;

      if (timer1occured) {
        state = 0;
        reset_cnts();
      }
      break;
  }
}

void reset_cnts(){
  GPT1_OCR1 = t1; // writing to OCR1 forces cnt to 0
  GPT2_OCR1 = t2;
  timer1occured = 0;
  timer2occured = 0;
}


void gpt1_isr() {
  GPT1_SR |= GPT_SR_OF3;  // clear all set bits
  timer1occured = 1;
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