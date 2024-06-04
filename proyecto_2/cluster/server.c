#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define KEY 0xAA // Simple XOR key for encryption
#define BUFFER_SIZE 1024

void encrypt_decrypt(char *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= KEY;
    }
}

void handle_connection(int new_socket) {
    char buffer[BUFFER_SIZE];
    FILE *file;
    long file_size;
    ssize_t bytes_received, total_bytes_received = 0;

    // Receive file size
    if (recv(new_socket, &file_size, sizeof(file_size), 0) != sizeof(file_size)) {
        perror("Receive file size error");
        close(new_socket);
        exit(EXIT_FAILURE);
    }

    // Open the file for writing
    file = fopen("text.txt", "wb");
    if (!file) {
        perror("Could not open file to write");
        close(new_socket);
        exit(EXIT_FAILURE);
    }

    // Receive the file content in chunks
    while (total_bytes_received < file_size) {
        bytes_received = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            perror("Receive error");
            fclose(file);
            close(new_socket);
            exit(EXIT_FAILURE);
        }
        encrypt_decrypt(buffer, bytes_received);
        fwrite(buffer, 1, bytes_received, file);
        total_bytes_received += bytes_received;
    }

    fclose(file);
    printf("File received and saved successfully\n");

    // Execute the command "make run"
    system("make run");

    close(new_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accept an incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Create a child process to handle the connection
        if (fork() == 0) {
            // Child process
            close(server_fd);
            handle_connection(new_socket);
            exit(0);
        } else {
            // Parent process
            close(new_socket);
            // Wait for any child processes to prevent zombie processes
            while (waitpid(-1, NULL, WNOHANG) > 0);
        }
    }

    close(server_fd);
    return 0;
}
