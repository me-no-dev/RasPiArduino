/*
  I2C Master library for Raspberry Pi Arduino.
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
#include "Arduino.h"
#include "twi.h"

uint8_t TWBR;

uint8_t twi_writeTo(uint8_t address, uint8_t * buf, uint32_t len, uint8_t var, uint8_t sendStop){
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
    while(remaining && (BSC1S & _BV(BSCTXD))){
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
  
  while(!(BSC1S & _BV(BSCDONE))){
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
