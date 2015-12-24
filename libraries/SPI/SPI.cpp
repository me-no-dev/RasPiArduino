#include "pins_arduino.h"
#include "SPI.h"

void SPIClass::begin() {
  pinMode(9, ALT0);
  pinMode(10, ALT0);
  pinMode(11, ALT0);
  SPI0CLK = SPI0F2DIV(1000000);//1MHz
  SPI0CS = _BV(SPI0CLEAR_RX) | _BV(SPI0CLEAR_TX);
}

void SPIClass::end() {
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  SPI0CS = _BV(SPI0CLEAR_RX) | _BV(SPI0CLEAR_TX);
}

void SPIClass::setDataMode(uint32_t mode){
  SPI0CS &= ~SPI_MODE3;
  SPI0CS |= mode;
}

void SPIClass::setClockDivider(uint32_t rate){
  SPI0CLK = rate;
}

void SPIClass::setClock(uint32_t rate){
  setClockDivider(SPI0F2DIV(rate));
}

uint8_t SPIClass::transfer(uint8_t data) {
  SPI0CS |= _BV(SPI0TA) | _BV(SPI0CLEAR_RX) | _BV(SPI0CLEAR_TX);
  SPI0FIFO = data;
  while (!(SPI0CS & _BV(SPI0RXD)));
  uint32_t ret = SPI0FIFO;
  SPI0CS &= ~_BV(SPI0TA);
  return ret;
}

SPIClass SPI;
