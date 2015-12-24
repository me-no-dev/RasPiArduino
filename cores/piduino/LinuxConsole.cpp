#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

#include "LinuxConsole.h"
#include "idemonitor.h"

void LinuxConsole::begin(){
  console_begin();
}

void LinuxConsole::end(){
    console_end();
}

int LinuxConsole::available(void){
  return idemonitor_available() + console_available();
}

int LinuxConsole::peek(void){
  if(idemonitor_connected() && idemonitor_available())
    return idemonitor_peek();
  return console_peek();
}

int LinuxConsole::read(void){
  if(idemonitor_connected() && idemonitor_available())
    return idemonitor_read_char();
  return console_read();
}

void LinuxConsole::flush(){
  idemonitor_flush();
  console_flush();
}

size_t LinuxConsole::write(uint8_t c){
  if(idemonitor_connected())
    idemonitor_write((char*)&c, 1);
  console_write(c);
  return 1;
}

LinuxConsole Console;



