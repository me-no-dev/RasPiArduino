#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int _server_socket;
int _client_socket;
struct sockaddr_in _server;
struct sockaddr_in _client;
uint8_t _server_running;

#define MONITOR_RX_LEN 1024
char _rx_buffer[MONITOR_RX_LEN];
size_t _rx_fill;

int idemonitor_begin(){
  _server_running = 0;
  _rx_fill = 0;
  _client_socket = -1;
  _server.sin_family = AF_INET;
  _server.sin_addr.s_addr = INADDR_ANY;
  _server.sin_port = htons(6571);
  
  _server_socket = socket(AF_INET , SOCK_STREAM | SOCK_NONBLOCK , 0);
  if (_server_socket == -1 || bind(_server_socket,(struct sockaddr *)&_server , sizeof(_server)) < 0)
    return 1;
  
  listen(_server_socket , 3);
  _server_running = 1;
  return 0;
}

int idemonitor_write(char * data, size_t len){
  if(!_server_running || _client_socket < 0) return 0;
  return send(_client_socket, data, len, MSG_DONTWAIT);
}

void idemonitor_run(){
  if(!_server_running)
    return;
  int c = sizeof(struct sockaddr_in);
  if(_client_socket < 0){
    _client_socket = accept(_server_socket, (struct sockaddr *)&_client, (socklen_t*)&c);
    if(_client_socket > 0){
      struct sockaddr_in *s = (struct sockaddr_in *)&_client;
      if(s->sin_addr.s_addr != 16777343){//not 127.0.0.1
        shutdown(_client_socket, SHUT_RDWR);
        _client_socket = -1;
      }
    }
  } else {
    struct sockaddr_in _cc;
    int _c = accept(_server_socket, (struct sockaddr *)&_cc, (socklen_t*)&c);
    if(c)
      shutdown(_c, SHUT_RDWR);
    //we should get only what we have available to receive
    size_t space_available = MONITOR_RX_LEN - _rx_fill;
    if(space_available){
      int res = recv(_client_socket, _rx_buffer + _rx_fill, space_available, MSG_DONTWAIT);
      if(res > 0){
        _rx_fill += res;
        if(_rx_fill < MONITOR_RX_LEN)
          _rx_buffer[_rx_fill] = '\0';
      } else if(res == 0){
        shutdown(_client_socket, SHUT_RDWR);
        _client_socket = -1;
      }
    }
  }
}

size_t idemonitor_available(){
  return _rx_fill;
}

size_t idemonitor_read(char *buf, size_t len){
  len = (len > _rx_fill)?_rx_fill:len;
  memcpy(buf, _rx_buffer, len);
  memmove(_rx_buffer, _rx_buffer+len, len);
  return len;
}

int idemonitor_read_char(){
  if(!_rx_fill)
    return -1;
  char r = (char)_rx_buffer[0];
  memmove(_rx_buffer, _rx_buffer+1, 1);
  return r;
}

int idemonitor_peek(){
  if(!_rx_fill)
    return -1;
  return (char)_rx_buffer[0];
}

int idemonitor_flush(){
  _rx_fill = 0;
}
