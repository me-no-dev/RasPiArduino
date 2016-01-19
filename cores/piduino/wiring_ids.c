/*
  I2C0 implementation for the Raspberry Pi
  Copyright (c) 2015 Hristo Gochkov.  All right reserved.

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

#include "Arduino.h"
#include "bcm2835_registers.h"

void ids_begin(void){
  pinMode(0, ALT0);
  pinMode(1, ALT0);
  BSC0DIV = BSCF2DIV(100000);
}

void ids_end(void){
  pinMode(0, INPUT);
  pinMode(1, INPUT);
}

void ids_set_freq(uint32_t frequency){
  BSC0DIV = BSCF2DIV(frequency);
}

uint8_t ids_write(uint8_t address, uint8_t * buf, uint32_t len){
  uint32_t remaining = len;
  uint32_t i = 0;
  uint8_t reason = 0;

  BSC0A = address;
  BSC0C |= _BV(BSCCLEAR);
  BSC0S = _BV(BSCCLKT) | _BV(BSCERR) | _BV(BSCDONE);
  BSC0DLEN = len;
  while(remaining && ( i < 16 )){
    BSC0FIFO = buf[i];
    i++;
    remaining--;
  }

  BSC0C = _BV(BSCI2CEN) | _BV(BSCST);
  while(!(BSC0S & _BV(BSCDONE))){
    while(remaining && (BSC0S & _BV(BSCTXD))){
      BSC0FIFO = buf[i];
      i++;
      remaining--;
    }
  }

  if (BSC0S & _BV(BSCERR)){
    reason = 1;
  } else if (BSC0S & _BV(BSCCLKT)){
    reason = 2;
  } else if (remaining){
    reason = 3;
  }
  BSC0C |= _BV(BSCDONE);
  return reason;
}

uint8_t ids_read(uint8_t address, uint8_t* buf, uint32_t len){
  uint32_t remaining = len;
  uint32_t i = 0;
  uint8_t reason = 0;

  BSC0A = address;
  BSC0C |= _BV(BSCCLEAR);
  BSC0S = _BV(BSCCLKT) | _BV(BSCERR) | _BV(BSCDONE);
  BSC0DLEN = len;
  BSC0C = _BV(BSCI2CEN) | _BV(BSCST) | _BV(BSCREAD);
  
  while(!(BSC0S & _BV(BSCDONE))){
    while (BSC0S & _BV(BSCRXD)){
      buf[i] = BSC0FIFO;
      i++;
      remaining--;
    }
  }
  
  while (remaining && (BSC0S & _BV(BSCRXD))){
    buf[i] = BSC0FIFO;
    i++;
    remaining--;
  }
  
  if (BSC0S & _BV(BSCERR)){
    reason = 1;
  } else if (BSC0S & _BV(BSCCLKT)){
    reason = 2;
  } else if (remaining){
    reason = 3;
  }
  BSC0C |= _BV(BSCDONE);
  return reason;
}