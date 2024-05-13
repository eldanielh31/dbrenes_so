#include "arduino_lib.h"
#include <stdio.h>
#include <stddef.h>

int main() {
    int fd = arduino_open();
    if (fd < 0) {
        return 1;
    }

    char buf[256];
    int bytes_read = arduino_read(fd, buf, sizeof(buf));
    if (bytes_read < 0) {
        perror("Error al leer del dispositivo");
        return 1;
    }

    printf("Leído del Arduino: %s\n", buf);

    arduino_close(fd);
    return 0;
}
