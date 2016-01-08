#ifndef pi_udp_h
#define pi_udp_h

#include <Arduino.h>
#include <Udp.h>

class PiUDP : public UDP {

public:
  uint8_t begin(uint16_t);
  void stop();
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
