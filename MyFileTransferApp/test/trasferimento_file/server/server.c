#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void send_file(FILE *file, int socket) {
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Legge dal file e invia al socket
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(socket, buffer, bytes_read, 0);
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);

    printf("Server in attesa di connessioni...\n");
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    printf("Connessione accettata.\n");

    // Nome del file da inviare
    char *filename = "prova.txt"; 
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Errore nell'apertura del file");
        close(client_fd);
        close(server_fd);
        return 1;
    }

    // Chiedi conferma all'utente
    char conferma;
    printf("Sei sicuro di voler inviare il file '%s'? (y/n): ", filename);
    scanf(" %c", &conferma);

    if (conferma != 'y' && conferma != 'Y') {
        printf("Invio annullato.\n");
        fclose(file);
        close(client_fd);
        close(server_fd);
        return 0;
    }

    // Invia il nome del file
    send(client_fd, filename, strlen(filename) + 1, 0); // Invia il nome del file (incluso il terminatore '\0')

    // Invia il contenuto del file
    send_file(file, client_fd);

    printf("File '%s' inviato con successo.\n", filename);
    fclose(file);
    close(client_fd);
    close(server_fd);
    return 0;
}
