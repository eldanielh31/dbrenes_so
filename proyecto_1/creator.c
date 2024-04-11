#include "library.h"

void sigint_handler();

int fd;
int fds;
int shared_memory_size;
struct SharedData *shared_memory;
struct SharedStats *shared_memory_stats;

int main(int argc, char *argv[]) {
    // Establecer el manejador de señales para SIGINT
    signal(SIGINT, sigint_handler);
    if (argc != 2){
        printf("Uso: %s <tamanno del buffer>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Crear el espacio de memoria compartida
    fds = shm_open(SHARED_MEMORY_STATS_NAME, O_CREAT | O_RDWR, 0666);
    if (fds == -1) {
        perror("shm_open_stats");
        exit(EXIT_FAILURE);
    }
    ftruncate(fds, sizeof(struct SharedStats));
    // Mapear la memoria compartida
    shared_memory_stats = mmap(NULL, sizeof(struct SharedStats), PROT_READ | PROT_WRITE, MAP_SHARED, fds, 0);
    if (shared_memory_stats == MAP_FAILED) {
        perror("mmap stats");
        exit(EXIT_FAILURE);
    }

    // TODO: Manejar la cantidad de espacio de memoria compartida
    shared_memory_size = atoi(argv[1]);
    shared_memory_stats[DEFAULT_STRUCT_POS].shared_memory_size = shared_memory_size;
    shared_memory_stats[DEFAULT_STRUCT_POS].pos_read = 0;
    shared_memory_stats[DEFAULT_STRUCT_POS].pos_write = 0;

    // Crear el espacio de memoria compartida
    fd = shm_open(SHARED_MEMORY_DATA_NAME, O_CREAT | O_RDWR, 0666);
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

    // Inicializar los semáforos
    sem_init(&shared_memory_stats[DEFAULT_STRUCT_POS].space_available, 1, shared_memory_size); // Inicializa el semáforo con valor shared_memory_size (todos los espacios disponibles)
    sem_init(&shared_memory_stats[DEFAULT_STRUCT_POS].space_unavailable, 1, 0);

    // Mantener la lectura de la memoria compartida en tiempo real
    while (1) {
        // Esperar a que se presione Enter
        printf("Presiona Enter para actualizar la memoria compartida...");
        while (getchar() != '\n');

        // Leer el contenido de todas las posiciones de la memoria compartida
        for (int i = 0; i < shared_memory_size; i++) {
            struct tm *time_info = localtime(&shared_memory[i].timestamp);
            char time_str[80];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
            printf("Posición %d: Carácter: %c, Hora: %s\n", shared_memory[i].position, shared_memory[i].character, time_str);
        }
    }

    return 0;
}

// Manejador de señales para SIGINT
void sigint_handler() {
    // Desmapear y cerrar el espacio de memoria compartida
    munmap(shared_memory, shared_memory_size * sizeof(struct SharedData));
    munmap(shared_memory_stats, sizeof(struct SharedStats));
    close(fd); close(fds);
    shm_unlink(SHARED_MEMORY_DATA_NAME);
    shm_unlink(SHARED_MEMORY_STATS_NAME);
    exit(EXIT_SUCCESS);
}