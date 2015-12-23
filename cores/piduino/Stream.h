#ifndef Stream_h
#define Stream_h

#include <inttypes.h>
#include "Print.h"

class Stream : public Print{
  protected:
    unsigned long _timeout;      // number of milliseconds to wait for the next char before aborting timed read
    unsigned long _startMillis;  // used for timeout measurement
    int timedRead();    // private method to read stream with timeout
    int timedPeek();    // private method to peek stream with timeout
    int peekNextDigit(); // returns the next numeric digit in the stream or -1 if timeout

  public:
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int peek() = 0;
    virtual void flush() = 0;

    Stream() {_timeout=1000;_startMillis=0;}

  void setTimeout(unsigned long timeout);

  bool find(char *target);
  bool find(uint8_t *target) { return find ((char *)target); }

  bool find(char *target, size_t length);
  bool find(uint8_t *target, size_t length) { return find ((char *)target, length); }

  bool findUntil(char *target, char *terminator);
  bool findUntil(uint8_t *target, char *terminator) { return findUntil((char *)target, terminator); }

  bool findUntil(char *target, size_t targetLen, char *terminate, size_t termLen);
  bool findUntil(uint8_t *target, size_t targetLen, char *terminate, size_t termLen) {return findUntil((char *)target, targetLen, terminate, termLen); }

  long parseInt();
  float parseFloat();

  size_t readBytes(char *buffer, size_t length);
  size_t readBytes(uint8_t *buffer, size_t length) { return readBytes((char *)buffer, length); }

  size_t readBytesUntil(char terminator, char *buffer, size_t length);
  size_t readBytesUntil(char terminator, uint8_t *buffer, size_t length) { return readBytesUntil(terminator, (char *)buffer, length); }
  String readString();
  String readStringUntil(char terminator);

  protected:
  long parseInt(char skipChar);
  float parseFloat(char skipChar);
};

#endif
