#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

#include "LinuxConsole.h"

void LinuxConsole::begin(){
	console_begin();
}

void LinuxConsole::end(){
  	console_end();
}

int LinuxConsole::available(void){
  	return console_available();
}

int LinuxConsole::peek(void){
	return console_peek();
}

int LinuxConsole::read(void){
	return console_read();
}

void LinuxConsole::flush(){
  	console_flush();
}

size_t LinuxConsole::write(uint8_t c){
  console_write(c);
  return 1;
}

LinuxConsole Console;



