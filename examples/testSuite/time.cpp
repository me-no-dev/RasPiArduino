#include <Arduino.h>
#include "time.h"

/**
 * Test that a delay of some time is actually that amount of time.
 */
void testDelay() {
  unsigned long start =  millis();
  delay(1000);
  unsigned long end = millis();
  printf("[Time1] We waited for %d milliseconds after delaying for 1000 milliseconds\n", end-start);
}
