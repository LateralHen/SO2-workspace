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
    int client_fd;
    struct sockaddr_in server_addr;


    //gestione degli argomenti dell'input
    while ((opt = getopt(argc, argv, "wrla:p:f:o:")) != -1)
    {
        switch (opt)
        {

        case 'w':
            comando_imput.command = 'w';// upload(scrittura)
            break;
        
        case 'r':
            comando_imput.command = 'r';// download(lettura)
            break;

        case 'l':
            comando_imput.command = 'l';// Listing(elencare tutti i file)
            break;
        
        case 'a':
            connessione.server_address = (char *)optarg;

            break;
        
        case 'p':
            connessione.port = (int)atoi(optarg);
            break;
        
        case 'f':
            // Gestisce il path locale o remoto in base al comando
            if (comando_imput.command == 'w') {
                comando_imput.local_path = optarg; // Path locale per upload
            } else if (comando_imput.command == 'r' || comando_imput.command == 'l') {
                comando_imput.remote_path = optarg; // Path remoto per download o elenco
            } else {
                fprintf(stderr, "Errore: opzione -f non valida per il comando '%c'.\n", comando_imput.command);
                exit(EXIT_FAILURE);
            }
            break;

        case 'o':
            // Gestisce il path remoto o locale in base al comando
            if (comando_imput.command == 'w') {
                comando_imput.remote_path = optarg; // Path remoto per upload
            } else if (comando_imput.command == 'r') {
                comando_imput.local_path = optarg; // Path locale per download
            } else if (comando_imput.command == 'l') {
                fprintf(stderr, "Errore: il comando 'l' non può avere l'opzione -o.\n");
                exit(EXIT_FAILURE);
            } else {
                fprintf(stderr, "Errore: opzione -o non valida per il comando '%c'.\n", comando_imput.command);
                exit(EXIT_FAILURE);
            }
            break;

        default:
            printf("Usage: %s [-w | -r | -l] -a server_address -p port -f file [-o remote_path]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Verifica che l'indirizzo del server e la porta siano stati forniti
    if (connessione.server_address == NULL || connessione.port == 0) {
        
        printf("Error: server address and port are required.\n");
        exit(EXIT_FAILURE);
    }
    
    //Se non viene specificata l'opzione -o
    if (comando_imput.command2 != 'o')
    {
        if (comando_imput.command == 'w'){
            comando_imput.remote_path = comando_imput.local_path;
        }

        else if (comando_imput.command == 'r'){
            comando_imput.local_path = comando_imput.remote_path;
        }
    }

    //connessione
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0){
        perror("Socket error\n");
        exit(1);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(connessione.server_address);
    server_addr.sin_port = htons(connessione.port);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore di connessione");
        return 1;
    }
    printf("Connessione al server effettuata.\n");

    //elaborazione dei comandi e la gestione del invio/ricevemento dati/file
    processing(client_fd);


    return 0;
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
    
    printf("path: %s\n", path);

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
    printf("file ricevuto con successo\n");
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

void fix_path(const char *input_path, char **output_path, int add_base) {
    size_t base_dir_len = strlen(base_dir);
    size_t input_path_len = strlen(input_path);

    if (add_base) {
        // Caso: aggiungere base_dir
        if (strncmp(input_path, base_dir, base_dir_len) == 0) {
            *output_path = strdup(input_path); // Copia il percorso così com'è
        } else {
            // Allocazione dinamica per il nuovo percorso
            *output_path = malloc(base_dir_len + input_path_len + 2); // +2 per '/' e '\0'
            if (*output_path == NULL) {
                perror("Errore di allocazione memoria");
                exit(EXIT_FAILURE);
            }
            snprintf(*output_path, base_dir_len + input_path_len + 2, "%s/%s", base_dir, input_path);
        }
    } else {
        // Caso: rimuovere base_dir
        if (strncmp(input_path, base_dir, base_dir_len) == 0) {
            size_t remaining_len = input_path_len - base_dir_len;
            *output_path = malloc(remaining_len + 1); // +1 per '\0'
            if (*output_path == NULL) {
                perror("Errore di allocazione memoria");
                exit(EXIT_FAILURE);
            }
            snprintf(*output_path, remaining_len + 1, "%s", input_path + base_dir_len);

            // Rimuove eventuale '/' iniziale
            if ((*output_path)[0] == '/') {
                memmove(*output_path, *output_path + 1, remaining_len);
            }
        } else {
            *output_path = strdup(input_path); // Copia il percorso così com'è
        }
    }
}

void create_file(char *full_path){


    // Divide il percorso completo in path e filename
    char path[1024];
    char filename[256];

    // Usa split_path per separare il percorso dal nome del file
    split_path(full_path, path, filename);

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
    FILE *file = fopen(full_path, "wb");  // Usa "wb" per scrivere un file binario
    if (file == NULL) {
        perror("Errore nella creazione del file");
        return;
    }
    printf("File creato con successo: %s\n", full_path);
    fclose(file);
}

void processing(int socket){

    // invio del comando
    if (send(socket,&comando_imput.command,sizeof(char), 0)<= 0) {
        perror("Errore durante l'invio del comando");
        close(socket);
        exit(EXIT_FAILURE);
    }


    // Gestione delle azioni in base al comando
    switch (comando_imput.command) {
        case 'w':
            // Upload: file locale -> server remoto
            if (comando_imput.local_path == NULL || comando_imput.remote_path == NULL) {
                printf("Error: for upload, both local file and remote file must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Uploading...\n");

            if (comando_imput.command2 == 'o') {


                printf("opzione -o\n");

                // invia dimensione path
                int pathlen = strlen(comando_imput.remote_path) + 1;
                printf("dimensione: %d \n", pathlen);
                if (send_message(socket,&pathlen, sizeof(pathlen))<= 0)
                {
                    perror("Errore durante l'invio del comando");
                    close(socket);
                    exit(EXIT_FAILURE);
                }

                // invia il path 
                printf("path : %s\n",comando_imput.remote_path);
                if (send_message (socket,comando_imput.remote_path, pathlen) <= 0) {
                    perror("Errore durante l'invio del comando");
                    close(socket);
                    exit(EXIT_FAILURE);
                }
                //inviare il file
                send_file(comando_imput.remote_path, socket);
                close(socket);  
            }
            
            else {
                printf("senza opzione -o\n");
                //dimensione path
                int pathlen = strlen(comando_imput.local_path) + 1;
                printf("dimensione: %d \n", pathlen);
                if (send_message(socket,&pathlen, sizeof(pathlen)) <= 0) {
                    perror("Errore durante l'invio del comando");
                    close(socket);
                    exit(EXIT_FAILURE);
                }
                //inviare dove il file : path di destinazione e nome del file (considera anche caso con -o)
                
                printf("path : %s\n",comando_imput.local_path);
                if (send_message(socket,comando_imput.local_path, pathlen) <= 0) {
                    perror("Errore durante l'invio del comando");
                    close(socket);
                    exit(EXIT_FAILURE);
                }
                //inviare il file
                
                char *fixed_path;
                fix_path(comando_imput.local_path, &fixed_path, 1);
                
                printf("file da inviare : %s\n",fixed_path);
                send_file(fixed_path, socket);
                close(socket); 
            }

            break;

        case 'r':
            // Download: file remoto -> file locale
            if (comando_imput.remote_path == NULL || comando_imput.local_path == NULL) {
                printf("Error: for download, both remote file and local file must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Downloading...\n");

            // invii la dimensione del il path del file desiderato
            int pathlen = strlen(comando_imput.remote_path) + 1;
            printf("dimensione: %d \n", pathlen);
            if (send_message(socket,&pathlen, sizeof(pathlen)) <= 0) {
                perror("Errore durante l'invio del comando");
                close(socket);
                exit(EXIT_FAILURE);
            }
            // inviare il path 
            printf("path : %s\n",comando_imput.remote_path);
            if (send_message(socket,comando_imput.remote_path, pathlen) <= 0) {
                perror("Errore durante l'invio del comando");
                close(socket);
                exit(EXIT_FAILURE);
            }


            //Aggiornamento del path************
            printf("path da modificare: %s\n",comando_imput.local_path);
            char *fixed_path;
            fix_path(comando_imput.local_path, &fixed_path, 1);
            printf("path generato: %s\n",fixed_path);



            //controllo esistenza del file
            create_file(fixed_path);
            
            // riceve riceve il file richiesto dal server (send_file)


            receive_file(fixed_path,socket);
            printf("chiusura Downloading.\n");
           
            close(socket);


            break;

        case 'l':
            // Listing: elenca i file nel percorso remoto
            if (comando_imput.remote_path == NULL) {
                printf("Error: for listing, remote path must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Listing files in remote path %s on server %s:%d\n", comando_imput.remote_path, connessione.server_address, connessione.port);
            // Logica per il listing (es. connessione al server e recupero elenco file)

            //invio dimensione del path
            int path_len = strlen(comando_imput.remote_path) + 1;
            printf("dimensione: %d \n", path_len);
            if (send_message(socket,&path_len, sizeof(path_len)) <= 0) {
                perror("Errore durante l'invio della dimensione");
                close(socket);
                exit(EXIT_FAILURE);
            }
            // invii il path 
            printf("path : %s\n",comando_imput.remote_path);
            if (send_message(socket,comando_imput.remote_path, path_len) <= 0) {
                perror("Errore durante l'invio del path");
                close(socket);
                exit(EXIT_FAILURE);
            }
            //ricezione del listing e print di esso
            int resultLen;
            receive_message(socket, &resultLen, sizeof(resultLen));
            printf("%d\n", resultLen);
            char *result = (char *) malloc(resultLen);
            receive_message(socket, result, resultLen);
            printf("cartella: %s\ncontenuto: %s\n", comando_imput.remote_path,result);
            break;

        default:
            printf("Error: invalid command. Use -w for upload, -r for download, or -l for listing.\n");
            exit(EXIT_FAILURE);
    }

}