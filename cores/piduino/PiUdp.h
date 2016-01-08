#ifndef pi_udp_h
#define pi_udp_h

#include <Arduino.h>
#include <Udp.h>
#include <cbuf.h>

class PiUDP : public UDP {
private:
  int udp_server;
  IPAddress remote_ip;
  uint16_t remote_port;
  char buffer[1500];
  size_t buffer_len;
  cbuf * rx_buffer;
public:
  PiUDP():udp_server(-1), remote_port(0), buffer_len(0), rx_buffer(0){}
  ~PiUDP(){
    stop();
    if(rx_buffer)
      delete rx_buffer;
  }
  uint8_t begin(uint16_t);
  void stop();
  int beginPacket();
  int beginPacket(IPAddress ip, uint16_t port);
  int beginPacket(const char *host, uint16_t port);
  int endPacket();
  size_t write(uint8_t);
  size_t write(const uint8_t *buffer, size_t size);
  int parsePacket();
  int available();
  int read();
  int read(unsigned char* buffer, size_t len);
  int read(char* buffer, size_t len);
  int peek();
  void flush();
  IPAddress remoteIP();
  uint16_t remotePort();
};

#endif
