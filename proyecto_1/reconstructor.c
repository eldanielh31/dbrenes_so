#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <time.h>

#define MAX_CHARACTERS 100

typedef struct {
    char buffer[MAX_CHARACTERS];
    int in;
    int out;
    int count;
    sem_t empty;
    sem_t full;
    sem_t mutex;
} SharedData;

int main() {
    // Abrir memoria compartida
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(SharedData), 0666);
    SharedData *sharedData = (SharedData *)shmat(shmid, NULL, 0);

    // Reconstruir archivo original
    while (1) {
        sem_wait(&sharedData->full);
        sem_wait(&sharedData->mutex);

        if (sharedData->count == -1) {
            sem_post(&sharedData->mutex);
            sem_post(&sharedData->full);
            break;
        }

        // Leer de memoria compartida y procesar
        char character = sharedData->buffer[sharedData->out];
        sharedData->out = (sharedData->out + 1) % MAX_CHARACTERS;
        sharedData->count--;

        sem_post(&sharedData->mutex);
        sem_post(&sharedData->empty);

        // Muestra el caracter, la hora y la posición
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("Caracter: %c, Hora: %s, Posición: %d\n", character, asctime(timeinfo), sharedData->out);
    }

    // Desconectar memoria compartida
    shmdt(sharedData);

    return 0;
}
