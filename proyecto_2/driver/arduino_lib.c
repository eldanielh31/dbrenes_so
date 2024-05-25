// arduino_lib.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "arduino_lib.h"

#define DEVICE_PATH "/dev/arduino0"

int send_to_arduino(const char *message)
{
    int fd, result;

    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open the device");
        return fd;
    }

    result = write(fd, message, strlen(message));
    if (result < 0) {
        perror("Failed to write the message to the device");
    }

    close(fd);
    return result;
}