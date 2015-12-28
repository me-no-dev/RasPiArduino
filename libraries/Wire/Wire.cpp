extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}

#include "Arduino.h"
#include "Wire.h"
#include "utility/twi.h"

uint8_t TwoWire::rxBuffer[BUFFER_LENGTH];
uint8_t TwoWire::rxBufferIndex = 0;
uint8_t TwoWire::rxBufferLength = 0;

uint8_t TwoWire::txAddress = 0;
uint8_t TwoWire::txBuffer[BUFFER_LENGTH];
uint8_t TwoWire::txBufferIndex = 0;
uint8_t TwoWire::txBufferLength = 0;

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
  BSC1DIV = BSCF2DIV(frequency);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop){
  if(quantity > BUFFER_LENGTH) quantity = BUFFER_LENGTH;
  uint8_t read = twi_readFrom(address, rxBuffer, quantity, sendStop);
  rxBufferIndex = 0;
  rxBufferLength = (read == 0)?quantity:0;
  return rxBufferLength;
}

void TwoWire::beginTransmission(uint8_t address){
  txAddress = address;
  txBufferIndex = 0;
  txBufferLength = 0;
}

uint8_t TwoWire::endTransmission(uint8_t sendStop){
  uint8_t ret = twi_writeTo(txAddress, txBuffer, txBufferLength, 0, sendStop);
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

void TwoWire::begin(uint8_t address){
  begin();
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity){
  return requestFrom(address, quantity, (uint8_t)true);
}

TwoWire Wire = TwoWire();

