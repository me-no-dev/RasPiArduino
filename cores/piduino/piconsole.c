/*
  console.c - Linux STDIN/STDOUT library for Raspberry Pi
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

#include "Arduino.h"
#include "Console.h"

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <pthread.h>
#include <signal.h>

volatile char console_rx_buffer[1024];
volatile int console_rx_buffer_index = 0;

volatile uint8_t console_thread_running = 0;

void console_begin(){
  console_thread_running = 1;
  console_noecho();
  console_mode_raw();
}

void console_end(){
  console_thread_running = 0;
  console_echo();
  console_mode_line();
}

void console_run(void){
  if(console_thread_running && !console_rx_empty() && console_rx_buffer_index < 1024){
    console_rx_buffer[console_rx_buffer_index++] = console_rx();
  }
}

void console_signal_handler(int signo){
  if (signo == SIGINT || signo == SIGTERM || signo == SIGUSR1){
    console_end();
    request_sketch_terminate();
  }
}

void console_attach_signal_handlers(){
  signal(SIGINT, console_signal_handler);
  signal(SIGTERM, console_signal_handler);
  signal(SIGUSR1, console_signal_handler);
}

uint8_t console_rx(){
  char ch;
  read(STDIN_FILENO, &ch, 1);
  fflush(stdin);
  if(ch == 3) console_signal_handler(SIGINT);//Ctrl+C received and sig handlers are not initialized
  return ch;
}

size_t console_tx(uint8_t data){
  putc(data, stdout);
  fflush(stdout);
  return 1;
}

uint8_t console_read(){
  uint8_t data = console_rx_buffer[0];
  console_rx_buffer_index--;
  int i;
  for(i = 0; i < console_rx_buffer_index; i++) console_rx_buffer[i] = console_rx_buffer[i+1];
  return data;
}

uint8_t console_peek(){
  uint8_t data = console_rx_buffer[0];
  return data;
}

void console_write(uint8_t data){
  console_tx(data);
}

int console_available(){
    return console_rx_buffer_index;
}

void console_flush(void){
  fflush(stdin);
}

void console_echo(){
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag |= ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void console_noecho(){
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void console_mode_raw(){
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag &= ~ICANON;
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void console_mode_line(){
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag |= ICANON;
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int console_rx_empty(){
  fd_set fds;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  return select(STDIN_FILENO+1, &fds, NULL, NULL, &tv) == 0;
}


