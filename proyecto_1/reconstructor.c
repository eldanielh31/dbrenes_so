#include "library.h"

struct SharedData *shared_memory;
struct SharedStats *shared_memory_stats;
int fd;
int fds;
int shared_memory_size;

int main(int argc, char *argv[]) {
    // TODO: Puede que el modo tenga un modo defaul, lo que quiere decir que si no pone nada tiene uno default
    if (argc != 2) {
        printf("Uso: %s <modo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* mode = argv[1];

    printf("Mode: %s \n", mode);

    // Crear el espacio de memoria para stats
    fds = shm_open(SHARED_MEMORY_STATS_NAME, O_RDWR | O_CREAT, 0666);
    if (fds == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(fds, sizeof(struct SharedStats));

    //Mapear la memoria compartida
    shared_memory_stats = mmap(NULL, sizeof(struct SharedStats), PROT_READ | PROT_WRITE, MAP_SHARED, fds, 0);
    if (shared_memory == MAP_FAILED) {
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

    // Crear un archivo para guardar los datos reconstruidos
    FILE *file = fopen("reconstructed.txt", "w");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int sem_value;
    // Obtener el valor actual del semáforo
    if (sem_getvalue(&(shared_memory_stats[DEFAULT_STRUCT_POS].space_unavailable), &sem_value) == -1) {
        perror("sem_getvalue");
        exit(EXIT_FAILURE);
    }
    // Leer datos de la memoria compartida y escribirlos en el archivo
    while (sem_value != 0) {
        //Posicion actual de escritura
        int position = shared_memory_stats[DEFAULT_STRUCT_POS].pos_read;

        // Escribir caracter en archivo de texto
        fprintf(file, "%c", shared_memory[position].character);
        
        /* 
        struct tm *time_info = localtime(&shared_memory[position].timestamp);
        char time_str[80];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);

        printf("Carácter: %c, Hora: %s, Posición: %d\n", 
        shared_memory[position].character, 
        time_str, 
        shared_memory[position].position); */

        // Imprimir caracter
        printf("%c", shared_memory[position].character);

        
        // Actualizar posicion 
        position = (position == shared_memory_size - 1) ? 0 : (position + 1);
        shared_memory_stats[DEFAULT_STRUCT_POS].pos_read = position;

        // Liberar el semáforo
        sem_post(&(shared_memory_stats[DEFAULT_STRUCT_POS].space_available));
        // Ocupar un espacio en semaforo
        sem_wait(&(shared_memory_stats[DEFAULT_STRUCT_POS].space_unavailable));
        //volver a leer los espacios disponibles
        sem_getvalue(&(shared_memory_stats[DEFAULT_STRUCT_POS].space_unavailable), &sem_value);

    }

    fclose(file);

    munmap(shared_memory, shared_memory_size * sizeof(struct SharedData));
    close(fd);

    return 0;
}
