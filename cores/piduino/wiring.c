/*
  Arduino.c -  Registers initialization for Raspberry Pi
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

#define ARDUINO_MAIN

#include "Arduino.h"

static inline void _halt(uint32_t microseconds){
  uint32_t start = STCLO;
  uint32_t compare = start + microseconds;
  if(compare < start)
    while(STCLO - compare);
  else
    while(STCLO < compare);
}

void sleepMicroseconds(uint32_t m){
  usleep(m);
}

void delay(uint32_t m){
  while(m--) usleep(1000);
}

void delayMicroseconds(uint32_t m){
  if(m < 1000)
    return _halt(m);
  usleep(m);
}

void analogReference(uint8_t mode){}
int analogRead(uint8_t pin){ return 0; }

/*
 * CORE INIT AND CLOSE
 *
 * */

volatile uint32_t        *bcmreg_st;
volatile uint32_t        *bcmreg_irq;
volatile uint32_t        *bcmreg_pm;
volatile uint32_t        *bcmreg_cm;
volatile uint32_t        *bcmreg_gpio;
volatile uint32_t        *bcmreg_uart0;
volatile uint32_t        *bcmreg_pcm;
volatile uint32_t        *bcmreg_spi0;
volatile uint32_t        *bcmreg_bsc0;
volatile uint32_t        *bcmreg_pwm;
volatile uint32_t        *bcmreg_bscs;
volatile uint32_t        *bcmreg_aux;
volatile uint32_t        *bcmreg_bsc1;

static void *mapreg(int fd, off_t off){
    return mmap(NULL, 4096, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, off);
}

static void unmapreg(void **pmem){
    if (*pmem == MAP_FAILED) return;
    munmap(*pmem, 4096);
    *pmem = MAP_FAILED;
}

static void unmap_registers(void){
    unmapreg((void**) &bcmreg_st);
    unmapreg((void**) &bcmreg_irq);
    unmapreg((void**) &bcmreg_pm);
    unmapreg((void**) &bcmreg_cm);
    unmapreg((void**) &bcmreg_gpio);
    unmapreg((void**) &bcmreg_uart0);
    unmapreg((void**) &bcmreg_pcm);
    unmapreg((void**) &bcmreg_spi0);
    unmapreg((void**) &bcmreg_bsc0);
    unmapreg((void**) &bcmreg_pwm);
    unmapreg((void**) &bcmreg_bscs);
    unmapreg((void**) &bcmreg_aux);
    unmapreg((void**) &bcmreg_bsc1);
}

