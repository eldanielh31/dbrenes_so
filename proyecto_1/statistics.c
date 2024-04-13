#include "library.h"

struct SharedStats *shared_memory_stats;
int fd;
int fds;

void sigint_handler();

int main() {
    // Crear el espacio de memoria para stats
    fds = shm_open(SHARED_MEMORY_STATS_NAME, O_RDWR | O_CREAT, 0666);
    if (fds == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(fds, sizeof(struct SharedStats));

    //Mapear la memoria compartida
    shared_memory_stats = mmap(NULL, sizeof(struct SharedStats), PROT_READ | PROT_WRITE, MAP_SHARED, fds, 0);
    if (shared_memory_stats == MAP_FAILED) {
        perror("mmap stats");
        exit(EXIT_FAILURE);
    }

    // IMPRIMIR ESTATISTICAS
    printf("Total char transferidos: %i, Restantes en buffer: %i\n", 
    shared_memory_stats[DEFAULT_STRUCT_POS].total_char,
    abs(shared_memory_stats[DEFAULT_STRUCT_POS].pos_write - shared_memory_stats[DEFAULT_STRUCT_POS].pos_read));
    printf("Tiempo bloqueado cliente (s): %f\n", shared_memory_stats[DEFAULT_STRUCT_POS].blocked_client_time);
    printf("Tiempo bloqueado reconstructor (s): %f\n", shared_memory_stats[DEFAULT_STRUCT_POS].blocked_reconstruct_time);
    printf("Tiempo kernel cliente (us): %ld\n", shared_memory_stats[DEFAULT_STRUCT_POS].total_kernel_time_client);
    printf("Tiempo usuario cliente (us): %ld\n", shared_memory_stats[DEFAULT_STRUCT_POS].total_user_time_client);
    printf("Tiempo kernel reconstructor (us): %ld\n", shared_memory_stats[DEFAULT_STRUCT_POS].total_kernel_time_reconstructor);
    printf("Tiempo usuario reconstructor (us): %ld\n", shared_memory_stats[DEFAULT_STRUCT_POS].total_user_time_reconstructor);


    munmap(shared_memory_stats, sizeof(struct SharedStats));
    close(fd);

    return 0;
}




