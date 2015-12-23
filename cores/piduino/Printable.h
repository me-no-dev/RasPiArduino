#ifndef Printable_h
#define Printable_h

class Print;

class Printable{
  public:
    virtual size_t printTo(Print& p) const = 0;
};

#endif

