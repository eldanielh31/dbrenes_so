#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <semaphore.h> // Agregar la biblioteca de semáforos

#define SHARED_MEMORY_SIZE 100
#define SEMAPHORE_NAME "/shared_semaphore" // Nombre del semáforo compartido

struct SharedData {
    char character;
    time_t timestamp;
    int position;
    sem_t semaphore; 
};

int main(int argc, char *argv[]) {
    struct SharedData *shared_memory;
    int fd;

    if (argc != 2) {
        printf("Uso: %s <modo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* mode = argv[1];

    printf("Mode: %s \n", mode);

    // Crear o abrir el semáforo
    sem_t *sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Abrir el espacio de memoria compartida
    fd = shm_open("/shared_memory", O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Mapear la memoria compartida
    shared_memory = mmap(NULL, SHARED_MEMORY_SIZE * sizeof(struct SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Esperar a que el semáforo esté libre
    sem_wait(sem);

    // Leer de memoria compartida y reconstruir el archivo original
    FILE *file = fopen("reconstructed.txt", "w");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int position = 0;
    while (1) {
        char c = shared_memory[position % SHARED_MEMORY_SIZE].character;

        struct tm *time_info = localtime(&shared_memory[position % SHARED_MEMORY_SIZE].timestamp);
        char time_str[80];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
        
        int pos = shared_memory[position % SHARED_MEMORY_SIZE].position;
        
        if (c == '\0' || position == SHARED_MEMORY_SIZE) {// Fin del archivo
            break;
        }
                    
        fprintf(file, "%c", c);       
        printf("Carácter: %c, Hora: %s, Posición: %d\n", c, time_str, pos); // Imprimir caracter, hora y posición
        
        position++;
    }

    fclose(file);

    // Liberar el semáforo
    sem_post(sem);

    // Desmapear espacio de memoria compartida
    memset(shared_memory, 0, SHARED_MEMORY_SIZE * sizeof(struct SharedData));
    close(fd);

    // Cerrar el semáforo
    sem_close(sem);

    return 0;
}
