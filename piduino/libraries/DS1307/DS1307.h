#ifndef DS1307_h
#define DS1307_h
#include "Arduino.h"

#define DS1307_OUT_LOW   0x00
#define DS1307_OUT_HIGH  0x80
#define DS1307_OUT_1HZ   0x10
#define DS1307_OUT_4KHZ  0x11
#define DS1307_OUT_8KHZ  0x12
#define DS1307_OUT_32KHZ 0x13

typedef struct  { 
  uint8_t s; 
  uint8_t m; 
  uint8_t h;
  uint8_t w;
  uint8_t d;
  uint8_t mo; 
  uint8_t y;   // offset from 2000;
} rtc_t;

class DS1307 {
  public:
    DS1307();
    void begin();
    bool read(uint8_t address, uint8_t *data, uint16_t len);
    bool read(rtc_t &tm);
    bool write(uint8_t address, uint8_t *data, uint16_t len);
    bool write(rtc_t &tm);
    bool output(uint8_t out);
    bool set(void);
};

#endif
