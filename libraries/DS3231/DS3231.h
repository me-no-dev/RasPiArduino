/*
  DS3231 library for Raspberry Pi Arduino.
  Copyright (c) 2015 by Hristo Gochkov

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef DS3231_h
#define DS3231_h
#include "Arduino.h"

#define DS3231_SQW_MASK  0x18
#define DS3231_SQW_1HZ   0x00
#define DS3231_SQW_1KHZ  0x08
#define DS3231_SQW_4KHZ  0x10
#define DS3231_SQW_8KHZ  0x18

//registers
#define RTCDR       0x00 //data register start
#define RTCA1       0x07 //Alarm1 register start
#define RTCA2       0x0B //Alarm 2 register start
#define RTCCR       0x0E //Control Register
#define RTCSR       0x0F //Status Register
#define RTCAO       0x10 //Aging Offset
#define RTCTH       0x11 //Temperature MSB
#define RTCTL       0x12 //Temperature LSB (value * 0.25 deg)

//Control register bits
#define RTCEOSC     (1 << 7) //Enable Oscillator (active low)
#define RTCBBSQW    (1 << 6) //Battery-Backed Square-Wave Enable
#define RTCCONV     (1 << 5) //Convert Temperature
#define RTCRS2      (1 << 4) //Rate Select
#define RTCRS1      (1 << 3) //Rate Select
#define RTCINTCN    (1 << 2) //Interrupt Control
#define RTCA2IE     (1 << 1) //Alarm 2 Interrupt Enable
#define RTCA1IE     (1 << 0) //Alarm 1 Interrupt Enable

//Status register bits
#define RTCOSF      (1 << 7) //Oscillator Stop Flag (clear needed)
#define RTCEN32kHz  (1 << 3) //Enable 32kHz Output
#define RTCBSY      (1 << 2) //Busy
#define RTCA2F      (1 << 1) //Alarm 2 Flag
#define RTCA1F      (1 << 0) //Alarm 1 Flag

typedef struct  { 
  uint8_t s; 
  uint8_t m; 
  uint8_t h;
  uint8_t w;
  uint8_t d;
  uint8_t mo; 
  uint8_t y;   // offset from 2000;
} rtc_t;

class DS3231 {
  public:
    DS3231(){}
    ~DS3231(){}
    void begin();
    int  getReg(uint8_t address);
    bool setReg(uint8_t address, uint8_t value);
    bool updateReg(uint8_t address, uint8_t value, uint8_t mask);
    bool read(uint8_t address, uint8_t *data, uint16_t len);
    bool read(rtc_t &tm);
    bool write(uint8_t address, uint8_t *data, uint16_t len);
    bool write(rtc_t &tm);
    bool set(void);
    bool hasStopped(void);
    uint16_t temperature(void);
    
    bool setAlarm1(int seconds=-1, int minutes=-1, int hour=-1, int day=-1, bool weekday=false);
    bool enableAlarm1(void);
    bool disableAlarm1(void);
    bool hasAlarm1(void);
    bool isAlarm1Enabled(void);
    
    bool setAlarm2(int minutes=-1, int hour=-1, int day=-1, bool weekday=false);
    bool enableAlarm2(void);
    bool disableAlarm2(void);
    bool hasAlarm2(void);
    bool isAlarm2Enabled(void);
};

#endif
