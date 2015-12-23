#ifndef LINUX_CONSOLE_H
#define LINUX_CONSOLE_H

#include "Stream.h"

class LinuxConsole : public Stream{
  protected:
    
  public:
    inline LinuxConsole(){}
    void begin();
    void end();
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
};

extern LinuxConsole Console;

#endif
