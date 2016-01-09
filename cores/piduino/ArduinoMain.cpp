/*
  main.cpp - Main loop for Arduino sketches
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
#include "idemonitor.h"

volatile uint8_t _keep_sketch_running;
volatile uint8_t _loop_is_running;

void request_sketch_terminate(){
  _keep_sketch_running = 0;
}

void *_loop_thread(void *arg){
  _loop_is_running = 1;
  setup();
  while(_keep_sketch_running) {
    loop();
    usleep(300);
  }
  //maybe do something before we exit?
  _loop_is_running = 0;
  pthread_exit(NULL);
}

void *_pin_isr_thread(void *arg){
  _loop_is_running = 1;
  while(_keep_sketch_running) {
    isr_check();
    usleep(200);
  }
  pthread_exit(NULL);
}

/**
 * Initialize the BCM2835 registers earlier than any constructors for any static Arduino classes.
 * Note the usage of the gcc constructor(101) attribute.  This registers the function as a constructor which will
 * be automatically invoked when the executable is launched.  It will be executed BEFORE main and BEFORE any of the
 * other global static C++ class instances.
 *
 * See Issue #10
 */
__attribute__((constructor(101))) void startInit() {
  if(init()) exit(1);
}

int main(int argc, char **argv){
  _keep_sketch_running = 1;
  _loop_is_running = 0;
  //elevate_prio(55);
  idemonitor_begin();
  console_attach_signal_handlers();
  create_named_thread(_loop_thread,"arduino-loop");
  create_named_thread(_pin_isr_thread,"arduino-isr");
  while(_keep_sketch_running) {
    uart_check_fifos();
    idemonitor_run();
    console_run();
    usleep(1000);
  }
  while(_loop_is_running)
    usleep(300);
  uninit();
  return 0;
}

