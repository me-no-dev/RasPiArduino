/*
  Arduino.c -  Partial implementation of the Wiring API for the Raspberry Pi
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

void pinMode(uint8_t pin, uint8_t mode){
    GPFSEL(pin) &= ~(0x07 << GPFSELB(pin));//clear gpio function
    GPFSEL(pin) |= ((mode & 0x07) << GPFSELB(pin));//set function to pin
    if((mode & 0x07) == 0){//PULLUP/PULLDOWN
        mode &= 0x30;
        mode >>= 4;
        GPPUD = mode;
        delayMicroseconds(10);
        if(pin < 32) {
            GPPUDCLK0 = _BV(pin);
        } else if(pin < 46) {
            GPPUDCLK1 = _BV(pin - 32);
        }
        delayMicroseconds(10);
        GPPUD = 0;
        if(pin < 32) {
            GPPUDCLK0 = 0;
        } else if(pin < 46) {
            GPPUDCLK1 = 0;
        }
    }
}

void digitalWrite(uint8_t pin, uint8_t val){
    if(pin < 32){
        if(val) {
            GPSET0 = _BV(pin);
        } else {
            GPCLR0 = _BV(pin);
        }
    } else if(pin < 46){
        if(val) {
            GPSET1 = _BV(pin - 32);
        } else {
            GPCLR1 = _BV(pin - 32);
        }
    }
}

int digitalRead(uint8_t pin){
    if(pin < 32){
        return (GPLEV0 & _BV(pin)) != 0;
    } else if(pin > 45) {
        return 0;
    }
    return (GPLEV1 & _BV(pin - 32)) != 0;
}
