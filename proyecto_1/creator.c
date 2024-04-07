#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#define SHARED_MEMORY_SIZE 400

int main() {
    int fd;
    char *shared_memory;

    // Crear el espacio de memoria compartida
    fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, SHARED_MEMORY_SIZE);

    // Mapear la memoria compartida
    shared_memory = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Visualización en tiempo real del contenido de la memoria compartida
    while (1) {
        // Mostrar el contenido de la memoria compartida
        printf("Contenido de la memoria compartida: %s\n", shared_memory);
        sleep(1); // Actualizar cada segundo
    }

    // Desmapear y cerrar el espacio de memoria compartida
    munmap(shared_memory, SHARED_MEMORY_SIZE);
    close(fd);
    shm_unlink("/shared_memory");

    return 0;
}
