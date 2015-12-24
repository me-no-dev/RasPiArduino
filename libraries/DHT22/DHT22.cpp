#include "DHT22.h"
#include <Arduino.h>
#include <pins_arduino.h>

#define DIRECT_READ(pin)		((GPLEV0 & (1 << pin)) ? 1 : 0)
#define DIRECT_MODE_INPUT(pin)	(GPFSEL(pin) &= ~(0x07 << GPFSELB(pin)))
#define DIRECT_MODE_OUTPUT(pin)	(GPFSEL(pin) |= (1 << GPFSELB(pin)))
#define DIRECT_WRITE_LOW(pin)	(GPCLR0 = (1 << pin))

// This should be 40, but the sensor is adding an extra bit at the start
#define DHT22_DATA_BIT_COUNT 41

DHT22::DHT22(uint8_t pin){
  _bitmask = pin;
  _lastReadTime = 0;
  _lastHumidity = DHT22_ERROR_VALUE;
  _lastTemperature = DHT22_ERROR_VALUE;
}

//
// Read the 40 bit data stream from the DHT 22
// Store the results in private member data to be read by public member functions
//
DHT22_ERROR_t DHT22::readData(){
  uint8_t bitmask = _bitmask;
  uint8_t retryCount;
  uint8_t bitTimes[DHT22_DATA_BIT_COUNT];
  int currentHumidity;
  int currentTemperature;
  uint8_t checkSum, csPart1, csPart2, csPart3, csPart4;
  unsigned long currentTime;
  int i;

  currentHumidity = 0;
  currentTemperature = 0;
  checkSum = 0;
  currentTime = millis();
  for(i = 0; i < DHT22_DATA_BIT_COUNT; i++){
    bitTimes[i] = 0;
  }

  if((currentTime - _lastReadTime) < 2000){
    return DHT_ERROR_TOOQUICK;
  }
  _lastReadTime = currentTime;

  // Pin needs to start HIGH, wait until it is HIGH with a timeout
  pinMode(bitmask, INPUT);//use this first to reset any previous PullUp/Downs
  retryCount = 0;
  while(!DIRECT_READ(bitmask)) {
    if (retryCount > 250){
      return DHT_BUS_HUNG;
    }
    retryCount++;
    delayMicroseconds(1);
  }
  DIRECT_WRITE_LOW(bitmask);
  DIRECT_MODE_OUTPUT(bitmask);
  delayMicroseconds(1000); // 1.1 ms
  DIRECT_MODE_INPUT(bitmask);
  // Find the start of the ACK Pulse
  retryCount = 0;
  while(!DIRECT_READ(bitmask)) {
    if (retryCount++ > 40){ //20-40us spec
      return DHT_ERROR_NOT_PRESENT;
    }
    delayMicroseconds(1);
  }
  // Find the end of the ACK Pulse
  retryCount = 0;
  while(DIRECT_READ(bitmask)) {
    if (retryCount++ > 80) { //80us spec
      return DHT_ERROR_ACK_TOO_LONG;
    }
    delayMicroseconds(1);
  }
  // Read the 40 bit data stream
  for(i = 0; i < DHT22_DATA_BIT_COUNT; i++){
    // Find the start of the sync pulse
    retryCount = 0;
    while(!DIRECT_READ(bitmask)) {
      if (retryCount++ > 100) {//(Spec is 50 us, 35*2 == 70 us)
        return DHT_ERROR_SYNC_TIMEOUT;
      }
      delayMicroseconds(1);
    }
    // Measure the width of the data pulse
    retryCount = 0;
    while(DIRECT_READ(bitmask)) {
      if (retryCount++ > 150) {//(Spec is 80 us, 50*2 == 100 us)
        return DHT_ERROR_DATA_TIMEOUT;
      }
      delayMicroseconds(1);
    }
    bitTimes[i] = retryCount;
  }

  // Now bitTimes have the number of retries (us)
  // that were needed to find the end of each data bit
  // Spec: 0 is 26 to 28 us
  // Spec: 1 is 70 us
  // bitTimes[x] <= 40 is a 0
  // bitTimes[x] >  40 is a 1
  // Note: the bits are offset by one from the data sheet, not sure why

  for(i = 0; i < 16; i++){
    if(bitTimes[i + 1] > 40) currentHumidity |= (1 << (15 - i));
  }
  for(i = 0; i < 16; i++){
    if(bitTimes[i + 17] > 40) currentTemperature |= (1 << (15 - i));
  }
  for(i = 0; i < 8; i++){
    if(bitTimes[i + 33] > 40) checkSum |= (1 << (7 - i));
  }

  _lastHumidity = currentHumidity & 0x7FFF;
  if(currentTemperature & 0x8000){
    _lastTemperature = -currentTemperature & 0x7FFF;
  } else {
    _lastTemperature = currentTemperature;
  }

  csPart1 = currentHumidity >> 8;
  csPart2 = currentHumidity & 0xFF;
  csPart3 = currentTemperature >> 8;
  csPart4 = currentTemperature & 0xFF;
  if(checkSum != ((csPart1 + csPart2 + csPart3 + csPart4) & 0xFF)) return DHT_ERROR_CHECKSUM;
  return DHT_ERROR_NONE;
}
