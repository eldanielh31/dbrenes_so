#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include "../driver/arduino_lib.h"

#define TAG 0
#define MAX_WORD_LEN 100
#define MAX_WORDS 1000

// Estructura para almacenar la palabra y su frecuencia
typedef struct {
    char word[MAX_WORD_LEN];
    int count;
} WordCount;

// Función para comparar dos WordCount por su conteo (descendente)
int compare(const void *a, const void *b) {
    return ((WordCount *)b)->count - ((WordCount *)a)->count;
}

// Función para convertir una cadena a minúsculas
void to_lowercase(char *str) {
    for (; *str; ++str) *str = tolower(*str);
}

// Función para contar las palabras en un texto
void count_words(char *text, WordCount *word_counts, int *num_words) {
    char *token = strtok(text, " \t\n\r");
    int found;
    while (token != NULL) {
        to_lowercase(token);  // Convertir la palabra a minúsculas
        found = 0;
        for (int i = 0; i < *num_words; i++) {
            if (strcmp(word_counts[i].word, token) == 0) {
                word_counts[i].count++;
                found = 1;
                break;
            }
        }
        if (!found && *num_words < MAX_WORDS) {
            strcpy(word_counts[*num_words].word, token);
            word_counts[*num_words].count = 1;
            (*num_words)++;
        }
        token = strtok(NULL, " \t\n\r");
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    FILE *file;
    long file_size;
    char *buffer = NULL;
    WordCount local_word_counts[MAX_WORDS] = {{{0}}};
    int local_num_words = 0;

    if (rank == 0) {
        // Leer el archivo de texto
        file = fopen("text.txt", "r");
        if (file == NULL) {
            fprintf(stderr, "No se pudo abrir el archivo text.txt\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Obtener el tamaño del archivo
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Asignar buffer para el contenido del archivo
        buffer = malloc(file_size + 1);
        if (buffer == NULL) {
            fprintf(stderr, "No se pudo asignar memoria para el buffer\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Leer el contenido del archivo en el buffer
        fread(buffer, 1, file_size, file);
        buffer[file_size] = '\0';
        fclose(file);
    }

    // Broadcast del tamaño del archivo
    MPI_Bcast(&file_size, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    // Asignar buffer local para cada proceso
    int local_size = file_size / size;
    char *local_buffer = malloc(local_size + 1);

    // Scatter del contenido del archivo a cada proceso
    MPI_Scatter(buffer, local_size, MPI_CHAR, local_buffer, local_size, MPI_CHAR, 0, MPI_COMM_WORLD);
    local_buffer[local_size] = '\0';

    // Contar las palabras localmente
    count_words(local_buffer, local_word_counts, &local_num_words);

    // Recopilar el número de palabras de cada proceso
    int *recv_counts = NULL;
    if (rank == 0) {
        recv_counts = malloc(size * sizeof(int));
    }
    MPI_Gather(&local_num_words, 1, MPI_INT, recv_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Recopilar los resultados locales en el proceso 0
    WordCount *global_word_counts = NULL;
    if (rank == 0) {
        global_word_counts = malloc(size * MAX_WORDS * sizeof(WordCount));
    }
    MPI_Gather(local_word_counts, MAX_WORDS * sizeof(WordCount), MPI_BYTE,
               global_word_counts, MAX_WORDS * sizeof(WordCount), MPI_BYTE,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Combinar los resultados globales
        int total_word_capacity = size * MAX_WORDS;
        WordCount *final_counts = malloc(total_word_capacity * sizeof(WordCount));
        int final_num_words = 0;

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < recv_counts[i]; j++) {
                if (global_word_counts[i * MAX_WORDS + j].count == 0) {
                    continue;
                }
                int found = 0;
                for (int k = 0; k < final_num_words; k++) {
                    if (strcmp(final_counts[k].word, global_word_counts[i * MAX_WORDS + j].word) == 0) {
                        final_counts[k].count += global_word_counts[i * MAX_WORDS + j].count;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    strcpy(final_counts[final_num_words].word, global_word_counts[i * MAX_WORDS + j].word);
                    final_counts[final_num_words].count = global_word_counts[i * MAX_WORDS + j].count;
                    final_num_words++;
                }
            }
        }

        // Ordenar las palabras por frecuencia
        qsort(final_counts, final_num_words, sizeof(WordCount), compare);

        // Imprimir la palabra más repetida
        if (final_num_words > 0) {
            printf("La palabra más repetida es: %s (frecuencia: %d)\n", final_counts[0].word, final_counts[0].count);

            if (rank == 0) {

                int result = send_to_arduino(final_counts[0].word);
                if (result < 0) {
                    printf("Failed to send message to Arduino\n");
                } else {
                    printf("Message sent successfully\n");
                }
            }

        } else {
            printf("No se encontraron palabras.\n");
        }

        // Liberar memoria
        free(recv_counts);
        free(global_word_counts);
        free(final_counts);
    }

    // Liberar la memoria asignada
    if (rank == 0) {
        free(buffer);
    }
    free(local_buffer);

    MPI_Finalize();
    return 0;
}
