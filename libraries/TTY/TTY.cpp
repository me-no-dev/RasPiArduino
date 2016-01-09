/*
  Server.h - Server class for Raspberry Pi
  Copyright (c) 2016 Hristo Gochkov  All right reserved.

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

#include "TTY.h"

#include <sys/ioctl.h>
#include <termios.h>

uint8_t TTYClass::_put_c(uint8_t c){
  return ioctl(_fd, TIOCSTI, &c) != -1;
}

TTYClass::TTYClass(uint8_t tty){
  sprintf(_tty,"/dev/tty%u", tty);
  _fd = open(_tty, O_RDWR);
}
TTYClass::~TTYClass(){
  end();
}
void TTYClass::end(){
  if(_fd > 0)
    close(_fd);
  _fd = 0;
}
size_t TTYClass::write(uint8_t c){
  if(_fd <= 0)
    return 0;
  if(!_put_c(c)){
    end();
    return 0;
  }
  return 1;
}
size_t TTYClass::println(void){
  return print('\n');
}

TTYClass TTY1(1);
