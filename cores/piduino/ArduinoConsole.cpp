#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

#include "ArduinoConsole.h"
#include "idemonitor.h"

int ArduinoConsole::available(void){
  return idemonitor_available();
}

int ArduinoConsole::peek(void){
  return idemonitor_peek();
}

int ArduinoConsole::read(uint8_t *data, size_t len){
  return idemonitor_read((char*)data, len);
}

int ArduinoConsole::read(void){
  return idemonitor_read_char();
}

void ArduinoConsole::flush(){
  console_flush();
}

size_t ArduinoConsole::write(uint8_t c){
  return idemonitor_write((char*)&c, 1);
}

size_t ArduinoConsole::write(uint8_t *data, size_t len){
  return idemonitor_write((char*)data, len);
}

ArduinoConsole Monitor;



