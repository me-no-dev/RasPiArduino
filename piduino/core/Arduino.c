#define ARDUINO_MAIN

#include "Arduino.h"
#include "bcm2835_registers.h"
#include "pins_arduino.h"
#include "console.h"

// Forward declarations of static functions
static void dumpRegisters();

static pthread_mutex_t thread_mutexes[4];

int start_thread(void *(*fn)(void *), void * arg){
  	pthread_t myThread ;
  	int fd = pthread_create(&myThread, NULL, fn, arg) ;
  	pthread_detach(myThread);
  	return fd;
}

int create_thread(void *(*fn)(void *)){
	return start_thread(fn, NULL);
}

void lock_thread(int index){
	pthread_mutex_lock(&thread_mutexes[index]);
}

void unlock_thread(int index){
  	pthread_mutex_unlock(&thread_mutexes[index]);
}

int elevate_prio(const int pri){
	struct sched_param sched ;
	memset (&sched, 0, sizeof(sched)) ;
	if (pri > sched_get_priority_max(SCHED_RR)) sched.sched_priority = sched_get_priority_max(SCHED_RR);
	else sched.sched_priority = pri ;
	return sched_setscheduler(0, SCHED_RR, &sched) ;
}

void nap(uint32_t m){
	usleep(m);
}

void pinMode(uint8_t pin, uint8_t mode){
	GPFSEL(pin) &= ~(0x07 << GPFSELB(pin));//clear gpio function
	GPFSEL(pin) |= ((mode & 0x07) << GPFSELB(pin));//set function to pin
	if((mode & 0x07) == 0){
		mode &= 0x30;
		mode >>= 4;
		GPPUD = mode;
		halt(10);
		if(pin < 32) GPPUDCLK0 = _BV(pin);
		else if(pin < 46) GPPUDCLK1 = _BV(pin - 32);
		halt(10);
		GPPUD = 0;
		if(pin < 32) GPPUDCLK0 = 0;
		else if(pin < 46) GPPUDCLK1 = 0;
	}
}

void digitalWrite(uint8_t pin, uint8_t val){
	if(pin < 32){
		if(val) GPSET0 = _BV(pin);
		else GPCLR0 = _BV(pin);
	} else if(pin < 46){
		if(val) GPSET1 = _BV(pin - 32);
		else GPCLR1 = _BV(pin - 32);
	}
}

int digitalRead(uint8_t pin){
	if(pin < 32){
		return (GPLEV0 & _BV(pin)) != 0;
	} else if(pin > 45) return 0;
	return (GPLEV1 & _BV(pin - 32)) != 0;
}

void analogReference(uint8_t mode){}

int analogRead(uint8_t pin){
	return 0;
}

void analogWriteInit(){
	GPPCTL = GPSRC_OSC + GPCPASS;//stop clock
	while(GPPCTL & _BV(GPBUSY));//wait if running
	GPPDIV = (PWM_DEVIDER << GPDIVI) | GPCPASS;//set divider
	GPPCTL = (1 << GPENAB) | GPSRC_OSC | GPCPASS;//set ctrl

	PWMCTL = _BV(PWMMSEN1) | _BV(PWMMSEN2);
	PWMRNG1 = PWM_RANGE+1;
	PWMRNG2 = PWM_RANGE+1;
}

void analogWrite(uint8_t p, uint8_t v){
	if(p == 18 || p == 19){
		pinMode(p, GPF5);
	} else if(p == 12 || p == 13 || p == 40 || p == 41 || p == 45){
		pinMode(p, GPF0);
	}
	if(p == 12 || p == 18 || p == 40){
		PWMCTL |= _BV(PWMPWEN1);
		PWMDAT1 = v % (PWM_RANGE+1);
	} else if(p == 13 || p == 19 || p == 41 || p == 45){
		PWMCTL |= _BV(PWMPWEN2);
		PWMDAT2 = v % (PWM_RANGE+1);
	}
}


typedef struct {
	uint8_t mode;
	void (*fn)(void);
} isr_handler_t;

static isr_handler_t isr_handlers[64];

static volatile uint64_t isr_reg = 0;

void *isr_executor_task(void *isr_num){
	//elevate_prio(55);
	isr_handler_t *handler = &isr_handlers[(int)isr_num];
	handler->fn();
	pthread_exit(NULL);
    return 0;
}

static volatile uint64_t isr_freg = 0;
static volatile uint64_t isr_rreg = 0;

void isr_check(){
	if(isr_freg != 0){
		uint64_t isrfst = GPFEN0;
		isrfst |= (uint64_t)GPFEN1 << 32;
		if(isrfst != isr_freg){
			GPFEN0 = isr_freg;
			GPFEN1 = isr_freg >> 32;
			int i = 0;
			uint32_t changedfbits = isrfst ^ isr_freg;
			while(changedfbits){
				while(!(changedfbits & _BV(i))) i++;
				changedfbits &= ~_BV(i);
				isr_handler_t *handler = &isr_handlers[i];
				if((handler->mode == FALLING || handler->mode == CHANGE) && handler->fn) {
					start_thread(isr_executor_task, (void *)i);
				}
			}
		}
	}
	if(isr_rreg != 0){
		uint64_t isrrst = GPREN0;
		isrrst |= (uint64_t)GPREN1 << 32;
		if(isrrst != isr_rreg){
			GPREN0 = isr_rreg;
			GPREN1 = isr_rreg >> 32;
			int i = 0;
			uint32_t changedrbits = isrrst ^ isr_rreg;
			while(changedrbits){
				while(!(changedrbits & _BV(i))) i++;
				changedrbits &= ~_BV(i);
				isr_handler_t *handler = &isr_handlers[i];
				if((handler->mode == RISING || handler->mode == CHANGE) && handler->fn) {
					start_thread(isr_executor_task, (void *)i);
				}
			}
		}
	}
}

