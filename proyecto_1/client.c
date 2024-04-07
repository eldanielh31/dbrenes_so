#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

// #include "creator.h"

#define SHARED_MEMORY_SIZE 100

struct SharedData {
    char character;
    time_t timestamp;
    int position;
};

 
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <archivo_de_texto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // int SHARED_MEMORY_SIZE = atoi(getenv("SHARED_MEMORY_SIZE"));
    
    char *archivo = argv[1];
    struct SharedData *shared_memory;
    int fd;

    // Abrir el espacio de memoria compartida
    fd = shm_open("/shared_memory", O_RDWR, 0666);
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
        if (position < SHARED_MEMORY_SIZE){
            shared_memory[position % SHARED_MEMORY_SIZE].character = c;
            shared_memory[position % SHARED_MEMORY_SIZE].timestamp = time(NULL);
            shared_memory[position % SHARED_MEMORY_SIZE].position = position;
            struct tm *time_info = localtime(&shared_memory[position % SHARED_MEMORY_SIZE].timestamp);
            char time_str[80];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
            printf("Carácter: %c, Hora: %s, Posición: %d\n", c, time_str, position); // Imprimir caracter, hora y posición
        }
        position++;
    }

    fclose(file);

    //Caracter para finalizar
    if (position < SHARED_MEMORY_SIZE) {
        shared_memory[position % SHARED_MEMORY_SIZE].character = '\0';
    }
    //Desmapear espacio de memoria compartida
    munmap(shared_memory, SHARED_MEMORY_SIZE * sizeof(struct SharedData));
    close(fd);

    return 0;
}

