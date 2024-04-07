#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define SHARED_MEMORY_SIZE 400

int main(int argc, char *argv[]) {
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

    // Leer de memoria compartida y reconstruir el archivo original
    FILE *file = fopen("reconstructed.txt", "w");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int position = 0;
    while (1) {
        char c = shared_memory[position++ % SHARED_MEMORY_SIZE];
        if (c == '\0') // Fin del archivo
            break;
        fprintf(file, "%c", c);
    }

    fclose(file);

    // Desmapear espacio de memoria compartida
    munmap(shared_memory, SHARED_MEMORY_SIZE);
    close(fd);

    return 0;
}
