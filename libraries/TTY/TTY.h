/*
  Server.h - TTY class for Raspberry Pi
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

#ifndef pi_tty_h
#define pi_tty_h

#include "Arduino.h"
#include "Print.h"

class TTYClass : public Print {
  private:
    char _tty[16];
    int _fd;
    uint8_t _put_c(uint8_t c);

  public:
    operator bool(){return _fd > 0;}
    TTYClass(uint8_t tty);
    ~TTYClass();
    void end();
    size_t write(uint8_t c);
    size_t println(void);
    using Print::println;
};

extern TTYClass TTY1;

#endif
