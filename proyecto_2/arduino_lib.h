// arduino_lib.h
#include <stddef.h>
#ifndef ARDUINO_LIB_H
#define ARDUINO_LIB_H

int arduino_open(void);
void arduino_close(int fd);
int arduino_read(int fd, char *buf, size_t count);
int arduino_write(int fd, const char *buf, size_t count);

#endif
