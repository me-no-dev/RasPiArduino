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
  thread_set_priority(65);
  _keep_sketch_running = 1;
  _loop_is_running = 0;
  console_attach_signal_handlers();
  idemonitor_begin();

  pthread_attr_t attr;
  pthread_attr_t *attrp;      /* NULL or &attr */
  if(pthread_attr_init(&attr) == 0){
    attrp = &attr;
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    //pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
  }
  if(pthread_create(&_loop_thread, attrp, _loop_thread_task, NULL) == 0){
    if (attrp != NULL) pthread_attr_destroy(attrp);
    pthread_setname_np(_loop_thread, "arduino-loop");
    pthread_detach(_loop_thread);
  }
  usleep(1000);
  while(_keep_sketch_running){
    console_run();
    uart_check_fifos();
    idemonitor_run();
    usleep(1000);
  }
  uninit();
  return 0;
}

