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
#include "server.h"

int main(int argc, char const *argv[])
{
    if (argc != 7)
    {
        printf("Errore negli argomenti\n");
        printf("Uso corretto: ./server -a server_address -p server_port -d ft_root_directory\n");
        exit(1);
    }
    
    //inizializzazione varibili di input 
    char *server_address = NULL;
    int server_port = 0;
    char *root_dir = NULL;

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
    
    make_directory(root_dir);
    printf("Server_address: %s \nServer_port: %d \nRoot_dir: %s\n", server_address,server_port,root_dir);

    //VARIABILI
    struct sockaddr_in server_addr,client_addr;// indirizzi del server e del client
    int server_fd, client_fd; // socket del server e del client
    int lenght;
    char buffer[BUFFER_SIZE];
    MSG messaggio;
    
    
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

    // Accetta una connessione (blocca finché non arriva un client)
    socklen_t addr_len = sizeof(client_addr);
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd == -1) {
            perror("Errore nell'accettare la connessione");
            continue;
        }

        printf("Connessione accettata da %s:%d\n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        if (read(client_fd,&messaggio, sizeof(messaggio)) < 0)
        {
            perror("Read error");
            exit(-1);
        }
        
        print("Messaggio ricevuto.\n pathsorgente : %s\n pathremoto: %s\n", messaggio.localpath_file,messaggio.remotepath_file);

        printf("Chiusura della connessione con il client...\n");
        close(client_fd);
        printf("Connassione chiusa.\n In attesa di nuova connessione...\n");
    }




    return 0;
}

// Funzione per creare la directory se non esiste
void make_directory(char *dir) {
    if (access(dir, F_OK) == -1) { // Controlla se la directory non esiste
        if (mkdir(dir, 0777) == -1) { // Prova a creare la directory
            perror("Errore nella creazione della directory");
            exit(1);
        } else {
            printf("Directory '%s' creata.\n", dir);
        }   
    } else { // La directory esiste già
        printf("La directory '%s' esiste già.\n", dir);
    }
}

//controllo se la stringa è formata da soli numeri
int is_numeric( char *str) {

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) {
            return 0; // Se un carattere non è numerico, ritorna falso
        }
    }
    
    return 1; // Tutti i caratteri sono numerici
}