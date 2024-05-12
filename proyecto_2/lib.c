#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUF_SIZE 64
#define DEVICE_FILE "/dev/ttyACM0" // El nombre del archivo del dispositivo puede variar según el sistema

int arduino_open() {
    int fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Error opening Arduino device");
        exit(EXIT_FAILURE);
    }
    return fd;
}

void arduino_close(int fd) {
    close(fd);
}

ssize_t arduino_read(int fd, char *buffer, size_t count) {
    ssize_t bytes_read = read(fd, buffer, count);
    if (bytes_read < 0) {
        perror("Error reading from Arduino device");
        exit(EXIT_FAILURE);
    }
    return bytes_read;
}

ssize_t arduino_write(int fd, const char *buffer, size_t count) {
    ssize_t bytes_written = write(fd, buffer, count);
    if (bytes_written < 0) {
        perror("Error writing to Arduino device");
        exit(EXIT_FAILURE);
    }
    return bytes_written;
}

int main() {
    char data[256]; // Buffer para almacenar los datos ingresados por el usuario

    // Abrir el dispositivo
    int fd = open(DEVICE_FILE, O_WRONLY);
    if (fd == -1) {
        perror("Error al abrir el dispositivo");
        return 1;
    }

    // Bucle principal
    while (1) {
        // Esperar por la entrada del usuario
        printf("Ingrese el dato a escribir en el dispositivo (o escriba 'exit' para salir):\n");
        fgets(data, sizeof(data), stdin);

        // Eliminar el carácter de nueva línea del final de la cadena
        data[strcspn(data, "\n")] = 0;

        // Salir del bucle si el usuario escribe 'exit'
        if (strcmp(data, "exit") == 0) {
            break;
        }

        // Escribir los datos en el dispositivo
        if (write(fd, data, strlen(data)) == -1) {
            perror("Error al escribir en el dispositivo");
            close(fd);
            return 1;
        }

        printf("Datos escritos en el dispositivo exitosamente.\n");
    }

    // Cerrar el dispositivo
    close(fd);

    return 0;
}