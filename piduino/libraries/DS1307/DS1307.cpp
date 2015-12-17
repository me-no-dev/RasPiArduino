#include "DS1307.h"
#include <ctime>
#include <iostream>
using namespace std;

#define dec2bcd(n) ((n % 10) + ((uint8_t)(n / 10) << 4)) // Convert Decimal to Binary Coded Decimal (BCD)
#define bcd2dec(n) ((n & 0x0F) + (10 * ((n & 0xF0) >> 4)))// Convert Binary Coded Decimal (BCD) to Decimal

DS1307::DS1307(){}

void DS1307::begin(){
	Wire.begin();
}

bool DS1307::read(uint8_t reg, uint8_t *buf, uint16_t len){
	Wire.setClock(100000);
	Wire.beginTransmission(0x68);
	Wire.write(reg);
	if(Wire.endTransmission()) return false;
	len = Wire.requestFrom(0x68, len);
	if(!len) return false;
	for(uint8_t i = 0; i < len; i++) buf[i] = Wire.read();
	return len > 0;
}

bool DS1307::write(uint8_t reg, uint8_t *buf, uint16_t len){
	Wire.setClock(100000);
	Wire.beginTransmission(0x68);
	Wire.write(reg);
  	for (uint8_t i=0; i<len; i++) Wire.write(buf[i]);
  	return Wire.endTransmission() == 0;
}

bool DS1307::read(rtc_t &tm){
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

bool DS1307::write(rtc_t &tm){
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

bool DS1307::output(uint8_t value){
	return write(0x07, &value, 1);
}

bool DS1307::set(){
	rtc_t rtm;
	time_t timev = time(0);
	struct tm * now = localtime(&timev);
	rtm.s = now->tm_sec;
	rtm.m = now->tm_min;
	rtm.h = now->tm_hour;
	rtm.w = now->tm_wday + 1;
	rtm.d = now->tm_mday;
	rtm.mo = now->tm_mon + 1;
	rtm.y = now->tm_year - 100;
	return write(rtm) && output(DS1307_OUT_1HZ);
}
