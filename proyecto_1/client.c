#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>

#define MAX_CHARACTERS 100
#define BUFFER_SIZE 512

typedef struct {
    char buffer[MAX_CHARACTERS];
    int in;
    int out;
    int count;
    sem_t empty;
    sem_t full;
    sem_t mutex;
} SharedData;

void writeToSharedMemory(char character, SharedData *sharedData) {
    sem_wait(&sharedData->empty);
    sem_wait(&sharedData->mutex);

    // Write to shared memory
    sharedData->buffer[sharedData->in] = character;
    sharedData->in = (sharedData->in + 1) % MAX_CHARACTERS;
    sharedData->count++;

    sem_post(&sharedData->mutex);
    sem_post(&sharedData->full);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];

    // Abrir memoria compartida
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(SharedData), 0666);
    SharedData *sharedData = (SharedData *)shmat(shmid, NULL, 0);

    // Leer archivo y escribir en memoria compartida
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        for (int i = 0; i < strlen(buffer); i++) {
            writeToSharedMemory(buffer[i], sharedData);
            // Muestra el caracter, la hora y la posición
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            printf("Caracter: %c, Hora: %s, Posición: %d\n", buffer[i], asctime(timeinfo), sharedData->in);
        }
    }

    fclose(file);

    // Indicar que no hay más datos
    // (esto puede ser mejorado para tener una señal más robusta)
    sem_wait(&sharedData->mutex);
    sharedData->count = -1;
    sem_post(&sharedData->mutex);
    sem_post(&sharedData->full);

    // Desconectar memoria compartida
    shmdt(sharedData);

    return 0;
}
