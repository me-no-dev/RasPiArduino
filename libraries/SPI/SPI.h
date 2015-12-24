#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <stdio.h>
#include "Arduino.h"

//AVR Arduino compatibility
#define SPI_CLOCK_DIV2 SPI0F2DIV(8000000)
#define SPI_CLOCK_DIV4 SPI0F2DIV(4000000)
#define SPI_CLOCK_DIV8 SPI0F2DIV(1000000)
#define SPI_CLOCK_DIV16 SPI0F2DIV(500000)
#define SPI_CLOCK_DIV32 SPI0F2DIV(250000)
#define SPI_CLOCK_DIV64 SPI0F2DIV(175000)
#define SPI_CLOCK_DIV128 SPI0F2DIV(87500)

#define SPI_MODE0 SPI0MODE0
#define SPI_MODE1 SPI0MODE1
#define SPI_MODE2 SPI0MODE2
#define SPI_MODE3 SPI0MODE3

class SPIClass {
public:
  static void begin();
  static void end();
  static void setDataMode(uint32_t);
  static void setClockDivider(uint32_t);
  static void setClock(uint32_t);
  static uint8_t transfer(uint8_t data);
  //no bit order on the pi
  static void setBitOrder(uint32_t){}
};

extern SPIClass SPI;

#endif
