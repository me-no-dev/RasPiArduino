#ifndef twi_h
#define twi_h

#include <inttypes.h>

#ifdef __cplusplus
extern "C"{
#endif

extern uint8_t TWBR;

uint8_t twi_readFrom(uint8_t, uint8_t*, uint32_t, uint8_t);
uint8_t twi_writeTo(uint8_t, uint8_t*, uint32_t, uint8_t, uint8_t);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

