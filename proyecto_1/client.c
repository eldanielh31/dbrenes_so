#include "library.h"

struct SharedData *shared_memory;
struct SharedStats *shared_memory_stats;
int fd;
int fds;
int shared_memory_size;
FILE *file;

void sigint_handler();

int main(int argc, char *argv[]) {
    //Validar los modos y cantidad de parametros
    char* mode = NULL;
    if (argc == 3) {
        if ( strcmp(argv[2], "auto") == 0 || strcmp(argv[2], "manual") == 0 ) {
            mode = argv[2];
        }
    }
    if (mode == NULL) {
        printf("Uso: %s <archivo_de_texto> auto|manual\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *archivo = argv[1];

    signal(SIGINT, sigint_handler); //Manejar el ctrl + c
    setup_terminal(); // Configurar la terminal

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

    // Leer el archivo de texto y escribir en memoria compartida
    file = fopen(archivo, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char c;
    while ((c = fgetc(file)) != EOF) {
        //Si es modo manual tiene que esperar un enter
        if( strcmp(mode, "manual") == 0) while (getchar() != 10); 

        // Obtener el tiempo de uso antes de la ejecución
        time_t time0, time1;
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        long start_user = usage.ru_utime.tv_usec;
        long start_kernel = usage.ru_stime.tv_usec;

        // Obtener el tiempo de inicio
        time(&time0);
        // Verificar si hay espacio disponible en la memoria compartida
        sem_wait(&shared_memory_stats[DEFAULT_STRUCT_POS].space_available);
        // Obtener el tiempo de finalización
        time(&time1);

        shared_memory_stats[DEFAULT_STRUCT_POS].blocked_client_time += difftime(time1, time0);

        //Posicion actual de escritura
        int position = shared_memory_stats[DEFAULT_STRUCT_POS].pos_write;

        // Escribir en memoria compartida de manera circular
        shared_memory[position].character = c;
        shared_memory[position].timestamp = time(NULL);
        shared_memory[position].position = position;

        // Convertir el tiempo al formato requerido
        struct tm *time_info = localtime(&shared_memory[position].timestamp);
        char time_str[80];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
        printf("Carácter: %c, Hora: %s, Posición: %d\n", c, time_str, position); // Imprimir caracter, hora y posición

        // Establecer nueva posicion y establecer la nueva posicion en memoria
        position = (position == shared_memory_size - 1) ? 0 : (position + 1);
        shared_memory_stats[DEFAULT_STRUCT_POS].pos_write = position;

        // Levantar un semaforo de espacio ocupado
        sem_post(&shared_memory_stats[DEFAULT_STRUCT_POS].space_unavailable);

        // Obtener el tiempo de uso después de la ejecución
        getrusage(RUSAGE_SELF, &usage);
        long end_user = usage.ru_utime.tv_usec;
        long end_kernel = usage.ru_stime.tv_usec;

        shared_memory_stats[DEFAULT_STRUCT_POS].total_user_time_client += end_user - start_user;
        shared_memory_stats[DEFAULT_STRUCT_POS].total_kernel_time_client += end_kernel - start_kernel;
    }

    //Finalizar programa
    sigint_handler();

    return 0;
}

// Manejador de señales para SIGINT
void sigint_handler() {
    restore_terminal();
    fclose(file);
    // Desmapear espacio de memoria compartida
    munmap(shared_memory, shared_memory_size * sizeof(struct SharedData));
    munmap(shared_memory_stats, sizeof(struct SharedStats));
    close(fd);
    exit(EXIT_SUCCESS);
}


