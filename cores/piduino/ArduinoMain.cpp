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

