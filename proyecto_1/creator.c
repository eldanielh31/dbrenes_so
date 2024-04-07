#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define SHARED_MEMORY_SIZE 100

struct SharedData {
    char character;
    time_t timestamp;
    int position;
};

int fd;
struct SharedData *shared_memory;

// Manejador de señales para SIGINT
void sigint_handler() {
    // Desmapear y cerrar el espacio de memoria compartida
    munmap(shared_memory, SHARED_MEMORY_SIZE * sizeof(struct SharedData));
    close(fd);
    shm_unlink("/shared_memory");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    // Establecer el manejador de señales para SIGINT
    signal(SIGINT, sigint_handler);

    // Crear el espacio de memoria compartida
    fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, SHARED_MEMORY_SIZE * sizeof(struct SharedData));

    // Mapear la memoria compartida
    shared_memory = mmap(NULL, SHARED_MEMORY_SIZE * sizeof(struct SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Mantener la lectura de la memoria compartida en tiempo real
    while (1) {
        // Esperar a que se presione Enter
        printf("Presiona Enter para actualizar la memoria compartida...");
        while (getchar() != '\n');

        // Leer el contenido de todas las posiciones de la memoria compartida
        for (int i = 0; i < SHARED_MEMORY_SIZE; i++) {
            struct tm *time_info = localtime(&shared_memory[i].timestamp);
            char time_str[80];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
            printf("Posición %d: Carácter: %c, Hora: %s\n", shared_memory[i].position, shared_memory[i].character, time_str);
        }
    }

    return 0;
}
