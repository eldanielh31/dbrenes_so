#include "library.h"

void sigint_handler();

int fd;
int fds;
struct SharedData *shared_memory;
struct SharedStats *shared_memory_stats;

void initialize_semaphores(struct SharedData *shared_memory);
void destroy_semaphores(struct SharedData *shared_memory);

int main(int argc, char *argv[]) {
    // Establecer el manejador de señales para SIGINT
    signal(SIGINT, sigint_handler);
    if (argc != 2){
        printf("Uso: %s <tamanno del buffer>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // TODO: Manejar la cantidad de espacio de memoria compartida
    // int SHARED_MEMORY_SIZE = atoi(argv[1]);

    // Crear el espacio de memoria compartida
    fd = shm_open(SHARED_MEMORY_DATA_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, SHARED_MEMORY_SIZE * sizeof(struct SharedData));

    // Crear el espacio de memoria compartida
    fds = shm_open(SHARED_MEMORY_STATS_NAME, O_CREAT | O_RDWR, 0666);
    if (fds == -1) {
        perror("shm_open_stats");
        exit(EXIT_FAILURE);
    }
    ftruncate(fds, sizeof(struct SharedStats));

    // Mapear la memoria compartida
    shared_memory = mmap(NULL, SHARED_MEMORY_SIZE * sizeof(struct SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    shared_memory_stats = mmap(NULL, sizeof(struct SharedStats), PROT_READ | PROT_WRITE, MAP_SHARED, fds, 0);
    if (shared_memory_stats == MAP_FAILED) {
        perror("mmap stats");
        exit(EXIT_FAILURE);
    }

    // Inicializar los semáforos
    initialize_semaphores(shared_memory);
    sem_init(&shared_memory_stats[0].space_available, 1, SHARED_MEMORY_SIZE); // Inicializa el semáforo con valor SHARED_MEMORY_SIZE (todos los espacios disponibles)

    // Mantener la lectura de la memoria compartida en tiempo real
    while (1) {
        // Esperar a que se presione Enter
        printf("Presiona Enter para actualizar la memoria compartida...");
        while (getchar() != '\n');

        // Leer el contenido de todas las posiciones de la memoria compartida
        for (int i = 0; i < SHARED_MEMORY_SIZE; i++) {
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
    munmap(shared_memory, SHARED_MEMORY_SIZE * sizeof(struct SharedData));
    munmap(shared_memory_stats, sizeof(struct SharedStats));
    close(fd); close(fds);
    shm_unlink(SHARED_MEMORY_DATA_NAME);
    shm_unlink(SHARED_MEMORY_STATS_NAME);
    // Desinicializar los semáforos
    destroy_semaphores(shared_memory);
    exit(EXIT_SUCCESS);
}

// Función para inicializar los semáforos
void initialize_semaphores(struct SharedData *shared_memory) {
    for (int i = 0; i < SHARED_MEMORY_SIZE; ++i) {
        sem_init(&(shared_memory[i].semaphore), 1, 1); // Inicializa el semáforo con valor 1 (libre)
    }
}

// Función para destruir los semáforos
void destroy_semaphores(struct SharedData *shared_memory) {
    for (int i = 0; i < SHARED_MEMORY_SIZE; ++i) {
        sem_destroy(&(shared_memory[i].semaphore));
    }
}
