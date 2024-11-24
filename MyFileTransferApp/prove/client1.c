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
            if (daticonn.command == 'w') {
                daticonn.local_path = optarg; // Per upload (file locale)
            }

            else if (daticonn.command == 'r'){
                daticonn.remote_path = optarg; // Per download (file remoto)
            }

            else{
                daticonn.remote_path = optarg; // Per listing
            }

            break;

        case 'o':
            daticonn.command2 = 'o';
            if (daticonn.command == 'w'){
            daticonn.remote_path = optarg;
            }

            else if (daticonn.command == 'r'){
            daticonn.local_path = optarg;
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


    // Gestione delle azioni in base al comando
    switch (daticonn.command) {
        case 'w':
            // Upload: file locale -> server remoto
            if (daticonn.local_path == NULL || daticonn.remote_path == NULL) {
                printf("Error: for upload, both local file and remote file must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Uploading...\n");

            
            //inviare messaggio che avvisa che il client ha il comando "write"
            
            if (send(client_fd,&daticonn.command,sizeof(char), 0)<= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }

            if (daticonn.command2 == 'o')
            {
            // invia dimensione path
            int pathlen = strlen(daticonn.remote_path) + 1;
            printf("dimensione: %d \n", pathlen);
            if (send(client_fd,&pathlen, sizeof(pathlen), 0) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }

            // invia il path 
            printf("path : %s\n",daticonn.remote_path);
            if (send(client_fd,daticonn.remote_path, pathlen, 0) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            //inviare il file
            FILE *file_write = fopen(daticonn.local_path, "rb");
            if (!file_write)
            {
                perror("Errore nell'aprire il file: ");
                close(client_fd);
            }
            
            send_file(file_write, client_fd);
            close(client_fd);  
            }
            
            else
            {
            //dimensione path
            int pathlen = strlen(daticonn.local_path) + 1;
            printf("dimensione: %d \n", pathlen);
            if (send(client_fd,&pathlen, sizeof(pathlen), 0) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            //inviare dove il file : path di destinazione e nome del file (considera anche caso con -o)

            //daticonn.local_path = server/prova.txt

            printf("path : %s\n",daticonn.local_path);
            if (send(client_fd,daticonn.local_path, pathlen, 0) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            //inviare il file
            FILE *file_write = fopen(daticonn.local_path, "rb");
            if (!file_write)
            {
                perror("Errore nell'aprire il file: ");
                close(client_fd);
            }
            
            send_file(file_write, client_fd);
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

            // inviare messaggio che avvisa che il client ha il comando "read"
            if (send(client_fd,&daticonn.command,sizeof(char), 0)<= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }


            // invii la dimensione del il path del file desiderato
            int pathlen = strlen(daticonn.remote_path) + 1;
            printf("dimensione: %d \n", pathlen);
            if (send(client_fd,&pathlen, sizeof(pathlen), 0) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            // invii il path 
            printf("path : %s\n",daticonn.remote_path);
            if (send(client_fd,daticonn.remote_path, pathlen, 0) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            //controllo esistenza del file
            create_file(daticonn.local_path);


            FILE *file_read = fopen(daticonn.local_path, "wb");
            if (!file_read) // se il file non esiste/ci sono errori nell'aprire il file
            {
            perror("Errore nell'aprire il file:");
            printf("Creazione del file...\n");
            create_file(daticonn.local_path);
            FILE *file_read = fopen(daticonn.local_path, "wb");
            }
            else{
            printf("file già esistente\n");
            }
            // riceve riceve il file richiesto dal server (send_file)
            
            printf("ricezione del file...\n");

            receive_file(file_read,client_fd);
            fclose(file_read);
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



void receive_file(FILE *file, int socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Riceve dal socket e scrive nel file
    while ((bytes_received = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }
}

void send_file(FILE *file, int socket) {
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Legge dal file e invia al socket
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(socket, buffer, bytes_read, 0);
    }
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