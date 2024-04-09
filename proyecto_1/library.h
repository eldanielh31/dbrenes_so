#ifndef LIBRARY_H
#define LIBRARY_H

#include <time.h>
#include <semaphore.h>

// Estructura para los datos compartidos
struct SharedData {
    char character;
    time_t timestamp;
    int position;
    sem_t semaphore;
};

// Estructura para las estadísticas compartidas
struct SharedStats {
    sem_t space_available;
};

// Constantes necesarias
#define SHARED_MEMORY_DATA_NAME "/shared_memory"
#define SHARED_MEMORY_STATS_NAME "/shared_memory_stats"
#define SHARED_MEMORY_SIZE 100
#define SEMAPHORE_NAME_PREFIX "/shared_semaphore_" // Prefijo para los nombres de los semáforos
#define MAX_SEMAPHORE_NAME_LENGTH 50 // Longitud máxima del nombre del semáforo


#endif
