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
                daticonn.local_file = optarg; // Per upload (file locale)
            }

            else if (daticonn.command == 'r'){
                daticonn.remote_file = optarg; // Per download (file remoto)
            }

            else{
                daticonn.remote_file = optarg; // Per listing
            }

            break;

        case 'o':
            daticonn.command2 = 'o';
            break;

        default:
            printf("Usage: %s [-w | -r | -l] -a server_address -p port -f file [-o remote_file]\n", argv[0]);
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
            daticonn.remote_file = daticonn.local_file;
        }

        else if (daticonn.command == 'r')
        {
            daticonn.local_file = daticonn.remote_file;
        }
    }



    // Gestione delle azioni in base al comando
    switch (daticonn.command) {
        case 'w':
            // Upload: file locale -> server remoto
            if (daticonn.local_file == NULL || daticonn.remote_file == NULL) {
                printf("Error: for upload, both local file and remote file must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Uploading file %s to %s on server %s:%d\n", daticonn.local_file, daticonn.remote_file, daticonn.server_address, daticonn.port);
            // Logica per l'upload (es. apertura file, connessione al server, ecc.)
            //gestione connessione
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
            //inviare messaggio che avvisa che il client ha il comando "write"
            
            //comando
            if (send(client_fd,&daticonn.command,sizeof(char), 0)<= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            //dimensione path
            int pathlen = strlen(daticonn.local_file) + 1;
            printf("dimensione: %d \n", pathlen);
            if (send(client_fd,&pathlen, sizeof(pathlen), 0) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            //inviare dove il file : path di destinazione e nome del file (considera anche caso con -o)

            //daticonn.local_file = server/prova.txt

            printf("path : %s\n",daticonn.local_file);
            if (send(client_fd,daticonn.local_file, pathlen, 0) <= 0) {
                perror("Errore durante l'invio del comando");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            //inviare il file 
            break;

        case 'r':
            // Download: file remoto -> file locale
            if (daticonn.remote_file == NULL || daticonn.local_file == NULL) {
                printf("Error: for download, both remote file and local file must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Downloading file %s from %s to local file %s on server %s:%d\n", daticonn.remote_file, daticonn.server_address, daticonn.local_file, daticonn.server_address, daticonn.port);
            // Logica per il download (es. apertura file, connessione al server, ecc.)

            // creare connessione
            // inviare messaggio che avvisa che il client ha il comando "read" il path e il nome del file desiderato
            // riceve riceve il file richiesto dal server (send_file)
            //salva la risposta dentro il path e il filename scelti:



            break;

        case 'l':
            // Listing: elenca i file nel percorso remoto
            if (daticonn.remote_file == NULL) {
                printf("Error: for listing, remote path must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Listing files in remote path %s on server %s:%d\n", daticonn.remote_file, daticonn.server_address, daticonn.port);
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
