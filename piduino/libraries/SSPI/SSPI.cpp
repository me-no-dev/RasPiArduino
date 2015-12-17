/*
 * SSPI.cpp
 *
 *  Created on: Jul 18, 2014
 *      Author: ficeto
 */
#include "SSPI.h"

SSPI::SSPI(uint8_t mosi, uint8_t miso, uint8_t sck){
	_mode = SSPI_MODE0;
	_bit_order = SSPI_MSB_FIRST;
	_bitcount = 8;
	_ss_inverted = false;
	_mosi = mosi;
	_miso = miso;
	_sck = sck;
}

void SSPI::begin(){
	pinMode(_mosi, OUTPUT);
	pinMode(_miso, INPUT);
	pinMode(_sck, OUTPUT);
	
	digitalWrite(_mosi, LOW);
	digitalWrite(_sck, LOW);
}

uint32_t SSPI::transfer(uint32_t data, uint8_t bitcount, bool wait){
  uint8_t index = 0;
  uint8_t i = bitcount;
  uint32_t value = 0;
  
  //for each bit
  while(i--){
    if (_bit_order == SSPI_MSB_FIRST){
      index = i;
    } else {
      index = ((bitcount - 1) - i);
    }
    
    //mode 0 and 3 slave reads on leading edge so we need to set the bit before clocking in
    if((_mode & 1) == 0) digitalWrite(_mosi, ((data & (1 << index)) != 0));
    
    //clock leading edge
    digitalWrite(_sck, ((_mode & 2) == 0));
    digitalWrite(_sck, ((_mode & 2) == 0));
    
    //modes 0 and 3 read the bit here while 2 and 4 set it to be read on trailing clock edge
    if((_mode & 1) == 0){
      value |= (digitalRead(_miso) << index);
    } else {
      digitalWrite(_mosi, ((data & (1 << index)) != 0));
    }
    
    //trailing edge
    digitalWrite(_sck, ((_mode & 2) != 0));
    digitalWrite(_sck, ((_mode & 2) != 0));
    
    //modes 2 and 4 read the bit here
    if((_mode & 1) != 0) value |= (digitalRead(_miso) << index);
  }
  
  //if requested, wait for the slave to release the miso line
  while(wait && digitalRead(_miso));
  
  return value;
}

uint32_t SSPI::transfer(uint32_t data){
  return transfer(data, _bitcount, false);
}


//setters
void SSPI::setMode(uint8_t mode){
  uint8_t m = (mode & 0x03);
  if((m & 2) != (_mode & 2)) digitalWrite(_sck, !digitalRead(_sck)); //toggle on CPOL change
  _mode = m;
}

void SSPI::setBitOrder(uint8_t bitOrder){
  _bit_order = (bitOrder & 1);
}

void SSPI::setBitCount(uint8_t bitCount){
  _bitcount = ((bitCount - 1) & 0x1F) + 1;//32 max
}
