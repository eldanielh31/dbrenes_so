#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <numCharacters>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int numCharacters = atoi(argv[1]);

    // Crear memoria compartida
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(SharedData), IPC_CREAT | 0666);
    SharedData *sharedData = (SharedData *)shmat(shmid, NULL, 0);
    
    // Inicializar semáforos
    sem_init(&sharedData->empty, 1, MAX_CHARACTERS);
    sem_init(&sharedData->full, 1, 0);
    sem_init(&sharedData->mutex, 1, 1);
    
    // Inicializar buffer
    sharedData->in = 0;
    sharedData->out = 0;
    sharedData->count = 0;

    // Visualización en tiempo real
    while (1) {
        // Implementa aquí la visualización en tiempo real del contenido de la memoria compartida
        sleep(1);
    }

    // Liberar memoria compartida
    shmdt(sharedData);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