static int map_registers(uint32_t reg_offset){
    int memfd = -1;
    if ((memfd = open("/dev/mem", O_RDWR | O_SYNC) ) < 0) goto exit;

    bcmreg_st = (volatile uint32_t *)mapreg(memfd, BCM2835_ST_BASE + reg_offset);
    if(bcmreg_st == MAP_FAILED) goto exit;

    bcmreg_irq = (volatile uint32_t *)mapreg(memfd, BCM2835_IRQ_BASE + reg_offset);
    if(bcmreg_irq == MAP_FAILED) goto exit;

    bcmreg_pm = (volatile uint32_t *)mapreg(memfd, BCM2835_PM_BASE + reg_offset);
    if(bcmreg_pm == MAP_FAILED) goto exit;

    bcmreg_cm = (volatile uint32_t *)mapreg(memfd, BCM2835_CM_BASE + reg_offset);
    if(bcmreg_cm == MAP_FAILED) goto exit;

    bcmreg_gpio = (volatile uint32_t *)mapreg(memfd, BCM2835_GPIO_BASE + reg_offset);
    if(bcmreg_gpio == MAP_FAILED) goto exit;

    bcmreg_uart0 = (volatile uint32_t *)mapreg(memfd, BCM2835_UART0_BASE + reg_offset);
    if(bcmreg_uart0 == MAP_FAILED) goto exit;

    bcmreg_pcm = (volatile uint32_t *)mapreg(memfd, BCM2835_PCM_BASE + reg_offset);
    if(bcmreg_pcm == MAP_FAILED) goto exit;

    bcmreg_spi0 = (volatile uint32_t *)mapreg(memfd, BCM2835_SPI0_BASE + reg_offset);
    if(bcmreg_spi0 == MAP_FAILED) goto exit;

    bcmreg_bsc0 = (volatile uint32_t *)mapreg(memfd, BCM2835_BSC0_BASE + reg_offset);
    if(bcmreg_bsc0 == MAP_FAILED) goto exit;

    bcmreg_pwm = (volatile uint32_t *)mapreg(memfd, BCM2835_PWM_BASE + reg_offset);
    if(bcmreg_pwm == MAP_FAILED) goto exit;

    bcmreg_bscs = (volatile uint32_t *)mapreg(memfd, BCM2835_BSCS_BASE + reg_offset);
    if(bcmreg_bscs == MAP_FAILED) goto exit;

    bcmreg_aux = (volatile uint32_t *)mapreg(memfd, BCM2835_AUX_BASE + reg_offset);
    if(bcmreg_aux == MAP_FAILED) goto exit;

    bcmreg_bsc1 = (volatile uint32_t *)mapreg(memfd, BCM2835_BSC1_BASE + reg_offset);
    if(bcmreg_bsc1 == MAP_FAILED) goto exit;

    return 0;

exit:
    fprintf(stderr, "init_registers failed: %s\n", strerror(errno));
    if (memfd >= 0){
      close(memfd);
      unmap_registers();
    }
    return 1;
}

static uint32_t getBoardRev(){
  FILE* f;
  char buf[1024];
  char dest[32];
  int n = 0;
  int d = 0;
  int i;
  uint32_t revision = 0;

  char window[5];
  int wi = 0;
  
  // We must clear the window before use.  Can't assume it is zero.
  memset(window, 0, sizeof(window));

  if ((f = fopen("/proc/cmdline", "r"))) {
    n = fread(buf, 1, 1023, f);
    fclose(f);
  } else {
    fprintf(stderr, "Error opening /proc/cmdline\n");
  }
  
  for (i = 0; i < n; ++i) {
    char c = buf[i];
    if (strcmp(window, "rev=") == 0) {
      while (buf[i] != ' ' && buf[i] != 0) {
        dest[d++] = buf[i++];
      }
      dest[d++] = 0;
      revision = (int)strtol(dest, NULL, 0);
      return revision;
    }
    if (wi < 4) {
      window[wi++] = c;
      window[wi] = 0;
    } else {
      int win;
      for (win = 1; win < 4; ++win) window[win-1] = window[win];
      window[3] = c;
    }
  }
  return revision;
}

static uint32_t _board_revision = 0;

static void init_pins(){
  uint32_t pinmask = rpi_model_pinmasks[_board_revision];
  int i;
  for(i=0;i<32;i++){
    if((pinmask & (1 << i))){
      pinMode(i,INPUT);
    }
  }
}

void uninit(){
  init_pins();
  unmap_registers();
}

int init(){
  uint32_t revision = getBoardRev();
  uint32_t offset = ((revision == 0xa01041 || revision == 0xa21041)?0x1F000000:0);
  if(revision == 0xa01041)
    revision = 0x10;//pi2 has B+ pinout
  else if(revision == 0xa21041 || revision == 0x00900092)
    revision = 0x13;//pi2 has B+ pinout
  if(revision >= PINMASKS_LEN || !rpi_model_pinmasks[revision]){
    fprintf(stderr, "UNKNOWN_REVISION: 0x%08X\n", revision);
    return 1;
  }
  if(map_registers(offset))
    return 1;
  
  _board_revision = revision;
  init_pins();
  srand(time(NULL));
  return 0;
}
