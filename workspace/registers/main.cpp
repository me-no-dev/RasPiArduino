#include "Arduino.h"
#include "DS1307.h"
#include "NOKIA_LCD.h"
#include <DHT22.h>
#include "RF24.h"

#define RADIO_CE_PIN 6
#define RADIO_CS_PIN 8

#define RADIO_CHANNEL 0x55
#define RADIO_DATA_RATE RF24_1MBPS
#define RADIO_CRC_COUNT RF24_CRC_8

#define RADIO_BUFF_SIZE 256

RF24 radio(RADIO_CE_PIN, RADIO_CS_PIN);


DHT22 myDHT22(5);

NOKIA_LCD nokia_lcd(26, 27, 23, 25, 24);// MOSI,SCK,CS,DC,RST
DS1307 rtc;
static rtc_t rtc_time;



const uint64_t rf24_address = 0xE8E8F0F0E1LL;
volatile uint8_t radio_buffer[RADIO_BUFF_SIZE];
volatile uint8_t radio_buffer_len = 0;

void send_packet(void){
  if(radio_buffer_len > 0){
    if(radio_buffer_len > 32){
      radio.startWrite((const void *)&radio_buffer, 32);
      radio_buffer_len -= 32;
      int i;
      for(i = 0; i < radio_buffer_len; i++) radio_buffer[i] = radio_buffer[i+32];
    } else {
      radio.startWrite((const void *)&radio_buffer, radio_buffer_len);
      radio_buffer_len = 0;
    }
  } else {
    uint8_t out[1] = {0};
    radio.startWrite(&out, 1);
  }
}

void radio_write(uint8_t data){
	if(radio_buffer_len < RADIO_BUFF_SIZE) radio_buffer[radio_buffer_len++] = data;
}

PTHREAD check_radio(void *arg){
	radio.begin();
	radio.setDataRate(RADIO_DATA_RATE);
	radio.setCRCLength(RADIO_CRC_COUNT);
	radio.setChannel(RADIO_CHANNEL);
	radio.setRetries(4, 2);
	radio.setAutoAck(true);
	radio.enableAckPayload();
	radio.enableDynamicPayloads();
	radio.openWritingPipe(rf24_address);
	send_packet();

	bool tx,fail,rx,full;
	uint8_t pipenum;

	while(1){
		if(radio.whatHappened(tx,fail,rx,full,pipenum)){
			if (rx){
				uint8_t len = radio.getDynamicPayloadSize();
				uint8_t data[len];
				uint8_t i = 0;
				radio.read(&data,len);
				for(i=0; i<len; i++){
					Console.write(data[i]);
				}
			}
			if (tx || fail){
				send_packet();
			}
		}

		delay(1);
	}

	pthread_exit(NULL);
	return 0;
}

PTHREAD clock(void *arg){
	char msg[64];
	rtc.begin();
	//rtc.set();

	nokia_lcd.begin();
	nokia_lcd.setCursor(0,0);

	while(1){
		if(rtc.read(rtc_time)) {
			nokia_lcd.setCursor(0,0);
			sprintf(msg, "%02u/%02u/20%02u", rtc_time.d, rtc_time.mo, rtc_time.y);
			nokia_lcd.write(msg);
			nokia_lcd.setCursor(0,1);
			sprintf(msg, "%02u:%02u:%02u", rtc_time.h, rtc_time.m, rtc_time.s);
			nokia_lcd.write(msg);
		}
		if(myDHT22.readData() == DHT_ERROR_NONE){
			nokia_lcd.setCursor(0,2);
			sprintf(msg, "%+2.1fC %2.1f%%\n", myDHT22.getTemperatureC(), myDHT22.getHumidity());
			nokia_lcd.write(msg);
		}
		delay(995);
	}
	pthread_exit(NULL);
	return 0;
}

void setup(){
	create_thread(clock);
	//Serial.begin(115200);
	//Console.begin();
	//create_thread(check_radio);
	printf("HERE WE GO :) %llu\n", micros());
}

void loop(){
	//while(Console.available()) Serial.write(Console.read());
	//while(Serial.available()) Console.write(Serial.read());
	//while(Console.available() && radio_buffer_len < RADIO_BUFF_SIZE) radio_write(Console.read());
	delay(100);
}
