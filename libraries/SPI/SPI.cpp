/*
  SPI Master library for Raspberry Pi Arduino.
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

void SPIClass::beginTransaction(SPISettings settings){
  SPI0CLK = SPI0F2DIV(settings.freq);
  SPI0CS &= ~SPI_MODE3;
  SPI0CS |= settings.mode;
}

SPIClass SPI;
