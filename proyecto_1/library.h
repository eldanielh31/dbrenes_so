#ifndef LIBRARY_H
#define LIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <semaphore.h>

// Estructura para los datos compartidos
struct SharedData {
    char character;
    time_t timestamp;
    int position;
};

// Estructura para las estadísticas compartidas
struct SharedStats {
    sem_t space_available;
    sem_t space_unavailable;
    int pos_read;
    int pos_write;
    int shared_memory_size;
};

// Constantes necesarias
#define SHARED_MEMORY_DATA_NAME "/shared_memory"
#define SHARED_MEMORY_STATS_NAME "/shared_memory_stats"
#define DEFAULT_STRUCT_POS 0

#define SEMAPHORE_NAME_PREFIX "/shared_semaphore_" // Prefijo para los nombres de los semáforos
#define MAX_SEMAPHORE_NAME_LENGTH 50 // Longitud máxima del nombre del semáforo


#endif
