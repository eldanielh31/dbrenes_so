#ifndef LIBRARY_H
#define LIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include <termios.h>
#include <sys/resource.h>
#include <time.h>

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
    int existClient;

    int total_char;

    double blocked_client_time;
    double blocked_reconstruct_time;

    long total_user_time_client;
    long total_kernel_time_client;
    long total_user_time_reconstructor;
    long total_kernel_time_reconstructor;
};

// Función para restaurar el modo original de la terminal
void restore_terminal() {
    struct termios new_settings;
    tcgetattr(0, &new_settings);
    new_settings.c_lflag |= ICANON; // Habilitar el modo canónico
    new_settings.c_lflag |= ECHO;   // Mostrar los caracteres ingresados
    tcsetattr(0, TCSANOW, &new_settings);
}

// Función para configurar el modo de la terminal
void setup_terminal() {
    struct termios new_settings;
    tcgetattr(0, &new_settings);
    new_settings.c_lflag &= ~ICANON; // Deshabilitar el modo canónico
    new_settings.c_lflag &= ~ECHO;   // No mostrar los caracteres ingresados
    tcsetattr(0, TCSANOW, &new_settings);
}

// Constantes necesarias
#define SHARED_MEMORY_DATA_NAME "/shared_memory"
#define SHARED_MEMORY_STATS_NAME "/shared_memory_stats"
#define DEFAULT_STRUCT_POS 0

#define SEMAPHORE_NAME_PREFIX "/shared_semaphore_" // Prefijo para los nombres de los semáforos
#define MAX_SEMAPHORE_NAME_LENGTH 50 // Longitud máxima del nombre del semáforo


#endif
