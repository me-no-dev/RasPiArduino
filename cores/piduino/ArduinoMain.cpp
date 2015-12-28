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

void *user_code_thread(void *arg){
  setup();
  while(1) {
    loop();
    usleep(500);
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
  //elevate_prio(55);
  idemonitor_begin();
  console_attach_signal_handlers();
  create_thread(user_code_thread);
  while(1) {
    isr_check();
    uart_check_fifos();
    idemonitor_run();
    console_run();
    usleep(200);
  }
  uninit();
    return 0;
}

