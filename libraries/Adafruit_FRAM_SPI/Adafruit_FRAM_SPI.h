/**************************************************************************/
/*! 
    @file     Adafruit_FRAM_SPI.h
    @author   KTOWN (Adafruit Industries)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2013, Adafruit Industries
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#ifndef _ADAFRUIT_FRAM_SPI_H_
#define _ADAFRUIT_FRAM_SPI_H_

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include <SPI.h>

typedef enum opcodes_e
{
  OPCODE_WREN   = 0b0110,     /* Write Enable Latch */
  OPCODE_WRDI   = 0b0100,     /* Reset Write Enable Latch */
  OPCODE_RDSR   = 0b0101,     /* Read Status Register */
  OPCODE_WRSR   = 0b0001,     /* Write Status Register */
  OPCODE_READ   = 0b0011,     /* Read Memory */
  OPCODE_WRITE  = 0b0010,     /* Write Memory */
  OPCODE_RDID   = 0b10011111  /* Read Device ID */
} opcodes_t;

class Adafruit_FRAM_SPI {
 public:
  Adafruit_FRAM_SPI(int8_t cs);
  Adafruit_FRAM_SPI(int8_t clk, int8_t miso, int8_t mosi, int8_t cs);

  boolean  begin  (void);
  void     writeEnable (bool enable);
  void     write8 (uint16_t addr, uint8_t value);
  uint8_t  read8  (uint16_t addr);
  void     getDeviceID(uint8_t *manufacturerID, uint16_t *productID);
  uint8_t  getStatusRegister(void);
  void     setStatusRegister(uint8_t value);

 private:
  uint8_t  SPItransfer(uint8_t x);

  boolean _framInitialised;
  int8_t _cs, _clk, _mosi, _miso;
};

#endif
