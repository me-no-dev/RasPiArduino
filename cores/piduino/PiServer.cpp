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

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/tcp.h>
#include <sys/ioctl.h>
#include "PiServer.h"

void PiServer::begin(){
  if(_listening)
    return;
  struct sockaddr_in server;
  sockfd = socket(AF_INET , SOCK_STREAM | SOCK_NONBLOCK, 0);
  if (sockfd < 0)
    return;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(_port);
  if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    return;
  if(listen(sockfd , _max_clients) < 0)
    return;
  _listening = true;
}
void PiServer::end(){
  close(sockfd);
  _listening = false;
}
void PiServer::onClient(PiServerHandler cb){
  _cb = cb;
}
void PiServer::handle(){
  struct sockaddr_in _client;
  int c = sizeof(struct sockaddr_in);
  int client_sock = accept4(sockfd, (struct sockaddr *)&_client, (socklen_t*)&c, SOCK_NONBLOCK);
  if (client_sock < 0)
    return;
  PiClient client(client_sock);
  client.setNoDelay(true);
  if(_cb)
    _cb(client);
  else
    client.stop();
}
