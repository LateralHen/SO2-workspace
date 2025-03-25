#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define NUM_CLIENTS 5

// Funzione per il client
void *run_client(void *arg) {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Crea un socket TCP
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        return NULL;
    }

    // Configura l'indirizzo del server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // IP del server (localhost)
    server_addr.sin_port = htons(PORT);

    // Connetti al server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_socket);
        return NULL;
    }

    // Invia un messaggio al server
    const char *message = "Hello from client!";
    send(client_socket, message, strlen(message), 0);

    // Ricevi la risposta dal server
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("Receive failed");
    } else {
        buffer[bytes_received] = '\0';
        printf("Received from server: %s\n", buffer);
    }

    // Chiudi la connessione
    close(client_socket);
    return NULL;
}

int main() {
    pthread_t threads[NUM_CLIENTS];

    // Avvia i client in parallelo utilizzando i thread
    for (int i = 0; i < NUM_CLIENTS; i++) {
        if (pthread_create(&threads[i], NULL, run_client, NULL) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    // Aspetta che tutti i thread finiscano
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Tutti i client hanno terminato.\n");
    return 0;
}