void attachInterrupt(uint8_t pin, void (*userFunc)(void), int mode) {
	if(pin < 46) {
		isr_handler_t *handler = &isr_handlers[pin];
		handler->mode = mode;
		handler->fn = userFunc;
		if(mode == FALLING || mode == CHANGE){
			isr_freg |= _BV(pin);
			if(pin < 32) GPFEN0 = isr_freg & 0xFFFFFFFF;
			else GPFEN1 = isr_freg >> 32;
		}
		if(mode == RISING|| mode == CHANGE){
			isr_rreg |= _BV(pin);
			if(pin < 32) GPREN0 = isr_rreg & 0xFFFFFFFF;
			else GPREN1 = isr_rreg >> 32;
		}
	}
}

void detachInterrupt(uint8_t pin) {
	if(pin < 46) {
		isr_handler_t *handler = &isr_handlers[pin];
		
		if(handler->mode == FALLING || handler->mode == CHANGE){
			isr_freg &= ~_BV(pin);
			if(pin < 32){
				GPFEN0 = isr_freg & 0xFFFFFFFF;
			} else {
				GPREN1 = isr_freg >> 32;
			}
		}
		if(handler->mode == RISING|| handler->mode == CHANGE){
			isr_rreg &= ~_BV(pin);
			if(pin < 32){
				GPREN0 = isr_rreg & 0xFFFFFFFF;
			} else {
				GPREN1 = isr_rreg >> 32;
			}
		}   
		handler->mode = 0;
		handler->fn = 0;
	}
}

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout){
  pinMode(pin, INPUT);
  uint32_t start = micros();
  while(digitalRead(pin) == state && (micros() - start) < timeout);
  while(digitalRead(pin) != state && (micros() - start) < timeout);
  start = micros();
  while(digitalRead(pin) == state && (micros() - start) < timeout);
  return micros() - start;
}

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
	uint8_t value = 0;
	uint8_t i;

	for (i = 0; i < 8; ++i) {
		digitalWrite(clockPin, HIGH);
		if (bitOrder == LSBFIRST)
			value |= digitalRead(dataPin) << i;
		else
			value |= digitalRead(dataPin) << (7 - i);
		digitalWrite(clockPin, LOW);
	}
	return value;
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val){
	uint8_t i;

	for (i = 0; i < 8; i++)  {
		if (bitOrder == LSBFIRST)
			digitalWrite(dataPin, !!(val & (1 << i)));
		else	
			digitalWrite(dataPin, !!(val & (1 << (7 - i))));
			
		digitalWrite(clockPin, HIGH);
		digitalWrite(clockPin, LOW);		
	}
}

uint32_t getBoardRev(){
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
    printf("Error opening /proc/cmdline\n");
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

void init_pins(){
  uint32_t revision = getBoardRev();
  if(revision == 0xa01041) revision = 0x10;//pi2 has B+ pinout
  if(revision == 0xa21041) revision = 0x13;//pi2 has B+ pinout
	uint32_t pinmask = rpi_model_pinmasks[revision];
	if(!pinmask){
		printf("Pinmask was not found for revision 0x%08X\n", revision);
		return;
	}
	int i;
	for(i=0;i<32;i++){
		if((pinmask & _BV(i))){
			pinMode(i,INPUT);
		}
	}
}

void uninit(){
  init_pins();
	unmap_registers();
}

int init(){
  dumpRegisters();

 	if(map_registers((getBoardRev() == 0xa01041 || getBoardRev() == 0xa21041)?0x1F000000:0)) return 1;
	init_pins();
	srand(time(NULL));
	analogWriteInit();
	return 0;
}

/**
 * Dump registers.
 * Dump the register base addresses to stdout.
 */
static void dumpRegisters() {
  printf("boardRev = 0x%x\n", getBoardRev());
  printf("BCM2835_BASE       = 0x%x\n", BCM2835_BASE);
  printf("BCM2835_ST_BASE    = 0x%x\n", BCM2835_ST_BASE);
  printf("BCM2835_IRQ_BASE   = 0x%x\n", BCM2835_IRQ_BASE);
  printf("BCM2835_PM_BASE    = 0x%x\n", BCM2835_PM_BASE);
  printf("BCM2835_CM_BASE    = 0x%x\n", BCM2835_CM_BASE);
  printf("BCM2835_GPIO_BASE  = 0x%x\n", BCM2835_GPIO_BASE);
  printf("BCM2835_UART0_BASE = 0x%x\n", BCM2835_UART0_BASE);
  printf("BCM2835_PCM_BASE   = 0x%x\n", BCM2835_PCM_BASE);
  printf("BCM2835_SPI0_BASE  = 0x%x\n", BCM2835_SPI0_BASE);
  printf("BCM2835_BSC0_BASE  = 0x%x\n", BCM2835_BSC0_BASE);
  printf("BCM2835_PWM_BASE   = 0x%x\n", BCM2835_PWM_BASE);
  printf("BCM2835_BSCS_BASE  = 0x%x\n", BCM2835_BSCS_BASE);
  printf("BCM2835_BSC1_BASE  = 0x%x\n", BCM2835_BSC1_BASE);
}
