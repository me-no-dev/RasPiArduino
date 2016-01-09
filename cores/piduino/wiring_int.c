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

static volatile uint64_t _pin_isr_reg = 0;
static volatile uint64_t _pin_isr_last = 0;

void isr_check(){
  if(_pin_isr_reg != 0){
    uint64_t state = GPLEV0;
    state |= (uint64_t)GPLEV1 << 32;
    state &= _pin_isr_reg;
    int i = 0;
    uint32_t changed = state ^ _pin_isr_last;
    if(changed){
      _pin_isr_last = state;
      while(changed){
        while(!(changed & _BV(i))) i++;
        changed &= ~_BV(i);
        isr_handler_t *handler = &isr_handlers[i];
        if((state & _BV(i)) == 0 && (handler->mode == FALLING || handler->mode == CHANGE) && handler->fn) {
          start_thread(isr_executor_task, (void *)i);
        }
        else if((state & _BV(i)) != 0 && (handler->mode == RISING || handler->mode == CHANGE) && handler->fn) {
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
    _pin_isr_last |= (digitalRead(pin) << pin);
    _pin_isr_reg |= _BV(pin);
  }
}

void detachInterrupt(uint8_t pin) {
  if(pin < 46) {
    isr_handler_t *handler = &isr_handlers[pin];
    _pin_isr_reg &= ~_BV(pin);
    handler->mode = 0;
    handler->fn = 0;
  }
}
