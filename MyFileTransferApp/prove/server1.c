#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "server1.h"

int main(int argc, char const *argv[])
{
    //varibili di input 
    char *server_address = NULL;
    int server_port = 0;
    char *root_dir = NULL;

    //VARIABILI
    struct sockaddr_in server_addr,client_addr;// indirizzi del server e del client
    int server_fd, client_fd; // socket del server e del client
    int lenght;
    char buffer[BUFFER_SIZE];


    if (argc != 7)
    {
        printf("Errore negli argomenti\n");
        printf("Uso corretto: ./server -a server_address -p server_port -d ft_root_directory\n");
        exit(1);
    }  

    // divisione degli argomenti della riga di comando
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-a") == 0)
        {
            if (i + 1 < argc)
            {
                server_address = (char *)argv[i + 1];
                i++;
            }else
            {
                printf("Errore: manca il valore per l'opzione -a\n");
                exit(1);
            }
        }else if (strcmp(argv[i], "-p") == 0)
        {
            
            if(i + 1 < argc){

                if (is_numeric((char *)argv[i + 1]))
                    {
                        server_port = atoi(argv[i + 1]); 
                    }
                i++;
            }else{
                printf("Errore: manca il valore per l'opzione -p\n");
                exit(1);
            }

        }else if (strcmp(argv[i], "-d") == 0)
        {
            if(i + 1 < argc){
                root_dir = (char *)argv[i + 1];
                i++;
            }else
            {
                printf("Errore: manca il valore per l'opzione -d\n");
                exit(1);
            }
        }
    }

    
    if (!server_address || server_port == 0 || !root_dir)
    {
        printf("Errore nei parametri.");
        exit(1);
    }
    // creazione socket
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if (server_fd < 0)
    {
        perror("Socket error\n");
        exit(1);
    }
    
    //creazione indirizzo
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_address);

    //Binding del socket
    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("Binding error");
        exit(-1);
    }

    //Inizializzazione del server in ascolto
    if (listen(server_fd,MAX_CONNECTION)== -1)
    {
        perror("Listen error");
        exit(-1);
    }
    
    printf("Server in ascolto su %s:%d\n", server_address, server_port);

    while (1)
    {
        socklen_t addr_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd == -1) {
            perror("Errore nell'accettare la connessione");
            continue;
        }
        printf("Connessione accettata da %s:%d\n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        processing(client_fd);
        printf("Chiusura della connessione con il client...\n");
        close(client_fd);
    }
    
    return 0;
}


int is_numeric( char *str) {

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) {
            return 0; // Se un carattere non è numerico, ritorna falso
        }
    }
    
    return 1; // Tutti i caratteri sono numerici
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


void processing( int socket){
    /*
    qui io in base al comando che ricevo, preparo il server a fare le operazioni richieste.
    */
    
    // Comando
    char command;       
    if (recv(socket, &command, sizeof(char), 0) <= 0) {
    perror("Errore durante la ricezione del comando");
    close(socket);

    }
    printf("Comando ricevuto: %c\n", command);

    //dimensione del path
    int pathlen;
    if (recv(socket, &pathlen, sizeof(pathlen), 0) <= 0) {
    perror("Errore durante la ricezione della dimensione del path");
    close(socket);

    }
    printf("dimensione: %d \n", pathlen);

    char *path = (char *)malloc(pathlen * sizeof(char)); 
    // Controllo del path ricevuto
    if (recv(socket, path, pathlen, 0) <= 0) {
    perror("Errore durante la ricezione del comando");
    close(socket);
    }
    printf("Comando ricevuto: %s \n", path);

    
    char *filename = NULL;
    char *pathfile = NULL;
    split_path(path, pathfile, filename);

    printf("il path ricevuto è: %s\nIl nome del file ricevuto è:%s\n", pathfile, filename );
    
    
    switch (command)
    {
    case 'w':
        /*
        In questo caso il client vuole inviare il file e lo vuole salvare all'indirizzo selezionato con il nome selezionato.
        Qui devo invocare quindi la funzione di recive file per ricevere i dati del file in questione.
        Quando la trasmissione dei dati del file è terminata devo chiudere la connessione con il client e salvare il file in quel path con quel filename.
        */
        //printf("Ricezione della richiesta...\n");
        //devo controllare se il file e il path esite (funzione da creare)
        
        
        //apertura del file
        //FILE *file = fopen(path, "wb");
        //if (!file) // se il file non esiste/ci sono errori nell'aprire il file
        {
            //perror("Errore nell'aprire il file per la scrittura\n");


        }

        break;
    
    case 'r':
        /*
        In questo caso il client vuole leggere il file presente all'indirizzo selezionato con quel nome.
        Qui devo invocare la funzione di send file per inviare il file in questione.
        Quando finisce la trasmissione devo solamente chiudere la connessione con il client.
        */
        break;
    
    case 'l':
        /*
        In questo caso il client vuole leggere quali file sono presenti nella libreria specificata.
        Qui devo creare la stringa che conterrà il listing della cartella e la devo inviare al client.
        Quando finisce la trasmissione devo solamente chiudere la connessione con il client.
        */
        break;
    
    default:

        break;
    }
    
}