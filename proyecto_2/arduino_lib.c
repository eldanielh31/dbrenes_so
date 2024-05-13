// arduino_lib.c
#include "arduino_lib.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define DEVICE_PATH "/dev/arduino_device" // Cambiar si es necesario

int arduino_open(void) {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Error al abrir el dispositivo");
    }
    return fd;
}

void arduino_close(int fd) {
    close(fd);
}

int arduino_read(int fd, char *buf, size_t count) {
    return read(fd, buf, count);
}

int arduino_write(int fd, const char *buf, size_t count) {
    return write(fd, buf, count);
}
