#include <Arduino.h>
#include "time.h"

#define STATE_END (-1)
int state = STATE_TIME1;

void setup() {
  printf("Setup called!\n");
}

void loop() {
  printf("loop called\n");
  switch (state) {
  case STATE_TIME1:
    testDelay();
    state = STATE_END;
  case STATE_END:
    exit(0);
  }
}
