#include "DHT22.h"
#include <Arduino.h>
#include <pins_arduino.h>

#define DIRECT_READ(pin)		((GPLEV0 & (1 << pin)) > 0)
#define DIRECT_MODE_INPUT(pin)	(GPFSEL(pin) &= ~(0x07 << GPFSELB(pin)))
#define DIRECT_MODE_OUTPUT(pin)	(GPFSEL(pin) |= (1 << GPFSELB(pin)))
#define DIRECT_WRITE_LOW(pin)	(GPCLR0 = (1 << pin))

// This should be 40, but the sensor is adding an extra bit at the start
#define DHT22_DATA_BIT_COUNT 40

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
  uint8_t bitTimes[DHT22_DATA_BIT_COUNT];
  int currentHumidity;
  int currentTemperature;
  uint8_t checkSum, csPart1, csPart2, csPart3, csPart4;
  unsigned long currentTime;
  int i;

  uint32_t startWait;
#define WAITTO(level, error, timeout) do { startWait = STCLO; do { if((STCLO - startWait) >= timeout){ return error; } } while(DIRECT_READ(bitmask) == level); } while(0)

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
  //__/    Reset    /_SYN __/0 _/1 __/....
  //  |_____________| |__|  |_| |_|  |_.....
  //pinMode(27, OUTPUT); // << DEBUG
  //GPCLR0 = _BV(27); // << DEBUG
  pinMode(bitmask, INPUT);//use this first to reset any previous PullUp/Downs
  WAITTO(LOW, DHT_BUS_HUNG, 250);
  DIRECT_MODE_OUTPUT(bitmask);
  //GPSET0 = _BV(27); // << DEBUG
  DIRECT_WRITE_LOW(bitmask);
  usleep(1200); // 1.1 ms
  DIRECT_MODE_INPUT(bitmask);
  //GPCLR0 = _BV(27); // << DEBUG
  // Find the start of the ACK Pulse
  WAITTO(LOW, DHT_ERROR_NOT_PRESENT, 40);  //20-40 spec
  //GPSET0 = _BV(27); // << DEBUG
  // Find the end of the ACK Pulse
  WAITTO(HIGH, DHT_ERROR_ACK_TOO_LONG1, 60);//20-40 spec
  //GPCLR0 = _BV(27); // << DEBUG
  // Find the start of the Sync Pulse
  WAITTO(LOW, DHT_ERROR_ACK_TOO_LONG2, 100);//80 spec
  //GPSET0 = _BV(27); // << DEBUG
  // Find the end of the Sync Pulse
  WAITTO(HIGH, DHT_ERROR_ACK_TOO_LONG3, 100);//80 spec
  //GPCLR0 = _BV(27); // << DEBUG
  // Read the 40 bit data stream
  for(i = 0; i < DHT22_DATA_BIT_COUNT; i++){
    // Find the start of the data pulse
    WAITTO(LOW, DHT_ERROR_SYNC_TIMEOUT, 150);//50 spec
    //GPSET0 = _BV(27); // << DEBUG
    // Measure the width of the data pulse
    WAITTO(HIGH, DHT_ERROR_DATA_TIMEOUT, 150);//30-70 spec
    bitTimes[i] = micros() - startWait;
    //GPCLR0 = _BV(27); // << DEBUG
  }
  // Now bitTimes have the number of retries (us)
  // that were needed to find the end of each data bit
  // Spec: 0 is 26 to 28 us
  // Spec: 1 is 70 us
  // bitTimes[x] <= 40 is a 0
  // bitTimes[x] >  40 is a 1

  for(i = 0; i < 16; i++){
    if(bitTimes[i + 0] > 40) currentHumidity |= (1 << (15 - i));
  }
  for(i = 0; i < 16; i++){
    if(bitTimes[i + 16] > 40) currentTemperature |= (1 << (15 - i));
  }
  for(i = 0; i < 8; i++){
    if(bitTimes[i + 32] > 40) checkSum |= (1 << (7 - i));
  }

  csPart1 = currentHumidity >> 8;
  csPart2 = currentHumidity & 0xFF;
  csPart3 = currentTemperature >> 8;
  csPart4 = currentTemperature & 0xFF;
  if(checkSum != ((csPart1 + csPart2 + csPart3 + csPart4) & 0xFF)) return DHT_ERROR_CHECKSUM;

  _lastHumidity = currentHumidity & 0x7FFF;
  if(currentTemperature & 0x8000){
    _lastTemperature = -currentTemperature & 0x7FFF;
  } else {
    _lastTemperature = currentTemperature;
  }
  return DHT_ERROR_NONE;
}
