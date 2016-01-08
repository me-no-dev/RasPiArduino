#include <PiUdp.h>

uint8_t PiUDP::begin(uint16_t port){
  return 0;
}

void PiUDP::stop(){
  return;
}

int PiUDP::beginPacket(IPAddress ip, uint16_t port){
  return 0;
}

int PiUDP::beginPacket(const char *host, uint16_t port){
  return 0;
}

int PiUDP::endPacket(){
  return 0;
}

size_t PiUDP::write(uint8_t data){
  return 0;
}

size_t PiUDP::write(const uint8_t *buffer, size_t size){
  return 0;
}

int PiUDP::parsePacket(){
  return 0;
}

int PiUDP::available(){
  return 0;
}

int PiUDP::read(){
  return 0;
}

int PiUDP::read(unsigned char* buffer, size_t len){
  return 0;
}

int PiUDP::read(char* buffer, size_t len){
  return 0;
}

int PiUDP::peek(){
  return 0;
}

void PiUDP::flush(){
  return;
}

IPAddress PiUDP::remoteIP(){
  return IPAddress((uint32_t)0);
}

uint16_t PiUDP::remotePort(){
  return 0;
}
