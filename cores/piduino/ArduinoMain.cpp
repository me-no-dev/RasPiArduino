#include "Arduino.h"


void *user_code_thread(void *arg){
  setup();
  while(1) {
    loop();
    nap(500);
  }
  pthread_exit(NULL);
}

int main(int argc, char **argv){
  //elevate_prio(55);
  if(init()) exit(1);
  console_attach_signal_handlers();
  create_thread(user_code_thread);
  while(1) {
    isr_check();
    uart_check_fifos();
    console_run();
    usleep(200);
  }
  uninit();
    return 0;
}

