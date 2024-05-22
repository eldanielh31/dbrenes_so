#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0
#define BUFFER_SIZE 10

int main(int argc, char** argv) {
    int rank, size;
    int buffer[BUFFER_SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        printf("Se necesitan al menos 2 procesos para este programa.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == MASTER) {
        // Rellena el buffer con datos en el proceso maestro
        for (int i = 0; i < BUFFER_SIZE; ++i) {
            buffer[i] = i;
        }

        // Envía el buffer a todos los esclavos
        for (int dest = 1; dest < size; ++dest) {
            MPI_Send(buffer, BUFFER_SIZE, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
        printf("Maestro envió el buffer.\n");
    } else {
        // Recibe el buffer en cada proceso esclavo
        MPI_Recv(buffer, BUFFER_SIZE, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Esclavo %d recibió el buffer: ", rank);
        for (int i = 0; i < BUFFER_SIZE; ++i) {
            printf("%d ", buffer[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
