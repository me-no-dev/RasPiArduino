/*
  Client.h - Base class that provides Client
  Copyright (c) 2011 Adrian McEwen.  All right reserved.

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

#ifndef pi_client_h
#define pi_client_h

#include "Arduino.h"
#include "Client.h"

class PiClient : public Client {

public:
  PiClient():sockfd(-1),_connected(false){}
  PiClient(int fd):sockfd(fd),_connected(true){}
  ~PiClient();
  int fd(){return sockfd;}
  int connect(IPAddress ip, uint16_t port);
  int connect(const char *host, uint16_t port);
  int setSocketOption(int option, char* value, size_t len);
  int setOption(int option, int *value);
  int getOption(int option, int *value);
  int setTimeout(uint32_t seconds);
  int setNoDelay(bool nodelay);
  bool getNoDelay();
  size_t write(uint8_t data);
  size_t write(const uint8_t *buf, size_t size);
  int read();
  int read(uint8_t *buf, size_t size);
  int available();
  int peek(){return 0;}
  void flush(){}
  void stop();
  uint8_t connected();
  operator bool(){return connected();}
protected:
  int sockfd;
  bool _connected;
};

#endif
