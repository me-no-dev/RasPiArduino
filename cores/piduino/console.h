/*
  console.h - Linux STDIN/STDOUT library for Raspberry Pi
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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

void console_begin(void);
void console_end(void);
void console_run(void);
void console_attach_signal_handlers(void);
void console_signal_handler(int);

void console_echo(void);
void console_noecho(void);
void console_mode_raw(void);
void console_mode_line(void);

int console_rx_empty(void);

size_t console_tx(uint8_t data);
uint8_t console_rx(void);

uint8_t console_read(void);
uint8_t console_peek(void);
void console_flush(void);
void console_write(uint8_t str);
int console_available(void);


#ifdef __cplusplus
}
#endif

#endif
