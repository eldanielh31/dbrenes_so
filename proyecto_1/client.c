#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define SHARED_MEMORY_SIZE 400

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <archivo_de_texto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *archivo = argv[1];
    char *shared_memory;
    int fd;

    // Abrir el espacio de memoria compartida
    fd = shm_open("/shared_memory", O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Mapear la memoria compartida
    shared_memory = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Leer el archivo de texto y escribir en memoria compartida
    FILE *file = fopen(archivo, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char c;
    int position = 0;
    while ((c = fgetc(file)) != EOF) {
        // Escribir en memoria compartida de manera circular
        shared_memory[position++ % SHARED_MEMORY_SIZE] = c;
        printf("Carácter: %c, Hora: %ld, Posición: %d\n", c, time(NULL), position); // Imprimir caracter, hora y posición
    }

    fclose(file);

    // Marcar finalización
    // Implementación pendiente

    // Desmapear espacio de memoria compartida
    munmap(shared_memory, SHARED_MEMORY_SIZE);
    close(fd);

    return 0;
}
