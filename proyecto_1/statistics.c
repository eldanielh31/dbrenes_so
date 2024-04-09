#include "library.h"

#define SHARED_MEMORY_STATS_SIZE sizeof(struct Statistics)

struct Statistics {
    int client_blocked_time;
    int reconstructor_blocked_time;
    int transferred_characters;
    int remaining_buffer_characters;
    int total_memory_used;
    int total_user_kernel_time_client;
    int total_user_kernel_time_reconstructor;
};

int main() {
    struct Statistics *stats;
    int fd;

    // Crear espacio de memoria compartida para estadísticas
    fd = shm_open("/shared_memory_stats", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, SHARED_MEMORY_STATS_SIZE);

    // Mapear la memoria compartida
    stats = mmap(NULL, SHARED_MEMORY_STATS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (stats == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Inicializar las estadísticas
    stats->client_blocked_time = 10;
    stats->reconstructor_blocked_time = 20;
    stats->transferred_characters = 1000;
    stats->remaining_buffer_characters = 500;
    stats->total_memory_used = 2048;
    stats->total_user_kernel_time_client = 500;
    stats->total_user_kernel_time_reconstructor = 700;

    // Mostrar las estadísticas
    printf("Tiempo bloqueado del cliente: %d\n", stats->client_blocked_time);
    printf("Tiempo bloqueado del Re-constructor: %d\n", stats->reconstructor_blocked_time);
    printf("Caracteres transferidos: %d\n", stats->transferred_characters);
    printf("Caracteres restantes en el búfer: %d\n", stats->remaining_buffer_characters);
    printf("Espacio total de memoria utilizado: %d\n", stats->total_memory_used);
    printf("Tiempo total en modo usuario y kernel del cliente: %d\n", stats->total_user_kernel_time_client);
    printf("Tiempo total en modo usuario y kernel del Re-constructor: %d\n", stats->total_user_kernel_time_reconstructor);

    // Desmapear y cerrar espacio de memoria compartida
    munmap(stats, SHARED_MEMORY_STATS_SIZE);
    close(fd);
    shm_unlink("/shared_memory_stats");

    return 0;
}
