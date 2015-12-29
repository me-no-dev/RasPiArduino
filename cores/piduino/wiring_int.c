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

typedef struct {
  uint8_t mode;
  void (*fn)(void);
} isr_handler_t;

static isr_handler_t isr_handlers[64];

void *isr_executor_task(void *isr_num){
  //elevate_prio(55);
  isr_handler_t *handler = &isr_handlers[(int)isr_num];
  handler->fn();
  pthread_exit(NULL);
    return 0;
}

//static volatile uint64_t isr_reg = 0;
static volatile uint64_t isr_freg = 0;
static volatile uint64_t isr_rreg = 0;

void isr_check(){
  if(isr_freg != 0){
    uint64_t isrfst = GPFEN0;
    isrfst |= (uint64_t)GPFEN1 << 32;
    if(isrfst != isr_freg){
      GPFEN0 = isr_freg;
      GPFEN1 = isr_freg >> 32;
      int i = 0;
      uint32_t changedfbits = isrfst ^ isr_freg;
      while(changedfbits){
        while(!(changedfbits & _BV(i))) i++;
        changedfbits &= ~_BV(i);
        isr_handler_t *handler = &isr_handlers[i];
        if((handler->mode == FALLING || handler->mode == CHANGE) && handler->fn) {
          start_thread(isr_executor_task, (void *)i);
        }
      }
    }
  }
  if(isr_rreg != 0){
    uint64_t isrrst = GPREN0;
    isrrst |= (uint64_t)GPREN1 << 32;
    if(isrrst != isr_rreg){
      GPREN0 = isr_rreg;
      GPREN1 = isr_rreg >> 32;
      int i = 0;
      uint32_t changedrbits = isrrst ^ isr_rreg;
      while(changedrbits){
        while(!(changedrbits & _BV(i))) i++;
        changedrbits &= ~_BV(i);
        isr_handler_t *handler = &isr_handlers[i];
        if((handler->mode == RISING || handler->mode == CHANGE) && handler->fn) {
          start_thread(isr_executor_task, (void *)i);
        }
      }
    }
  }
}

void attachInterrupt(uint8_t pin, void (*userFunc)(void), int mode) {
  if(pin < 46) {
    isr_handler_t *handler = &isr_handlers[pin];
    handler->mode = mode;
    handler->fn = userFunc;
    if(mode == FALLING || mode == CHANGE){
      isr_freg |= _BV(pin);
      if(pin < 32) GPFEN0 = isr_freg & 0xFFFFFFFF;
      else GPFEN1 = isr_freg >> 32;
    }
    if(mode == RISING|| mode == CHANGE){
      isr_rreg |= _BV(pin);
      if(pin < 32) GPREN0 = isr_rreg & 0xFFFFFFFF;
      else GPREN1 = isr_rreg >> 32;
    }
  }
}

void detachInterrupt(uint8_t pin) {
  if(pin < 46) {
    isr_handler_t *handler = &isr_handlers[pin];
    
    if(handler->mode == FALLING || handler->mode == CHANGE){
      isr_freg &= ~_BV(pin);
      if(pin < 32){
        GPFEN0 = isr_freg & 0xFFFFFFFF;
      } else {
        GPFEN1 = isr_freg >> 32;
      }
    }
    if(handler->mode == RISING|| handler->mode == CHANGE){
      isr_rreg &= ~_BV(pin);
      if(pin < 32){
        GPREN0 = isr_rreg & 0xFFFFFFFF;
      } else {
        GPREN1 = isr_rreg >> 32;
      }
    }   
    handler->mode = 0;
    handler->fn = 0;
  }
}
