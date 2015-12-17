#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

void console_begin(void);
void console_end(void);
void console_run(void);
void console_attach_signal_handlers(void);
void console_signal_handler(int);

void console_echo(void);
void console_noecho(void);
void console_mode_raw(void);
void console_mode_line(void);

int console_rx_empty(void);

size_t console_tx(uint8_t data);
uint8_t console_rx(void);

uint8_t console_read(void);
uint8_t console_peek(void);
void console_flush(void);
void console_write(uint8_t str);
int console_available(void);


#ifdef __cplusplus
}
#endif

#endif
