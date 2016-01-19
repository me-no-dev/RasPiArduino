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
#include "DS3231.h"
#include <ctime>
#include <iostream>
#include "Wire.h"
using namespace std;

#define dec2bcd(n) ((n % 10) + ((uint8_t)(n / 10) << 4)) // Convert Decimal to Binary Coded Decimal (BCD)
#define bcd2dec(n) ((n & 0x0F) + (10 * ((n & 0xF0) >> 4)))// Convert Binary Coded Decimal (BCD) to Decimal

void DS3231::begin(){
  Wire.begin();
  Wire.setClock(400000);
}

int DS3231::getReg(uint8_t reg){
  Wire.beginTransmission(0x68);
  Wire.write(reg);
  if(Wire.endTransmission()) return -1;
  if(Wire.requestFrom(0x68, 1) == 0) return -1;
  return Wire.read();
}

bool DS3231::setReg(uint8_t reg, uint8_t value){
  Wire.beginTransmission(0x68);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool DS3231::updateReg(uint8_t reg, uint8_t value, uint8_t mask){
  int regval = getReg(reg);
  if(regval == -1) return false;
  return setReg(reg, (regval & ~mask) | value);
}

bool DS3231::read(uint8_t reg, uint8_t *buf, uint16_t len){
  Wire.beginTransmission(0x68);
  Wire.write(reg);
  if(Wire.endTransmission()) return false;
  len = Wire.requestFrom(0x68, len);
  if(!len) return false;
  for(uint8_t i = 0; i < len; i++)
    buf[i] = Wire.read();
  return len > 0;
}

bool DS3231::write(uint8_t reg, uint8_t *buf, uint16_t len){
  Wire.beginTransmission(0x68);
  Wire.write(reg);
  for (uint8_t i=0; i<len; i++)
    Wire.write(buf[i]);
  return Wire.endTransmission() == 0;
}

bool DS3231::read(rtc_t &tm){
  uint8_t buf[7];
  if(read(0x00, buf, 7)){
    if ((buf[0] & 0x80) == 0){
      tm.s = bcd2dec(buf[0] & 0x7F);
      tm.m = bcd2dec(buf[1] & 0x7F);
      tm.h = (buf[2] & 0x40)?(bcd2dec(buf[2] & 0x1F) + (12 * ((buf[2] & 0x20) >> 5))):bcd2dec(buf[2] & 0x3F);
      tm.w = bcd2dec(buf[3] & 0x07);
      tm.d = bcd2dec(buf[4] & 0x3F);
      tm.mo = bcd2dec(buf[5] & 0x1F);
      tm.y = bcd2dec(buf[6]);
      return true;
    }
  }
  return false;
}

bool DS3231::write(rtc_t &tm){
  uint8_t buf[7];
  buf[0] = dec2bcd(tm.s);   
  buf[1] = dec2bcd(tm.m);
  buf[2] = dec2bcd(tm.h);      // sets 24 hour format
  buf[3] = dec2bcd(tm.w);   
  buf[4] = dec2bcd(tm.d);
  buf[5] = dec2bcd(tm.mo);
  buf[6] = dec2bcd(tm.y); 
  return write(0x00, buf, 7);
}

bool DS3231::set(){
  rtc_t rtm;
  time_t timev = time(0);
  struct tm * now = localtime(&timev);
  rtm.s = now->tm_sec;
  rtm.m = now->tm_min;
  rtm.h = now->tm_hour;
  rtm.w = now->tm_wday?now->tm_wday:7;
  rtm.d = now->tm_mday;
  rtm.mo = now->tm_mon + 1;
  rtm.y = now->tm_year - 100;
  return write(rtm) && setReg(RTCCR, RTCINTCN) && setReg(RTCSR, 0);
}

bool DS3231::hasStopped(){
  return (getReg(RTCSR) & RTCOSF);
}

uint16_t DS3231::temperature(){
  uint16_t temp = 0;
  temp += getReg(RTCTH) * 100;
  temp += (getReg(RTCTL) >> 6) * 25;
  return temp;
}

bool DS3231::setAlarm1(int seconds, int minutes, int hour, int day, bool weekday){
  uint8_t data[4];
  if(day <= 0 || hour == -1 || minutes == -1 || seconds == -1){
    data[3] = 0x80;//A1M4 = 1; ignore day
  } else {
    if(weekday){
      day -= 1; //make it start from 0
      day %= 7; //make sure we are in range (0-6)
      day += 1; // return back to 1-7
      data[3] = 0x40 | day;
    } else {
      day -= 1; //go to 0-30
      day %= 31; // make sure it's in 0-30
      day += 1; //go back to 1-31
      data[3] = ((day/10) << 4) | (day % 10);
    }
  }
  
  if(hour == -1 || minutes == -1 || seconds == -1){
    data[2] = 0x80;//A1M3 = 1; ignore hour
  } else {
    hour %= 24;//ensure we are in 0-23 range
    data[2] = ((hour/10) << 4) | (hour % 10);
  }
  
  if(minutes == -1 || seconds == -1){
    data[1] = 0x80;//A1M2 = 1; ignore minutes
  } else {
    minutes %= 60;//ensure we are in 0-59 range
    data[1] = ((minutes/10) << 4) | (minutes % 10);
  }
  
  if(seconds == -1){
    data[0] = 0x80;//A1M1 = 1; ignore seconds
  } else {
    seconds %= 60;//ensure we are in 0-59 range
    data[0] = ((seconds/10) << 4) | (seconds % 10);
  }
  return write(RTCA1, data, 4) && updateReg(RTCCR, RTCA1IE, RTCA1IE) && updateReg(RTCSR, 0, RTCA1F);
}

bool DS3231::hasAlarm1(){
  uint8_t reg = getReg(RTCSR);
  if(reg & RTCA1F) setReg(RTCSR, reg & ~RTCA1F);
  return (reg & RTCA1F) != 0;
}

bool DS3231::enableAlarm1(){
  return setReg(RTCCR, getReg(RTCCR) | RTCA1IE);
}

bool DS3231::disableAlarm1(){
  return setReg(RTCCR, getReg(RTCCR) & ~RTCA1IE);
}

bool DS3231::isAlarm1Enabled(){
  return (getReg(RTCCR) & RTCA1IE) != 0;
}



bool DS3231::setAlarm2(int minutes, int hour, int day, bool weekday){
  uint8_t data[3];
  if(day <= 0 || hour == -1 || minutes == -1){
    data[2] = 0x80;//A2M4 = 1; ignore day
  } else {
    if(weekday){
      day -= 1; //make it start from 0
      day %= 7; //make sure we are in range (0-6)
      day += 1; // return back to 1-7
      data[2] = 0x40 | day;
    } else {
      day -= 1; //go to 0-30
      day %= 31; // make sure it's in 0-30
      day += 1; //go back to 1-31
      data[2] = ((day/10) << 4) | (day % 10);
    }
  }
  
  if(hour == -1 || minutes == -1){
    data[1] = 0x80;//A2M3 = 1; ignore hour
  } else {
    hour %= 24;//ensure we are in 0-23 range
    data[1] = ((hour/10) << 4) | (hour % 10);
  }
  
  if(minutes == -1){
    data[0] = 0x80;//A2M2 = 1; ignore minutes
  } else {
    minutes %= 60;//ensure we are in 0-59 range
    data[0] = ((minutes/10) << 4) | (minutes % 10);
  }
  return write(RTCA2, data, 3) && updateReg(RTCCR, RTCA2IE, RTCA2IE) && updateReg(RTCSR, 0, RTCA2F);
}

bool DS3231::enableAlarm2(){
  return setReg(RTCCR, getReg(RTCCR) | RTCA2IE);
}

bool DS3231::disableAlarm2(){
  return setReg(RTCCR, getReg(RTCCR) & ~RTCA2IE);
}

bool DS3231::isAlarm2Enabled(){
  return (getReg(RTCCR) & RTCA2IE) != 0;
}

bool DS3231::hasAlarm2(){
  uint8_t reg = getReg(RTCSR);
  if(reg & RTCA2F) setReg(RTCSR, reg & ~RTCA2F);
  return (reg & RTCA2F) != 0;
}



