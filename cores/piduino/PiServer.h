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

#ifndef pi_server_h
#define pi_server_h

#include "Arduino.h"
#include "Server.h"
#include "PiClient.h"

typedef void(*PiServerHandler)(PiClient);

class PiServer : public Server {
  private:
    int sockfd;
    uint16_t _port;
    uint8_t _max_clients;
    bool _listening;
    PiServerHandler _cb;
  public:
    PiServer(uint16_t port, uint8_t max_clients=8):sockfd(-1),_port(port),_max_clients(max_clients),_listening(false),_cb(NULL){}
    ~PiServer(){ end();}
    operator bool(){return _listening;}
    void begin();
    void end();
    void onClient(PiServerHandler cb);
    void handle();
};

#endif
