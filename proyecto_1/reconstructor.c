#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>

#include "library.h"

int main(int argc, char *argv[]) {
    struct SharedData *shared_memory;
    int fd;

        if (argc != 2) {
        printf("Uso: %s <modo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* mode = argv[1];

    printf("Mode: %s \n", mode);

    // Abrir la memoria compartida
    fd = shm_open(SHARED_MEMORY_DATA_NAME, O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Mapear la memoria compartida
    shared_memory = mmap(NULL, SHARED_MEMORY_SIZE * sizeof(struct SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Crear un archivo para guardar los datos reconstruidos
    FILE *file = fopen("reconstructed.txt", "w");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Leer datos de la memoria compartida y escribirlos en el archivo
    for (int i = 0; i < SHARED_MEMORY_SIZE; ++i) {
        // Esperar a que el semáforo esté libre
        sem_wait(&(shared_memory[i].semaphore));

        // Escribir el carácter en el archivo si no es '\0' (fin del archivo)
        if (shared_memory[i].character != '\0') {
            fprintf(file, "%c", shared_memory[i].character);
        }

        struct tm *time_info = localtime(&shared_memory[i].timestamp);
        char time_str[80];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);

        printf("Carácter: %c, Hora: %s, Posición: %d\n", 
        shared_memory[i].character, 
        time_str, 
        shared_memory[i].position); // Imprimir caracter, hora y posición

        // Liberar el semáforo
        sem_post(&(shared_memory[i].semaphore));
    }

    fclose(file);

    // Limpiar la memoria compartida y liberar recursos
    memset(shared_memory, 0, SHARED_MEMORY_SIZE * sizeof(struct SharedData));
    munmap(shared_memory, SHARED_MEMORY_SIZE * sizeof(struct SharedData));
    close(fd);

    return 0;
}
