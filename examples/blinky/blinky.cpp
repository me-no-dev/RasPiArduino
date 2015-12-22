#include <Arduino.h>

#define LEDPIN (17)

unsigned long startTime;
int value = HIGH;
void setup() {
  printf("Setup called!\n");
  startTime = millis();
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  if (millis() - startTime > 1000) {
    startTime = millis();
    digitalWrite(LEDPIN, value);
    printf("Value: %d\n", value);
    if (value == HIGH) {
      value = LOW;
    } else {
      value = HIGH;
    }
  }
}
