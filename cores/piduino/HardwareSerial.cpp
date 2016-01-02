/*
  HardwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2015 Hristo Gochkov.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/**
 * Notes:
 * Issue #23:
 * We desire that the Serial output be able to be redirected to the Console.  When a Serial device is created
 * we check the value of an environment variable called ARDUINO_SERIAL_TO_CONSOLE.  If its value is
 * either "1" or "true" then we redirect output to the Console.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

#include "HardwareSerial.h"

#define UART_BUFFER_SIZE 1024

volatile char uart_rx_buffer[UART_BUFFER_SIZE];
volatile char uart_tx_buffer[UART_BUFFER_SIZE];
volatile int uart_rx_buffer_index = 0;
volatile int uart_tx_buffer_index = 0;
volatile uint8_t uart_thread_running = 0;

//called by the interrupt check thread
void uart_check_fifos(){
  if((UART0CR & _BV(UART0EN)) != 0){
    while((UART0FR & _BV(UART0RXFE)) == 0 && uart_rx_buffer_index < UART_BUFFER_SIZE)
      uart_rx_buffer[uart_rx_buffer_index++] = UART0DR;
    while(uart_tx_buffer_index && (UART0FR & _BV(UART0TXFF)) == 0) {
      uint8_t data = uart_tx_buffer[0];
      uart_tx_buffer_index--;
      int i;
      for(i = 0; i < uart_tx_buffer_index; i++) uart_tx_buffer[i] = uart_tx_buffer[i+1];
      UART0DR = data;
    }
  }
}

void HardwareSerial::begin(uint32_t baud){
  // See if the serial line is being re-mapped to the console.  If it is, we set a flag called
  // isConsole to be true as an indicator to further Serial calls of our previous determination.
  char *ARDUINO_SERIAL_TO_CONSOLE = getenv("ARDUINO_SERIAL_TO_CONSOLE");
  if (ARDUINO_SERIAL_TO_CONSOLE != NULL) {
    if (strcmp(ARDUINO_SERIAL_TO_CONSOLE, "1") == 0 || strcmp(ARDUINO_SERIAL_TO_CONSOLE, "true") == 0) {
      isConsole = true;
      return;
    }
  }

  uint32_t divider = 12000000/baud;
  pinMode(14, GPF0); // TXD
  pinMode(15, GPF0); // RXD
  UART0IBRD = divider >> 6;
  UART0FBRD = divider & 0x3F;
  UART0LCRH = (UART0WLEN_8BIT << UART0WLEN) | _BV(UART0FEN);
  UART0CR   = _BV(UART0EN) | _BV(UART0TXE) | _BV(UART0RXE);
}

void HardwareSerial::end(){
  if (isConsole) {
    return;
  }
  UART0LCRH = 0;
  UART0CR = 0;
  pinMode(14, GPFI); // TXD
  pinMode(15, GPFI); // RXD
}

int HardwareSerial::available(void){
  if (isConsole) {
    return Console.available();
  }
  return uart_rx_buffer_index;
}

int HardwareSerial::peek(void){
  if (isConsole) {
    return Console.peek();
  }
  return uart_rx_buffer[0];
}

int HardwareSerial::read(void){
  if (isConsole) {
    return Console.read();
  }
  uint8_t data = uart_rx_buffer[0];
  uart_rx_buffer_index--;
  int i;
  for(i = 0; i < uart_rx_buffer_index; i++) uart_rx_buffer[i] = uart_rx_buffer[i+1];
  return data;
}

void HardwareSerial::flush(){
  if (isConsole) {
    Console.flush();
    return;
  }
  uart_rx_buffer_index = 0;
  uart_tx_buffer_index = 0;
}

size_t HardwareSerial::write(uint8_t c){
  if (isConsole) {
    return Console.write(c);
  }
  if(uart_tx_buffer_index < UART_BUFFER_SIZE)
    uart_tx_buffer[uart_tx_buffer_index++] = c;
  return 1;
}

HardwareSerial Serial;
