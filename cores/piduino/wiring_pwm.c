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

static uint32_t _pwm_range = PWM_RANGE;
static uint32_t _pwm_wanted_freq = 1;
static uint32_t _pwm_real_freq = 1;
static uint8_t _pwm_has_started = 0;

void analogWriteRange(uint32_t range){
  _pwm_range = range;
  PWMRNG1 = _pwm_range;
  PWMRNG2 = _pwm_range;
}

void analogWriteDiv(uint16_t div){
  GPPCTL = GPSRC_OSC + GPCPASS;//stop clock
  while(GPPCTL & (1 << GPBUSY));//wait if running
  GPPDIV = (div << GPDIVI) | GPCPASS;//set divider
  GPPCTL = (1 << GPENAB) | GPSRC_OSC | GPCPASS;//set ctrl
}

uint32_t analogWriteSetup(uint32_t frequency, uint32_t range){
  _pwm_wanted_freq = frequency;
  uint32_t div = 19200000/(_pwm_wanted_freq*range);
  div += (div & 1) * 1;
  div &= 0xFFF;
  _pwm_real_freq = 19200000/(div*range);
  analogWriteRange(range);
  analogWriteDiv(div);
  return _pwm_real_freq;
}

void analogWriteInit(){
  if(!_pwm_has_started){
    _pwm_has_started = 1;
    _pwm_wanted_freq = analogWriteSetup(1000, 256);
    PWMCTL = 0;
  }
}

void analogWrite(uint8_t p, uint16_t v){
  v = ((v*_pwm_real_freq)/_pwm_wanted_freq) % _pwm_range;
  if(v){
    if(!_pwm_has_started)
      analogWriteInit();
    if(p == 18 || p == 19){
      pinMode(p, GPF5);
    } else if(p == 12 || p == 13 || p == 40 || p == 41 || p == 45){
      pinMode(p, GPF0);
    }
    if(p == 12 || p == 18 || p == 40){
      PWMCTL |= _BV(PWMPWEN1) | (1 << PWMMSEN1);
      PWMDAT1 = v;
    } else if(p == 13 || p == 19 || p == 41 || p == 45){
      PWMCTL |= _BV(PWMPWEN2) | (1 << PWMMSEN2);
      PWMDAT2 = v;
    }
  } else {
    if(p == 12 || p == 18 || p == 40){
      PWMCTL &= ~(_BV(PWMPWEN1) | (1 << PWMMSEN1));
      PWMDAT1 = v;
    } else if(p == 13 || p == 19 || p == 41 || p == 45){
      PWMCTL &= ~(_BV(PWMPWEN2) | (1 << PWMMSEN2));
      PWMDAT2 = v;
    }
    if(p == 18 || p == 19){
      pinMode(p, INPUT);
    } else if(p == 12 || p == 13 || p == 40 || p == 41 || p == 45){
      pinMode(p, INPUT);
    }
  }
}
