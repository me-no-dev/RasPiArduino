#ifndef IDE_CONSOLE_H
#define IDE_CONSOLE_H
/*
#include "Stream.h"

class ArduinoConsole : public Stream {
  protected:
    
  public:
    inline ArduinoConsole(){}
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool() { return true; }
    int read(uint8_t *data, size_t len);
    size_t write(uint8_t *data, size_t len);
};

extern ArduinoConsole Monitor;
*/
#endif
