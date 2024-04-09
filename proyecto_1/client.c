#include "library.h"

int main(int argc, char *argv[]) {
    //TODO: Manejar modos igual que reconstructor
    if (argc != 2) {
        printf("Uso: %s <archivo_de_texto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *archivo = argv[1];
    struct SharedData *shared_memory;
    struct SharedStats *shared_memory_stats;
    int fd;
    int fds;

    // Crear o abrir el espacio de memoria compartida
    fd = shm_open(SHARED_MEMORY_DATA_NAME, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, SHARED_MEMORY_SIZE * sizeof(struct SharedData));

    fds = shm_open(SHARED_MEMORY_STATS_NAME, O_RDWR | O_CREAT, 0666);
    if (fds == -1) {
        perror("shm_open");
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
    if (shared_memory == MAP_FAILED) {
        perror("mmap stats");
        exit(EXIT_FAILURE);
    }

    // Leer el archivo de texto y escribir en memoria compartida
    FILE *file = fopen(archivo, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char c;
    // TODO: Para poder sincornizar varios clientes la posicion debe ser una varible global
    // lo que quiere decir que hay que meterla en un espacio compartido (stats)
    int position = 0;
    while ((c = fgetc(file)) != EOF) {
        // Verificar si hay espacio disponible en la memoria compartida
        sem_wait(&shared_memory_stats[0].space_available);

        // Esperar a que el semáforo esté libre
        sem_wait(&(shared_memory[position].semaphore));

        // Escribir en memoria compartida de manera circular
        shared_memory[position].character = c;
        shared_memory[position].timestamp = time(NULL);
        shared_memory[position].position = position;
        struct tm *time_info = localtime(&shared_memory[position].timestamp);
        char time_str[80];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
        printf("Carácter: %c, Hora: %s, Posición: %d\n", c, time_str, position); // Imprimir caracter, hora y posición
        // Liberar el semáforo y el espacio ocupado
        sem_post(&(shared_memory[position].semaphore));
        position = (position == SHARED_MEMORY_SIZE - 1) ? 0 : (position + 1);
    }
    fclose(file);

    // Carácter para finalizar
    if (position < SHARED_MEMORY_SIZE) {
        sem_wait(&(shared_memory[position].semaphore));
        shared_memory[position].character = '\0';
        sem_post(&(shared_memory[position].semaphore));
    }

    // Desmapear espacio de memoria compartida
    munmap(shared_memory, SHARED_MEMORY_SIZE * sizeof(struct SharedData));
    close(fd);

    return 0;
}


