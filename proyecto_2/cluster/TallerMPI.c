#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG 0

double leibniz(int start, int end) {
    double sum = 0.0;
    for (int i = start; i <= end; i++) {
        sum += (i % 2 == 0 ? -1.0 : 1.0) / (2 * i + 1);
    }
    return sum;
}

int main(int argc, char *argv[]) {
    int rank, size;
    double partial_sum, total_sum;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 100000000000; // Número de términos en la serie
    int chunk_size = n / size; // Tamaño de cada porción
    if (rank == 0) {
        for (int dest = 1; dest < size; dest++) {
            int start = dest * chunk_size;
            int end = (dest == size - 1) ? n - 1 : start + chunk_size - 1;
            MPI_Send(&start, 1, MPI_INT, dest, TAG, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT, dest, TAG, MPI_COMM_WORLD);
        }
        partial_sum = leibniz(0, chunk_size - 1); // Calcular la porción del proceso maestro
    } else {
        printf("Hola del Slave\n");
        int start, end;
        MPI_Recv(&start, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        partial_sum = leibniz(start, end);
    }

    MPI_Reduce(&partial_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("La suma de la serie de Leibniz es aproximadamente: %.10f\n", total_sum * 4);
    }

    MPI_Finalize();
    return 0;
}
