#include "library.h"

struct SharedData *shared_memory;
struct SharedStats *shared_memory_stats;
int fd;
int fds;
int shared_memory_size;
FILE *file;

void sigint_handler();

int main(int argc, char *argv[]) {
    char* mode = NULL;
    if (argc == 2) {
        if ( strcmp(argv[1], "auto") == 0 || strcmp(argv[1], "manual") == 0 ) {
            mode = argv[1];
        }
    }

    if (mode == NULL) {
        printf("Uso: %s auto|manual\n", argv[0]);
        exit(EXIT_FAILURE);
    }

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

    // Crear un archivo para guardar los datos reconstruidos
    file = fopen("reconstructed.txt", "w");
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
    while (1) {
        if( shared_memory_stats[DEFAULT_STRUCT_POS].existClient == 0 && sem_value == 0) {
            break;
        }
        //Espera poder continuar si es modo manual
        else if( strcmp(mode, "manual") == 0) while (getchar() != 10);

        // Obtener el tiempo de uso antes de la ejecución
        time_t time0, time1;
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        long start_user = usage.ru_utime.tv_usec;
        long start_kernel = usage.ru_stime.tv_usec;

        // Obtener el tiempo de inicio
        time(&time0);
        // Ocupar un espacio en semaforo
        sem_wait(&(shared_memory_stats[DEFAULT_STRUCT_POS].space_unavailable));
        // Obtener el tiempo de inicio
        time(&time1);

        shared_memory_stats[DEFAULT_STRUCT_POS].blocked_reconstruct_time += difftime(time1, time0);


        //Posicion actual de escritura
        int position = shared_memory_stats[DEFAULT_STRUCT_POS].pos_read;

        // Escribir caracter en archivo de texto
        fprintf(file, "%c", shared_memory[position].character);

        // Imprimir caracter
        printf("%c", shared_memory[position].character);
        fflush(stdout); // Vacía el búfer de salida para mostrar los caracteres en la consola
        
        // Actualizar posicion 
        position = (position == shared_memory_size - 1) ? 0 : (position + 1);
        shared_memory_stats[DEFAULT_STRUCT_POS].pos_read = position;

        shared_memory_stats[DEFAULT_STRUCT_POS].total_char++;

        // Liberar el semáforo
        sem_post(&(shared_memory_stats[DEFAULT_STRUCT_POS].space_available));

        // Obtener el tiempo de uso después de la ejecución
        getrusage(RUSAGE_SELF, &usage);
        long end_user = usage.ru_utime.tv_usec;
        long end_kernel = usage.ru_stime.tv_usec;

        shared_memory_stats[DEFAULT_STRUCT_POS].total_user_time_reconstructor += end_user - start_user;
        shared_memory_stats[DEFAULT_STRUCT_POS].total_kernel_time_reconstructor += end_kernel - start_kernel;

        if( strcmp(mode, "auto") == 0) usleep(250000); // Esperar un cuarto de segundo (250000 microsegundos)

        //volver a leer los espacios disponibles
        sem_getvalue(&(shared_memory_stats[DEFAULT_STRUCT_POS].space_unavailable), &sem_value);
    }

    //Finalizar programa
    sigint_handler();

    return 0;
}

// Manejador de señales para SIGINT
void sigint_handler() {
    printf("%c", '\n');
    restore_terminal();
    fclose(file);
    // Desmapear espacio de memoria compartida
    munmap(shared_memory, shared_memory_size * sizeof(struct SharedData));
    munmap(shared_memory_stats, sizeof(struct SharedStats));
    close(fd);
    exit(EXIT_SUCCESS);
}
