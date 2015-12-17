/**************************************************************************/
/*!
    @file     Adafruit_FRAM_SPI.cpp
    @author   KTOWN (Adafruit Industries)
    @license  BSD (see license.txt)

    Driver for the Adafruit SPI FRAM breakout.

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0 - First release
*/
/**************************************************************************/
//#include <avr/pgmspace.h>
//#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

#include "Adafruit_FRAM_SPI.h"

/*========================================================================*/
/*                            CONSTRUCTORS                                */
/*========================================================================*/

/**************************************************************************/
/*!
    Constructor
*/
/**************************************************************************/
Adafruit_FRAM_SPI::Adafruit_FRAM_SPI(int8_t cs) 
{
  _cs = cs;
  _clk = _mosi = _miso = -1;
  _framInitialised = false;
}

Adafruit_FRAM_SPI::Adafruit_FRAM_SPI(int8_t clk, int8_t miso, int8_t mosi, int8_t cs) 
{
  _cs = cs; _clk = clk; _mosi = mosi; _miso = miso;
  _framInitialised = false;
}

/*========================================================================*/
/*                           PUBLIC FUNCTIONS                             */
/*========================================================================*/

/**************************************************************************/
/*!
    Initializes SPI and configures the chip (call this function before
    doing anything else)
*/
/**************************************************************************/
boolean Adafruit_FRAM_SPI::begin(void) 
{
  /* Configure SPI */
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);

  if (_clk == -1) { // hardware SPI!
    SPI.begin();
      
#ifdef __SAM3X8E__
    SPI.setClockDivider (9); // 9.3 MHz
#else
    SPI.setClockDivider (SPI_CLOCK_DIV2); // 8 MHz
#endif
      
    SPI.setDataMode(SPI_MODE0);
  } else {
    pinMode(_clk, OUTPUT);
    pinMode(_mosi, OUTPUT);
    pinMode(_miso, INPUT);
  }

  /* Make sure we're actually connected */
  uint8_t manufID;
  uint16_t prodID;
  getDeviceID(&manufID, &prodID);
  if (manufID != 0x04)
  {
    //Serial.print("Unexpected Manufacturer ID: 0x"); Serial.println(manufID, HEX);
    return false;
  }
  if (prodID != 0x0302)
  {
    //Serial.print("Unexpected Product ID: 0x"); Serial.println(prodID, HEX);
    return false;
  }

  /* Everything seems to be properly initialised and connected */
  _framInitialised = true;

  return true;
}

/**************************************************************************/
/*!
    @brief  Enables or disables writing to the SPI flash
    
    @params[in] enable
                True enables writes, false disables writes
*/
/**************************************************************************/
void Adafruit_FRAM_SPI::writeEnable (bool enable)
{
  digitalWrite(_cs, LOW);
  if (enable)
  {
    SPItransfer(OPCODE_WREN);
  }
  else
  {
    SPItransfer(OPCODE_WRDI);
  }
  digitalWrite(_cs, HIGH);
}

/**************************************************************************/
/*!
    @brief  Writes a byte at the specific FRAM address
    
    @params[in] addr
                The 16-bit address to write to in FRAM memory
    @params[in] i2cAddr
                The 8-bit value to write at framAddr
*/
/**************************************************************************/
void Adafruit_FRAM_SPI::write8 (uint16_t addr, uint8_t value)
{
  digitalWrite(_cs, LOW);
  SPItransfer(OPCODE_WRITE);
  SPItransfer((uint8_t)(addr >> 8));
  SPItransfer((uint8_t)(addr & 0xFF));
  SPItransfer(value);
  /* CS on the rising edge commits the WRITE */
  digitalWrite(_cs, HIGH);
}

/**************************************************************************/
/*!
    @brief  Reads an 8 bit value from the specified FRAM address

    @params[in] addr
                The 16-bit address to read from in FRAM memory

    @returns    The 8-bit value retrieved at framAddr
*/
/**************************************************************************/
uint8_t Adafruit_FRAM_SPI::read8 (uint16_t addr)
{
  digitalWrite(_cs, LOW);
  SPItransfer(OPCODE_READ);
  SPItransfer((uint8_t)(addr >> 8));
  SPItransfer((uint8_t)(addr & 0xFF));
  uint8_t x = SPItransfer(0);
  digitalWrite(_cs, HIGH);
  return x;
}

/**************************************************************************/
/*!
    @brief  Reads the Manufacturer ID and the Product ID from the IC

    @params[out]  manufacturerID
                  The 8-bit manufacturer ID (Fujitsu = 0x04)
    @params[out]  productID
                  The memory density (bytes 15..8) and proprietary
                  Product ID fields (bytes 7..0). Should be 0x0302 for
                  the MB85RS64VPNF-G-JNERE1.
*/
/**************************************************************************/
void Adafruit_FRAM_SPI::getDeviceID(uint8_t *manufacturerID, uint16_t *productID)
{
  uint8_t a[4] = { 0, 0, 0, 0 };
  uint8_t results;

  digitalWrite(_cs, LOW);
  SPItransfer(OPCODE_RDID);
  a[0] = SPItransfer(0);
  a[1] = SPItransfer(0);
  a[2] = SPItransfer(0);
  a[3] = SPItransfer(0);
  digitalWrite(_cs, HIGH);
  
  /* Shift values to separate manuf and prod IDs */
  /* See p.10 of http://www.fujitsu.com/downloads/MICRO/fsa/pdf/products/memory/fram/MB85RS64V-DS501-00015-4v0-E.pdf */
  *manufacturerID = (a[0]);
  *productID = (a[2] << 8) + a[3];
}

/**************************************************************************/
/*!
    @brief  Reads the status register
*/
/**************************************************************************/
uint8_t Adafruit_FRAM_SPI::getStatusRegister(void)
{
  uint8_t reg = 0;
  digitalWrite(_cs, LOW);
  SPItransfer(OPCODE_RDSR);
  reg = SPItransfer(0);
  digitalWrite(_cs, HIGH);
  return reg;
}

/**************************************************************************/
/*!
    @brief  Sets the status register
*/
/**************************************************************************/
void Adafruit_FRAM_SPI::setStatusRegister(uint8_t value)
{
  digitalWrite(_cs, LOW);
  SPItransfer(OPCODE_WRSR);
  SPItransfer(value);
  digitalWrite(_cs, HIGH);
}

uint8_t Adafruit_FRAM_SPI::SPItransfer(uint8_t x) {
  if (_clk == -1) {
    return SPI.transfer(x);
  } else {
    // Serial.println("Software SPI");
    uint8_t reply = 0;
    for (int i=7; i>=0; i--) {
      reply <<= 1;
      digitalWrite(_clk, LOW);
      digitalWrite(_mosi, x & (1<<i));
      digitalWrite(_clk, HIGH);
      if (digitalRead(_miso)) 
	reply |= 1;
    }
    return reply;
  }
}
