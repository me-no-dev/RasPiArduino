#ifndef _IDE_MONITOR_H
#define _IDE_MONITOR_H

#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

void idemonitor_begin(void);
void idemonitor_run(void);
uint8_t idemonitor_connected();
int idemonitor_write(char * data, size_t len);
size_t idemonitor_available();
size_t idemonitor_read(char *buf, size_t len);
int idemonitor_read_char();
int idemonitor_peek();
void idemonitor_flush();

#ifdef __cplusplus
}
#endif

#endif
