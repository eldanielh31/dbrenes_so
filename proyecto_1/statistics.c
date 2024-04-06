#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

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

    // Mostrar estadísticas
    // Implementa aquí la lógica para calcular las estadísticas
    
    // Desconectar memoria compartida
    shmdt(sharedData);

    return 0;
}
