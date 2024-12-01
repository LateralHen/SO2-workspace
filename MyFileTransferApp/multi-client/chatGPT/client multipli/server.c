#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 12345

#define MAX_CONNECTIONS 10
#define BUFFER_SIZE 1024

// Funzione che gestisce la comunicazione con il client
void *handle_client(void *client_socket) {
    int socket = *((int *)client_socket);
    free(client_socket);  // Libera la memoria dopo aver copiato il valore

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Ricevi un messaggio dal client
    bytes_received = recv(socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("recv failed");
        close(socket);
        return NULL;
    }

    buffer[bytes_received] = '\0';  // Termina la stringa
    printf("Received message: %s\n", buffer);

    // Invia una risposta al client
    const char *response = "Message received successfully";
    send(socket, response, strlen(response), 0);

    // Chiudi la connessione con il client
    close(socket);
    return NULL;
}

int main() {
    int server_socket, *client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t thread_id;

    // Crea un socket TCP
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configura l'indirizzo del server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Associa il socket all'indirizzo
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Ascolta le connessioni in entrata
    if (listen(server_socket, MAX_CONNECTIONS) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Ciclo principale per accettare le connessioni dei client
    while (1) {
        client_socket = malloc(sizeof(int));  // Alloca memoria per il socket del client
        if (client_socket == NULL) {
            perror("Malloc failed");
            exit(EXIT_FAILURE);
        }

        *client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (*client_socket < 0) {
            perror("Accept failed");
            free(client_socket);
            continue;
        }

        // Crea un thread per gestire il client
        if (pthread_create(&thread_id, NULL, handle_client, (void *)client_socket) != 0) {
            perror("Thread creation failed");
            close(*client_socket);
            free(client_socket);
        }

        // Non aspetta il thread (perché i thread sono gestiti in modo indipendente)
        pthread_detach(thread_id);
    }

    close(server_socket);
    return 0;
}
