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

String shellExec(const char *cmd, int *result){
  String buf = "";
  char str[strlen(cmd)+6];
  int rs;
  int *r = result;
  if(r == NULL)
    r = &rs;
  r[0] = -1;
  strcpy(str, cmd);
  strcat(str, " 2>&1");
  FILE *in = popen(str, "r");
  if(!in)
    return String();
  int c = 0;
  while((c = fgetc(in)) > 0)
    buf += (char)c;
  r[0] = pclose(in);
  return buf;
}

volatile uint8_t _keep_sketch_running;
volatile uint8_t _loop_is_running;
pthread_t _loop_thread;

void request_sketch_terminate(){
  _keep_sketch_running = 0;
}

void *_loop_thread_task(void *arg){
  _loop_is_running = 1;
  setup();
  while(_keep_sketch_running) {
    loop();
    usleep(300);
  }
  _loop_is_running = 0;
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
  console_attach_signal_handlers();
  idemonitor_begin();
  if(pthread_create(&_loop_thread, NULL, _loop_thread_task, NULL) == 0){
    pthread_setname_np(_loop_thread, "arduino-loop");
    pthread_detach(_loop_thread);
  }
  usleep(1000);
  while(_loop_is_running){
    console_run();
    uart_check_fifos();
    idemonitor_run();
    usleep(1000);
  }
  uninit();
  return 0;
}

