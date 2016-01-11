/*
  LinuxConsole.cpp - Linux STDIN/STDOUT library for Wiring
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

#include "Console.h"
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



