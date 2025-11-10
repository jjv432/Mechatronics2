#include "src/myStepper/myStepper.h"  // doing it like this so git will keep up
#include "src/myDCEncoder/myDCEncoder.h"  // doing it like this so git will keep up

myStepper mS(1, 3, 2, 4);
int desPos = 1000;

void setup() {

  mS.init();
}

void loop() {
  // Simple test: move back and forth between two positions
  desPos = 200;
   while (mS._curPos != desPos) {
    mS.goToDesPos(desPos);  // Move toward position 10
  }

  desPos = 0;

  while (mS._curPos != desPos) {
    mS.goToDesPos(desPos);  // Move toward position 10
  }


}