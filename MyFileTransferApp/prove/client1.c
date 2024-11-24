#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <getopt.h>
#include "client1.h"

int main(int argc, char *argv[])
{
    int opt;
    InfoCon daticonn;
    int client_fd;
    struct sockaddr_in server_addr;


    //gestione degli argomenti dell'input
    while ((opt = getopt(argc, argv, "wrla:p:f:o:")) != -1)
    {
        switch (opt)
        {

        case 'w':
            daticonn.command = 'w';// upload(scrittura)
            break;
        
        case 'r':
            daticonn.command = 'r';// download(lettura)
            break;

        case 'l':
            daticonn.command = 'l';// Listing(elencare tutti i file)
            break;
        
        case 'a':
            daticonn.server_address = (char *)optarg;

            break;
        
        case 'p':
            daticonn.port = (int)atoi(optarg);
            break;
        
        case 'f':
            // Gestisce il path locale o remoto in base al comando
            if (daticonn.command == 'w') {
                daticonn.local_path = optarg; // Path locale per upload
            } else if (daticonn.command == 'r' || daticonn.command == 'l') {
                daticonn.remote_path = optarg; // Path remoto per download o elenco
            } else {
                fprintf(stderr, "Errore: opzione -f non valida per il comando '%c'.\n", daticonn.command);
                exit(EXIT_FAILURE);
            }
            break;

        case 'o':
            // Gestisce il path remoto o locale in base al comando
            if (daticonn.command == 'w') {
                daticonn.remote_path = optarg; // Path remoto per upload
            } else if (daticonn.command == 'r') {
                daticonn.local_path = optarg; // Path locale per download
            } else if (daticonn.command == 'l') {
                fprintf(stderr, "Errore: il comando 'l' non può avere l'opzione -o.\n");
                exit(EXIT_FAILURE);
            } else {
                fprintf(stderr, "Errore: opzione -o non valida per il comando '%c'.\n", daticonn.command);
                exit(EXIT_FAILURE);
            }
            break;

        default:
            printf("Usage: %s [-w | -r | -l] -a server_address -p port -f file [-o remote_path]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Verifica che l'indirizzo del server e la porta siano stati forniti
    if (daticonn.server_address == NULL || daticonn.port == 0) {
        
        printf("Error: server address and port are required.\n");
        exit(EXIT_FAILURE);
    }
    
    //Se non viene specificata l'opzione -o
    if (daticonn.command2 != 'o')
    {

        if (daticonn.command == 'w'){
            daticonn.remote_path = daticonn.local_path;
        }

        else if (daticonn.command == 'r'){
            daticonn.local_path = daticonn.remote_path;
        }
    }

    printf("Indirizzo IP: %s \nPorta: %d \nCommand: %c \nLocalpath: %s \nRemote path: %s\n", daticonn.server_address,daticonn.port,daticonn.command,daticonn.local_path,daticonn.remote_path);

    //connessione
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        perror("Socket error\n");
        exit(1);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(daticonn.server_address);
    server_addr.sin_port = htons(daticonn.port);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore di connessione");
        return 1;
    }
    printf("Connessione al server effettuata.\n");

    // invio del comando
    if (send(client_fd,&daticonn.command,sizeof(char), 0)<= 0) {
        perror("Errore durante l'invio del comando");
        close(client_fd);
        exit(EXIT_FAILURE);
    }


    // Gestione delle azioni in base al comando
    switch (daticonn.command) {
        case 'w':
            // Upload: file locale -> server remoto
            if (daticonn.local_path == NULL || daticonn.remote_path == NULL) {
                printf("Error: for upload, both local file and remote file must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Uploading...\n");

            if (daticonn.command2 == 'o')
            {
            // invia dimensione path
            int pathlen = strlen(daticonn.remote_path) + 1;
            printf("dimensione: %d \n", pathlen);
            if (send_message(client_fd,&pathlen, sizeof(pathlen))<= 0)
            {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }

            // invia il path 
            printf("path : %s\n",daticonn.remote_path);
            if (send_message (client_fd,daticonn.remote_path, pathlen) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            //inviare il file
            send_file(daticonn.remote_path, client_fd);
            close(client_fd);  
            }
            
            else
            {
            //dimensione path
            int pathlen = strlen(daticonn.local_path) + 1;
            printf("dimensione: %d \n", pathlen);
            if (send_message(client_fd,&pathlen, sizeof(pathlen)) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            //inviare dove il file : path di destinazione e nome del file (considera anche caso con -o)

            //daticonn.local_path = server/prova.txt

            printf("path : %s\n",daticonn.local_path);
            if (send_message(client_fd,daticonn.local_path, pathlen) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            //inviare il file
            
            send_file(daticonn.local_path, client_fd);
            close(client_fd); 
            }

            break;

        case 'r':
            // Download: file remoto -> file locale
            if (daticonn.remote_path == NULL || daticonn.local_path == NULL) {
                printf("Error: for download, both remote file and local file must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Downloading...\n");

            // invii la dimensione del il path del file desiderato
            int pathlen = strlen(daticonn.remote_path) + 1;
            printf("dimensione: %d \n", pathlen);
            if (send_message(client_fd,&pathlen, sizeof(pathlen)) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            // invii il path 
            printf("path : %s\n",daticonn.remote_path);
            if (send_message(client_fd,daticonn.remote_path, pathlen) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            //Aggiornamento del path
            char fixed_path[1024];
            fix_path(daticonn.local_path, fixed_path);
            daticonn.local_path = strdup(fixed_path); // Aggiorna il valore con il path corretto

            //controllo esistenza del file
            create_file(daticonn.local_path);
            // riceve riceve il file richiesto dal server (send_file)
            printf("ricezione del file...\n");
            receive_file(daticonn.local_path,client_fd);
            printf("ricezione del file completata.\n");
            printf("chiusura connessione.\n");
           
            close(client_fd);


            break;

        case 'l':
            // Listing: elenca i file nel percorso remoto
            if (daticonn.remote_path == NULL) {
                printf("Error: for listing, remote path must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Listing files in remote path %s on server %s:%d\n", daticonn.remote_path, daticonn.server_address, daticonn.port);
            // Logica per il listing (es. connessione al server e recupero elenco file)
            // creare connessione
            // inviare messaggio che avvisa che il client ha il comando "listing"
            // riceve il listing del path che ha inviato sottoforma di stringa

            break;

        default:
            printf("Error: invalid command. Use -w for upload, -r for download, or -l for listing.\n");
            exit(EXIT_FAILURE);
    }


    return 0;
}



void create_file(char *full_path){

    // Percorso finale completo
    char full_client_path[1024];

    // Controlla se full_path inizia con base_dir
    if (strncmp(full_path, base_dir, strlen(base_dir)) == 0) {
        // Se sì, usa full_path direttamente
        snprintf(full_client_path, sizeof(full_client_path), "%s", full_path);
    } else {
        // Altrimenti, concatenalo con base_dir
        snprintf(full_client_path, sizeof(full_client_path), "%s/%s", base_dir, full_path);
    }

    // Divide il percorso completo in path e filename
    char path[1024];
    char filename[256];

    // Usa split_path per separare il percorso dal nome del file
    split_path(full_client_path, path, filename);

    // Crea una copia temporanea del percorso
    char path_copy[1024];
    strncpy(path_copy, path, sizeof(path_copy));  // Copia la stringa originale
    path_copy[sizeof(path_copy) - 1] = '\0';     // Assicura il terminatore

    // Crea le directory necessarie
    if (strlen(path) > 0) {
        char built_path[1024] = "";
        char *dir_part = strtok(path_copy, "/");

        // Aggiungi il primo segmento del path direttamente
        if (dir_part != NULL) {
            strcpy(built_path, dir_part); // Inizia con il primo segmento senza '/'
            dir_part = strtok(NULL, "/");
        }

        // Inizia il percorso con la directory base
        snprintf(built_path, sizeof(built_path), "%s", base_dir);

        while (dir_part) {
            strcat(built_path, "/");
            strcat(built_path, dir_part);

            //debug per vedere cosa cerca di costruire 
            printf("Creando la directory: %s\n", built_path);

            // Crea la directory (relativa alla root directory serverDir)
            if (access(built_path, F_OK) == -1) { 
                // Se la directory non esiste, tenta di crearla
                if (mkdir(built_path, 0777) != 0 && errno != EEXIST) {
                    fprintf(stderr, "Errore nella creazione della directory '%s': %s\n", built_path, strerror(errno));
                    return;
                } else {
                    printf("Directory creata: %s\n", built_path);
                }
            } else {
                printf("Directory esistente: %s\n", built_path);
            }

            dir_part = strtok(NULL, "/");
        }
    }

    // Ora crea il file all'interno del percorso completo fornito
    FILE *file = fopen(full_client_path, "wb");  // Usa "wb" per scrivere un file binario
    if (file == NULL) {
        perror("Errore nella creazione del file");
        return;
    }
    printf("File creato con successo: %s\n", full_client_path);
    fclose(file);
}

void split_path(const char *full_path, char *path, char *filename){

    // Trova l'ultimo separatore di directory
    const char *last_slash = strrchr(full_path, '/');
    
    if (last_slash != NULL) {
        // Calcola la lunghezza del path
        size_t path_length = last_slash - full_path + 1;

        // Copia il path nella variabile
        strncpy(path, full_path, path_length);
        path[path_length] = '\0'; // Aggiungi il terminatore

        // Copia il filename
        strcpy(filename, last_slash + 1);
    } else {
        // Se non c'è il separatore, non c'è un path
        strcpy(path, ""); // Path vuoto
        strcpy(filename, full_path); // Il filename è l'intera stringa
    }
}

void fix_path(const char *input_path, char *output_path) {
    // Verifica se il path inizia con base_dir
    if (strncmp(input_path, base_dir, strlen(base_dir)) == 0) {
        snprintf(output_path, 1024, "%s", input_path);
    } else {
        snprintf(output_path, 1024, "%s/%s", base_dir, input_path);
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
