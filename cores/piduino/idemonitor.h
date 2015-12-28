/*
  idemonitor.h - Ardiono IDE remote monitor implementation for Raspberry Pi
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

#ifndef _IDE_MONITOR_H
#define _IDE_MONITOR_H

#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

void idemonitor_begin(void);
void idemonitor_run(void);
uint8_t idemonitor_connected();
int idemonitor_write(char * data, size_t len);
size_t idemonitor_available();
size_t idemonitor_read(char *buf, size_t len);
int idemonitor_read_char();
int idemonitor_peek();
void idemonitor_flush();

#ifdef __cplusplus
}
#endif

#endif
