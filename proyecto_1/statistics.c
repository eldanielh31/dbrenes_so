#include "library.h"

struct SharedData *shared_memory;
struct SharedStats *shared_memory_stats;
int fd;
int fds;
int shared_memory_size;

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

        //Asignar el tamanno de memoria compartida
    shared_memory_size = shared_memory_stats[DEFAULT_STRUCT_POS].shared_memory_size;

    // Crear o abrir el espacio de memoria compartida
    fd = shm_open(SHARED_MEMORY_DATA_NAME, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, shared_memory_size * sizeof(struct SharedData));

    // Mapear la memoria compartida
    shared_memory = mmap(NULL, shared_memory_size * sizeof(struct SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    int predicted_value;
    sem_getvalue(&shared_memory_stats[DEFAULT_STRUCT_POS].space_unavailable, &predicted_value);

    if (shared_memory_stats[DEFAULT_STRUCT_POS].existClient == 0 && predicted_value == 0) {
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
    } else {
        printf("Cierre todos los clientes y reconstructores y vuelva a ejecutar.\n");
    }

    


    munmap(shared_memory_stats, sizeof(struct SharedStats));
    close(fd);

    return 0;
}




