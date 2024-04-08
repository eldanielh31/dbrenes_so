#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>

#define SHARED_MEMORY_SIZE 100
#define SEMAPHORE_NAME_PREFIX "/shared_semaphore_" // Prefijo para los nombres de los semáforos
#define MAX_SEMAPHORE_NAME_LENGTH 50 // Longitud máxima del nombre del semáforo

struct SharedData {
    char character;
    time_t timestamp;
    int position;
    sem_t semaphore; // Semáforo para esta dirección en la memoria compartida
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <archivo_de_texto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *archivo = argv[1];
    struct SharedData *shared_memory;
    int fd;

    // Crear o abrir el espacio de memoria compartida
    fd = shm_open("/shared_memory", O_RDWR | O_CREAT, 0666);
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

    // Inicializar los semáforos para cada dirección en la memoria compartida
    for (int i = 0; i < SHARED_MEMORY_SIZE; ++i) {
        char semaphore_name[MAX_SEMAPHORE_NAME_LENGTH];
        snprintf(semaphore_name, MAX_SEMAPHORE_NAME_LENGTH, "%s%d", SEMAPHORE_NAME_PREFIX, i);
        sem_init(&(shared_memory[i].semaphore), 1, 1); // Inicializa el semáforo con valor 1 (libre)
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
        // Esperar a que el semáforo esté libre
        sem_wait(&(shared_memory[position % SHARED_MEMORY_SIZE].semaphore));

        // Escribir en memoria compartida de manera circular
        shared_memory[position % SHARED_MEMORY_SIZE].character = c;
        shared_memory[position % SHARED_MEMORY_SIZE].timestamp = time(NULL);
        shared_memory[position % SHARED_MEMORY_SIZE].position = position;
        struct tm *time_info = localtime(&shared_memory[position % SHARED_MEMORY_SIZE].timestamp);
        char time_str[80];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
        printf("Carácter: %c, Hora: %s, Posición: %d\n", c, time_str, position); // Imprimir caracter, hora y posición
        position++;

        // Liberar el semáforo
        sem_post(&(shared_memory[position % SHARED_MEMORY_SIZE].semaphore));
    }

    fclose(file);

    // Carácter para finalizar
    if (position < SHARED_MEMORY_SIZE) {
        sem_wait(&(shared_memory[position % SHARED_MEMORY_SIZE].semaphore));
        shared_memory[position % SHARED_MEMORY_SIZE].character = '\0';
        sem_post(&(shared_memory[position % SHARED_MEMORY_SIZE].semaphore));
    }

    // Desinicializar los semáforos
    for (int i = 0; i < SHARED_MEMORY_SIZE; ++i) {
        sem_destroy(&(shared_memory[i].semaphore));
    }

    // Desmapear espacio de memoria compartida
    munmap(shared_memory, SHARED_MEMORY_SIZE * sizeof(struct SharedData));
    close(fd);

    return 0;
}
