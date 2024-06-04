#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define KEY 0xAA // Simple XOR key for encryption
#define BUFFER_SIZE 1024

void encrypt_decrypt(char *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= KEY;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return -1;
    }

    char *filename = argv[1];
    int sock = 0;
    struct sockaddr_in serv_addr;
    FILE *file;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    // Read the file
    file = fopen(filename, "rb");
    if (!file) {
        perror("Could not open file");
        return -1;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Send file size to server
    if (send(sock, &file_size, sizeof(file_size), 0) != sizeof(file_size)) {
        perror("Send file size error");
        fclose(file);
        close(sock);
        return -1;
    }

    // Send file content in chunks
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        encrypt_decrypt(buffer, bytes_read);
        if (send(sock, buffer, bytes_read, 0) < 0) {
            perror("Send error");
            fclose(file);
            close(sock);
            return -1;
        }
    }

    fclose(file);
    close(sock);

    printf("File sent successfully\n");

    return 0;
}
