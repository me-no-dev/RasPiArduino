#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "bcm2835_registers.h"

#define EXTERNAL_NUM_INTERRUPTS 32
#define NUM_DIGITAL_PINS            32
#define NUM_ANALOG_INPUTS           0
#define analogInputToDigitalPin(p)  (-1)

#define digitalPinHasPWM(p)         ((p) == 12 || (p) == 13 || (p) == 18 || (p) == 19)

static const uint8_t SS1   = 7;
static const uint8_t SS    = 8;
static const uint8_t MOSI  = 10;
static const uint8_t MISO  = 9;
static const uint8_t SCK   = 11;

static const uint8_t SDA   = 2;
static const uint8_t SCL   = 3;

static const uint8_t RX    = 15;
static const uint8_t TX    = 14;

static const uint8_t PWM00 = 12;
static const uint8_t PWM01 = 13;
static const uint8_t PWM10 = 18;
static const uint8_t PWM11 = 19;

#define digitalPinToInterrupt(p)  (p)

#define SERIAL_PORT_MONITOR   Serial
#define SERIAL_PORT_HARDWARE  Serial

#endif
