#include "Arduino.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int _server_socket;
int _client_socket;
struct sockaddr_in _server;
struct sockaddr_in _client;
bool _server_running;

int telnet_begin(){
  _server_running = false;
  _client_socket = -1;
  _server.sin_family = AF_INET;
  _server.sin_addr.s_addr = INADDR_ANY;
  _server.sin_port = htons(6571);
  
  _server_socket = socket(AF_INET , SOCK_STREAM | SOCK_NONBLOCK , 0);
  if (_server_socket == -1 || bind(_server_socket,(struct sockaddr *)&_server , sizeof(_server)) < 0)
    return 1;
  
  listen(_server_socket , 3);
  _server_running = true;
  return 0;
}

int telnet_write(char * data, size_t len){
  if(!_server_running || _client_socket < 0) return 0;
  return send(_client_socket, data, len, MSG_DONTWAIT);
}

void telnet_run(){
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
    if(c){
      shutdown(_c, SHUT_RDWR);
    }
    char buf[1024];
    int len = 1024;
    int res;
    res = recv(_client_socket , buf , len , MSG_DONTWAIT);
    if(res > 0){
      buf[res] = '\0';
      //receive buf, res
    } else if(res == 0){
      shutdown(_client_socket, SHUT_RDWR);
      _client_socket = -1;
    }
  }
}