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
  uint32_t divider = 12000000/baud;
  pinMode(14, GPF0); // TXD
  pinMode(15, GPF0); // RXD
  UART0IBRD = divider >> 6;
  UART0FBRD = divider & 0x3F;
  UART0LCRH = (UART0WLEN_8BIT << UART0WLEN) | _BV(UART0FEN);
  UART0CR   = _BV(UART0EN) | _BV(UART0TXE) | _BV(UART0RXE);
}

void HardwareSerial::end(){
  UART0LCRH = 0;
  UART0CR = 0;
  pinMode(14, GPFI); // TXD
  pinMode(15, GPFI); // RXD
}

int HardwareSerial::available(void){
  return uart_rx_buffer_index;
}

int HardwareSerial::peek(void){
  return uart_rx_buffer[0];
}

int HardwareSerial::read(void){
  uint8_t data = uart_rx_buffer[0];
  uart_rx_buffer_index--;
  int i;
  for(i = 0; i < uart_rx_buffer_index; i++) uart_rx_buffer[i] = uart_rx_buffer[i+1];
  return data;
}

void HardwareSerial::flush(){
  uart_rx_buffer_index = 0;
  uart_tx_buffer_index = 0;
}

size_t HardwareSerial::write(uint8_t c){
  if(uart_tx_buffer_index < UART_BUFFER_SIZE)
    uart_tx_buffer[uart_tx_buffer_index++] = c;
  return 1;
}

HardwareSerial Serial;



