/*
 * SSPI.h
 *
 *  Created on: Jul 18, 2014
 *      Author: ficeto
 */

#ifndef SSPI_H_
#define SSPI_H_

#include "Arduino.h"

#define SSPI_MSB_FIRST  0x00
#define SSPI_LSB_FIRST  0x01

#define SSPI_MODE0 		0x00
#define SSPI_MODE1 		0x01
#define SSPI_MODE2 		0x02
#define SSPI_MODE3 		0x03

class SSPI {

public:
	SSPI(uint8_t mosi, uint8_t miso, uint8_t sck);
	
	void begin(void);//setup pins
	void setMode(uint8_t mode);//default 0
	void setBitOrder(uint8_t bitOrder);//default MSB first
	void setBitCount(uint8_t bitCount);//default to 8(1 byte)
	
	uint32_t transfer(uint32_t data, uint8_t count, bool wait = false);//transmits count number of bits (wait for slave to release the line)
	uint32_t transfer(uint32_t data);//transmits _bitcount number of bits from data
	
private:
	uint8_t _mode;
	uint8_t _bit_order;
	uint8_t _bitcount;
	bool _ss_inverted;
	uint8_t _mosi;
	uint8_t _miso;
	uint8_t _sck;
};

#endif /* MYSPI_H_ */
