#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include "server1.h"


// Funzione principale
int main(int argc, char *argv[]) {
    int opt;
    struct sockaddr_in server_addr, client_addr;
    int server_fd, client_fd;

    while ((opt = getopt(argc, argv, "a:p:d:")) != -1) {
        switch (opt) {
            case 'a':
                if (inet_addr(optarg) == INADDR_NONE)
                {
                    perror("Indirizzo Ip non valido.\n");
                    return 1;
                }
                server_address = optarg;
                break;
            case 'p':
                server_port = atoi(optarg);
                if (server_port < 1 || server_port > 65535) {
                    fprintf(stderr, "Errore: La porta deve essere tra 1 e 65535.\n");
                    return 1;
                }
                break;
            case 'd':
                root_dir = optarg;
                make_root(root_dir);
                break;
            default:
                fprintf(stderr, "Uso: %s -a indirizzo -p porta -d root Directory\n", argv[0]);
                return 1;
        }
    }

    if (!server_address || server_port == 0 || !root_dir) {
        fprintf(stderr, "Errore nei parametri.\n");
        exit(1);
    }

    // Creazione del socket
    if (server_fd = socket(AF_INET, SOCK_STREAM, 0) < 0) {
        perror("Errore nella creazione del socket");
        exit(1);
    }

    // Configurazione dell'indirizzo
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_address);

    // Binding
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore nel binding");
        exit(1);
    }

    // Ascolto delle connessioni
    if (listen(server_fd, MAX_CONNECTION) == -1) {
        perror("Errore nell'ascolto");
        exit(1);
    }

    printf("Server in ascolto su %s:%d\n", server_address, server_port);

    // Ciclo di accettazione connessioni
    while (1) {
        socklen_t addr_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd == -1) {
            perror("Errore nell'accettare la connessione");
            continue;
        }
        printf("Connessione accettata da %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        processing(client_fd);
        printf("Chiusura della connessione con il client...\n");
        close(client_fd);
    }

    return 0;
}

// Implementazione delle funzioni correlate ai file

void split_path(const char *full_path, char *path, char *filename) {
    const char *last_slash = strrchr(full_path, '/');
    if (last_slash != NULL) {
        size_t path_length = last_slash - full_path + 1;
        strncpy(path, full_path, path_length);
        path[path_length] = '\0';
        strcpy(filename, last_slash + 1);
    } else {
        strcpy(path, "");
        strcpy(filename, full_path);
    }
}

int send_message(int socket, const void *message, size_t length) {
    ssize_t bytes_sent = 0;
    while (bytes_sent < length) {
        ssize_t result = send(socket, (char*)message + bytes_sent, length - bytes_sent, 0);
        if (result < 0) {
            perror("Errore durante l'invio del messaggio");
            return -1;  // Errore durante l'invio
        }
        bytes_sent += result;
    }
    return bytes_sent;  // Ritorna il numero totale di byte inviati
}

ssize_t receive_message(int socket, void *buffer, size_t length) {
    ssize_t bytes_received = 0;
    while (bytes_received < length) {
        ssize_t result = recv(socket, (char*)buffer + bytes_received, length - bytes_received, 0);
        if (result < 0) {
            perror("Errore durante la ricezione del messaggio");
            return -1;  // Errore durante la ricezione
        }
        if (result == 0) {
            printf("Connessione chiusa dal client\n");
            return 0;  // Connessione chiusa
        }
        bytes_received += result;
    }
    return bytes_received;  // Ritorna il numero totale di byte ricevuti
}


void send_file(const char *path, int socket) {
    char buffer[BUFFER_SIZE];
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        perror("Errore nell'aprire il file");
        return;
    }

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send(socket, buffer, bytes_read, 0) < 0) {
            perror("Errore durante l'invio dei dati");
            fclose(file);
            return;
        }
    }

    fclose(file);
}

void receive_file(const char *path, int socket) {
    char buffer[BUFFER_SIZE];
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        perror("Errore nell'aprire il file");
        return;
    }

    ssize_t bytes_received;
    while ((bytes_received = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    if (bytes_received < 0) {
        perror("Errore durante la ricezione dei dati");
    }

    fclose(file);
}

// Gestione delle connessioni

void processing(int socket) {
    char command;
    if (receive_message(socket, &command, sizeof(char)) < 0){
        perror("Errore durante l'invio del comando");
        close(socket);
        return;
    }

    int pathlen;
    if (receive_message(socket, &pathlen, sizeof(pathlen)) < 0){
        perror("Errore durante la ricezione della dimensione del path");
        close(socket);
        return;
    }


    char *full_path = (char *)malloc(pathlen);
    if (receive_message(socket, full_path, pathlen) < 0)
    {
        perror("Errore durante la ricezione del path");
        free(full_path);
        close(socket);
        return;
    }
    
    fix_path(full_path, "clientDir");// TODO: aggingere ricezione della root del client
    char filename[256], pathfile[1024];
    split_path(full_path, pathfile, filename);

    switch (command) {
        case 'w':
            printf("***FUNZIONE DI WRITE***\n");
            receive_file(full_path, socket);
            break;
        case 'r':
            printf("***FUNZIONE DI READ***\n");
            send_file(full_path, socket);
            break;
        case 'l':
            printf("***FUNZIONE DI LISTING***\n");
            break;
        default:
            fprintf(stderr, "Comando non riconosciuto\n");
            break;
    }

    free(full_path);
    close(socket);
}

// Funzioni di gestione della root e percorsi

void make_root(char *dir) {
    if (access(dir, F_OK) == -1 && mkdir(dir, 0777) == -1) {
        perror("Errore nella creazione della directory");
        exit(1);
    }
}

void create_file(char *full_path) {
    FILE *file = fopen(full_path, "wb");
    if (file == NULL) {
        perror("Errore nella creazione del file");
        return;
    }
    fclose(file);
}

void fix_path(char *full_path, const char *clientDir) {
    if (strncmp(full_path, clientDir, strlen(clientDir)) == 0) {
        memmove(full_path, full_path + strlen(clientDir) + 1, strlen(full_path) - strlen(clientDir));
    }

    if (strncmp(full_path, root_dir, strlen(root_dir)) != 0) {
        char temp_path[1024];
        snprintf(temp_path, sizeof(temp_path), "%s/%s", root_dir, full_path);
        strcpy(full_path, temp_path);
    }
}
