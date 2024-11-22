#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void receive_file(FILE *file, int socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Riceve dal socket e scrive nel file
    while ((bytes_received = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore di connessione");
        return 1;
    }

    printf("Connessione al server effettuata.\n");

    // Riceve il nome del file
    char filename[256];
    ssize_t name_received = recv(client_fd, filename, sizeof(filename), 0);
    if (name_received <= 0) {
        printf("Il server non ha inviato un file. Chiusura del client.\n");
        close(client_fd);
        return 1;
    }
    printf("Ricevuto nome del file: %s\n", filename);

    // Crea un file con il nome ricevuto
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Errore nella creazione del file");
        close(client_fd);
        return 1;
    }

    // Riceve il contenuto del file
    printf("Ricezione del file in corso...\n");
    receive_file(file, client_fd);

    printf("File '%s' ricevuto con successo.\n", filename);
    fclose(file);
    close(client_fd);
    return 0;
}
