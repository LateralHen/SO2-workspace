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
#include "client.h"


int main(int argc, char *argv[])
{
    int opt;
    char *server_address = NULL;
    int port = 0;
    char *local_file = NULL;
    char *remote_file = NULL;
    char *remote_path = NULL;
    char command = '\0';

    while ((opt = getopt(argc, argv, "wrla:p:f:o:")) != -1)
    {
        switch (opt)
        {
        case 'w':
            command = 'w';// upload(scrittura)
            break;
        
        case 'r':
            command = 'r';// download(lettura)
            break;

        case 'l':
            command = 'l';// Listing(elencare tutti i file)
            break;
        
        case 'a':

            server_address = (char *)optarg;

            break;
        
        case 'p':
            port = (int)atoi(optarg);
            break;
        
        case 'f':
            if (command == 'w') {
                local_file = optarg; // Per upload (file locale)
            } else if (command == 'r')
            {
                remote_file = optarg; // Per download (file remoto)
            }else
            {
                remote_path = optarg; // Per listing (listing)
            }
            
            
            break;
        case 'o':
            remote_file = optarg; // for upload/download
            break;
        default:
            printf("Usage: %s [-w | -r | -l] -a server_address -p port -f file [-o remote_file]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    
    // Verifica che l'indirizzo del server e la porta siano stati forniti
    if (server_address == NULL || port == 0) {
        
        printf("Error: server address and port are required.\n");
        exit(EXIT_FAILURE);
    }

    // Gestione delle azioni in base al comando
    switch (command) {
        case 'w':
            // Upload: file locale -> server remoto
            if (local_file == NULL || remote_file == NULL) {
                printf("Error: for upload, both local file and remote file must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Uploading file %s to %s on server %s:%d\n", local_file, remote_file, server_address, port);
            // Logica per l'upload (es. apertura file, connessione al server, ecc.)
            break;

        case 'r':
            // Download: file remoto -> file locale
            if (remote_file == NULL || local_file == NULL) {
                printf("Error: for download, both remote file and local file must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Downloading file %s from %s to local file %s on server %s:%d\n", remote_file, server_address, local_file, server_address, port);
            // Logica per il download (es. apertura file, connessione al server, ecc.)
            break;

        case 'l':
            // Listing: elenca i file nel percorso remoto
            if (remote_path == NULL) {
                printf("Error: for listing, remote path must be specified.\n");
                exit(EXIT_FAILURE);
            }
            printf("Listing files in remote path %s on server %s:%d\n", remote_file, server_address, port);
            // Logica per il listing (es. connessione al server e recupero elenco file)
            break;

        default:
            printf("Error: invalid command. Use -w for upload, -r for download, or -l for listing.\n");
            exit(EXIT_FAILURE);
    }

    // connessione
    // variabili per la connessione
    int client_fd;
    struct sockaddr_in server_addr;
    struct hostent *hp;

    //socket

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd < 0 )
    {
        perror("Socket error");
        exit(-1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_address);

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        perror("Error localhost");
        exit(-1);
    }

    printf("Siamo connessi al server\n");





    return 0;
}


