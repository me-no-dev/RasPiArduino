#ifndef _DHT22_H_
#define _DHT22_H_

#include <inttypes.h>

#define DHT22_ERROR_VALUE -995

typedef enum{
  DHT_ERROR_NONE = 0,
  DHT_BUS_HUNG,
  DHT_ERROR_NOT_PRESENT,
  DHT_ERROR_ACK_TOO_LONG1,
  DHT_ERROR_ACK_TOO_LONG2,
  DHT_ERROR_ACK_TOO_LONG3,
  DHT_ERROR_SYNC_TIMEOUT,
  DHT_ERROR_DATA_TIMEOUT,
  DHT_ERROR_CHECKSUM,
  DHT_ERROR_TOOQUICK
} DHT22_ERROR_t;

class DHT22{
  private:
    uint8_t _bitmask;
    unsigned long _lastReadTime;
    short int _lastHumidity;
    short int _lastTemperature;

  public:
    DHT22(uint8_t pin);
    DHT22_ERROR_t readData();
    short int getHumidityInt();
    short int getTemperatureCInt();
    float getHumidity();
    float getTemperatureC();
};

// Report the humidity in .1 percent increments, such that 635 means 63.5% relative humidity
inline short int DHT22::getHumidityInt(){
  return _lastHumidity;
}

// Get the temperature in decidegrees C, such that 326 means 32.6 degrees C.
// The temperature may be negative, so be careful when handling the fractional part.
inline short int DHT22::getTemperatureCInt(){
  return _lastTemperature;
}

inline float DHT22::getHumidity(){
  return float(_lastHumidity)/10;
}

inline float DHT22::getTemperatureC(){
  return float(_lastTemperature)/10;
}

#endif /*_DHT22_H_*/
