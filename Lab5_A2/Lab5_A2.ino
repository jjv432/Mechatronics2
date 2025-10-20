int lastState1 = 0;
int lastState2 = 0;
int curState1 = 0;
int curState2 = 0;
int count1 = 0;
int count2 = 0;
int curReading1 = 0;
int curReading2 = 0;
int state = 0;
int curPos = 0;
float curAngle = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
}

void loop() {
  // put your main code here, to run repeatedly:
  curReading1 = analogRead(A0);
  curReading2 = analogRead(A1);

  if (curReading1 > 400) {
    curState1 = 1;
  } else if (curReading1 < 350) {
    curState1 = 0;
  }

  if (curReading2 > 400) {
    curState2 = 1;
  } else if (curReading2 < 350) {
    curState2 = 0;
  }

  if (curState1 && !lastState1) {
    count1++;
  }

  if (curState2 && !lastState2) {
    count2++;
  }

  // Serial.print(count1);
  // Serial.print("\t");
  // Serial.println(count2);
  // Serial.println(curReading1);
  lastState1 = curState1;
  lastState2 = curState2;

  switch (state) {
    case 0:  // a and b on
      if (!curState2) {
        curPos--;
        state = 1;
      } else if (!curState1) {
        curPos++;
        state = 2;
      }
      break;

    case 1:  // a on only
      if (!curState1) {
        curPos--;
        state = 3;
      } else if (curState2) {
        curPos++;
        state = 0;
      }

      break;

    case 2:  // b on only
      if (curState1) {
        curPos--;
        state = 0;
      } else if (!curState2) {
        curPos++;
        state = 3;
      }

      break;

    case 3:  // neither on
      if (curState1) {
        curPos++;
        state = 1;
      } else if (curState2) {
        curPos--;
        state = 2;
      }
      break;
  }
  curAngle = 7.5*curPos;
  Serial.println(curAngle);
}
