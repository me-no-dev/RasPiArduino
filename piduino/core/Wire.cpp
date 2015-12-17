extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}

#include "Arduino.h"
#include "Wire.h"


uint8_t twi_writeTo(uint8_t address, const uint8_t * buf, uint32_t len, uint8_t var, uint8_t sendStop){
    uint32_t remaining = len;
    uint32_t i = 0;
    uint8_t reason = 0;

    BSC1A = address;
    BSC1C |= _BV(BSCCLEAR);
    BSC1S = _BV(BSCCLKT) | _BV(BSCERR) | _BV(BSCDONE);
    BSC1DLEN = len;
    while(remaining && ( i < 16 )){
      BSC1FIFO = buf[i];
      i++;
      remaining--;
    }
    
    BSC1C = _BV(BSCI2CEN) | _BV(BSCST);
    while(!(BSC1S & _BV(BSCDONE))){
        while ( remaining && (BSC1S & _BV(BSCTXD))){
          BSC1FIFO = buf[i];
          i++;
          remaining--;
      }
    }

    if (BSC1S & _BV(BSCERR)){
      reason = 1;
    } else if (BSC1S & _BV(BSCCLKT)){
      reason = 2;
    } else if (remaining){
      reason = 3;
    }
    BSC1C |= _BV(BSCDONE);
    return reason;
}

uint8_t twi_readFrom(uint8_t address, uint8_t* buf, uint32_t len, uint8_t sendStop){
    uint32_t remaining = len;
    uint32_t i = 0;
    uint8_t reason = 0;

    BSC1A = address;
    BSC1C |= _BV(BSCCLEAR);
    BSC1S = _BV(BSCCLKT) | _BV(BSCERR) | _BV(BSCDONE);
    BSC1DLEN = len;
    BSC1C = _BV(BSCI2CEN) | _BV(BSCST) | _BV(BSCREAD);
    
    while (!(BSC1S & _BV(BSCDONE))){
        while (BSC1S & _BV(BSCRXD)){
        buf[i] = BSC1FIFO;
          i++;
          remaining--;
      }
    }
    
    while (remaining && (BSC1S & _BV(BSCRXD))){
        buf[i] = BSC1FIFO;
        i++;
        remaining--;
    }
    
    if (BSC1S & _BV(BSCERR)){
      reason = 1;
    } else if (BSC1S & _BV(BSCCLKT)){
      reason = 2;
    } else if (remaining){
      reason = 3;
    }
    BSC1C |= _BV(BSCDONE);
    return reason;
}

uint8_t TwoWire::rxBuffer[BUFFER_LENGTH];
uint8_t TwoWire::rxBufferIndex = 0;
uint8_t TwoWire::rxBufferLength = 0;

uint8_t TwoWire::txAddress = 0;
uint8_t TwoWire::txBuffer[BUFFER_LENGTH];
uint8_t TwoWire::txBufferIndex = 0;
uint8_t TwoWire::txBufferLength = 0;

volatile uint8_t bsc1_is_busy = 0;

TwoWire::TwoWire(){}

void TwoWire::begin(void){
  rxBufferIndex = 0;
  rxBufferLength = 0;

  txBufferIndex = 0;
  txBufferLength = 0;
  
  pinMode(2, ALT0);
  pinMode(3, ALT0);

  BSC1DIV = BSCF2DIV(100000);
}

void TwoWire::end(void){
  pinMode(2, INPUT);
  pinMode(3, INPUT);
}

void TwoWire::setClock(uint32_t frequency){
	if(bsc1_is_busy) return;
	BSC1DIV = BSCF2DIV(frequency);
}

uint8_t TwoWire::send(uint8_t address, uint8_t *data, uint8_t len){
	while(bsc1_is_busy);
	bsc1_is_busy = 1;
	uint8_t result = twi_writeTo(address, data, len, 0, true);
	bsc1_is_busy = 0;
	return result;
}

uint8_t TwoWire::receive(uint8_t address, uint8_t *data, uint8_t len){
	while(bsc1_is_busy);
	bsc1_is_busy = 1;
	uint8_t result = twi_readFrom(address, data, len, true);
	bsc1_is_busy = 0;
	return result;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop){
	if(quantity > BUFFER_LENGTH) quantity = BUFFER_LENGTH;
	uint8_t read = receive(address, rxBuffer, quantity);
	rxBufferIndex = 0;
	rxBufferLength = (read == 0)?quantity:0;
	return (read == 0)?quantity:0;
}

void TwoWire::beginTransmission(uint8_t address){
	while(bsc1_is_busy);
	bsc1_is_busy = 1;
	txAddress = address;
	txBufferIndex = 0;
	txBufferLength = 0;
}

uint8_t TwoWire::endTransmission(uint8_t sendStop){
	bsc1_is_busy = 0;
	uint8_t ret = send(txAddress, txBuffer, txBufferLength);
	txBufferIndex = 0;
	txBufferLength = 0;
	return ret;
}

uint8_t TwoWire::endTransmission(void){
	return endTransmission(true);
}

size_t TwoWire::write(uint8_t data){
	if(txBufferLength >= BUFFER_LENGTH) return 0;
	txBuffer[txBufferIndex] = data;
	++txBufferIndex;
	txBufferLength = txBufferIndex;
	return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity){
	for(size_t i = 0; i < quantity; ++i){
		write(data[i]);
	}
	return quantity;
}

int TwoWire::available(void){
	return rxBufferLength - rxBufferIndex;
}

int TwoWire::read(void){
	int value = -1;
	if(rxBufferIndex < rxBufferLength){
		value = rxBuffer[rxBufferIndex];
		++rxBufferIndex;
	}
	return value;
}

int TwoWire::peek(void){
	int value = -1;
	if(rxBufferIndex < rxBufferLength){
		value = rxBuffer[rxBufferIndex];
	}
	return value;
}

void TwoWire::flush(void){}

void TwoWire::onReceive( void (*function)(int)){}

void TwoWire::onRequest( void (*function)(void) ){}

void TwoWire::beginTransmission(int address){
	beginTransmission((uint8_t)address);
}

void TwoWire::begin(uint8_t address){
	begin();
}

void TwoWire::begin(int address){
	begin((uint8_t)address);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity){
	return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
}

uint8_t TwoWire::requestFrom(int address, int quantity){
	return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop){
	return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);
}

TwoWire Wire = TwoWire();

