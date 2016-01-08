/*
  Client.h - Client class for Raspberry Pi
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
#include "PiClient.h"

int sock_connect(int fd, struct sockaddr *addr, size_t len){
  return connect(fd, addr, len);
}

int sock_write(int fd, void *data, size_t len){
  return write(fd, data, len);
}

int sock_read(int fd, void *data, size_t len){
  return read(fd, data, len);
}

PiClient::~PiClient(){
  //if(sockfd) close(sockfd);
}
int PiClient::connect(IPAddress ip, uint16_t port){
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    return 0;
  }
  uint32_t ip_addr = ip;
  struct sockaddr_in serveraddr;
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((const void *)(&ip_addr), (void *)&serveraddr.sin_addr.s_addr, 4);
  serveraddr.sin_port = htons(port);
  if (sock_connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0){
    return 0;
  }
  _connected = true;
  return 1;
}
int PiClient::connect(const char *host, uint16_t port){
  struct hostent *server;
  server = gethostbyname(host);
  if (server == NULL)
    return 0;
  return connect(IPAddress((const uint8_t *)(server->h_addr)), port);
}
int PiClient::setSocketOption(int option, char* value, size_t len){
  return setsockopt(sockfd, SOL_SOCKET, option, value, len);
}
int PiClient::setOption(int option, int *value){
  return setsockopt(sockfd, IPPROTO_TCP, option, (char *)value, sizeof(int));
}
int PiClient::getOption(int option, int *value){
  size_t size = sizeof(int);
  return getsockopt(sockfd, IPPROTO_TCP, option, (char *)value, &size);
}
int PiClient::setTimeout(uint32_t seconds){
  struct timeval tv;
  tv.tv_sec = seconds;
  tv.tv_usec = 0;
  if(setSocketOption(SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
    return -1;
  return setSocketOption(SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));
}
int PiClient::setNoDelay(bool nodelay){
  int flag = nodelay;
  return setOption(TCP_NODELAY, &flag);
}
bool PiClient::getNoDelay(){
  int flag = 0;
  getOption(TCP_NODELAY, &flag);
  return flag;
}
size_t PiClient::write(uint8_t data){
  return write(&data, 1);
}
size_t PiClient::write(const uint8_t *buf, size_t size){
  if(!_connected) return 0;
  int res = send(sockfd, (void*)buf, size, MSG_DONTWAIT);
  if(res < 0){
    _connected = false;
    res = 0;
  }
  return res;
}
int PiClient::read(){
  uint8_t data = 0;
  int res = read(&data, 1);
  if(res < 0){
    return res;
  }
  return data;
}
int PiClient::read(uint8_t *buf, size_t size){
  if(!_connected) return -1;
  int res = sock_read(sockfd, 0, 0);
  if(size && res == 0 && available()){
     res = recv(sockfd, buf, size, MSG_DONTWAIT);
  }
  if(res < 0){
    _connected = false;
  }
  return res;
}
int PiClient::available(){
  int count;
  ioctl(sockfd, FIONREAD, &count);
  return count;
}
void PiClient::stop(){
  close(sockfd);
}
uint8_t PiClient::connected(){
  read(0,0);
  return _connected;
}
