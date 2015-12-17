#include "Arduino.h"
#include "DS3231.h"

DS3231 rtc;
rtc_t rtc_time;

void dumpRegs(){
    uint8_t registers[19];
    rtc.read(0, registers, 19);
    for (uint8_t i =0; i < 0xE; i++) printf("REG 0x%02X: 0x%02X\n", i, registers[i]);
    for (uint8_t i = 0x0E; i < 0x13; i++){
        printf("REG 0x%02X: ", i);
        Console.println(registers[i], BIN);
    }  
}

void getTime(){
    if(!rtc.read(rtc_time)){
	    printf("Could not read the time!\n");
	} else {
	    double temp = (int)rtc.temperature() / 100.0;
	    printf("%02u/%02u/20%02u %02u:%02u:%02u %+2.2fC\n", rtc_time.d, rtc_time.mo, rtc_time.y, rtc_time.h, rtc_time.m, rtc_time.s, temp);
	}
}

void checkClock(){
    if(rtc.hasAlarm1()) printf("Alarm 1 ");
    if(rtc.hasAlarm2()) printf("Alarm 2 ");
    getTime();
}

void setup(){
    pinMode(18, INPUT_PULLUP);
    attachInterrupt(18, checkClock, FALLING);

	rtc.begin();
	
	
	if(rtc.hasStopped()){
	    printf("RTC was stopped!\n");
	    if(!rtc.set()){//set to current system time
            printf("Could not set the time!\n");
        }
	}
	getTime();
	rtc.setAlarm1(30);
	rtc.setAlarm2();
	
	dumpRegs();
}

void loop(){
	
}
