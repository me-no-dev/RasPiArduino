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

#define TTY_SIGINT    3  //^C
#define TTY_EOF       4  //^D
#define TTY_BCKSPACE  8  //\b
#define TTY_TAB       9  //\t
#define TTY_LINE      10 //\n
#define TTY_ENTER     13 //\r
#define TTY_FLUSH     15 //^O
#define TTY_START     17 //^Q
#define TTY_ZPRINT    18 //^R
#define TTY_STOP      19 //^S
#define TTY_KILL      21 //^U
#define TTY_LNEXT     22 //^V
#define TTY_WERASE    23 //^W
#define TTY_SUSUPEND  26 //^Z
#define TTY_ESC       27
#define TTY_QUIT      28 //^/
#define TTY_SPACE     32 //\s
#define TTY_DEL       127
#define TTY_F1        "\x1B" "OP"
#define TTY_F2        "\x1B" "OQ"
#define TTY_F3        "\x1B" "OR"
#define TTY_F4        "\x1B" "OS"
#define TTY_F5        "\x1B" "[15~"
#define TTY_F6        "\x1B" "[17~"
#define TTY_F7        "\x1B" "[18~"
#define TTY_F8        "\x1B" "[19~"
#define TTY_F9        "\x1B" "[20~"
#define TTY_F10       "\x1B" "[21~"
#define TTY_F11       "\x1B" "[23~"
#define TTY_F12       "\x1B" "[24~"
#define TTY_CTRL(c)   ((char)(c) ^ 0x40)

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
