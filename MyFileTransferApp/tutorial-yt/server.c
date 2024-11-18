#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define LEN_MAX 20

typedef struct _MSG
{
    int value;
    char tipo[LEN_MAX];
} MSG;




int main(int argc, char const *argv[])
{
    int sock;
    struct sockaddr_in server, client;
    int lenght;
    int current_sock;
    int porta = 20000;
    MSG messaggio;

    // creazione socket
    sock = socket(AF_INET,SOCK_STREAM, 0);
    if (sock < 0 )
    {
        perror("Socket error");
        exit(-1);
    }
    
    // Binding
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(porta);

    if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("Binding error");
        exit(-1);
    }

    // getsockname

    socklen_t server_len = sizeof(server);
    if (getsockname(sock, (struct sockaddr *)&server, &server_len) < 0) {
        perror("Error in getsockname");
        exit(-1);
    }

    
    // Listen
    if (listen(sock, 3) < 0)
    {
        perror("Listen error");
        exit(-1);
    }
    
    printf("Server attivo alla porta %d, in attesa di connessioni\n", ntohs(server.sin_port));
    while (1)
    {
        socklen_t length = sizeof(client);
        current_sock = accept(sock, (struct sockaddr *)&client, &length);
        if (current_sock < 0) {
            perror("Accept error");
            exit(-1);
        }


        int pid = fork();

        if (pid < 0 )
        {
            perror("Fork error");
            exit(-1);
        }

        if (pid == 0)
        {
            // gestire richiesta
            if (read(current_sock,&messaggio,sizeof(messaggio)) < 0)
            {
                perror("Read error");
                exit(-1);
            }
            printf("Ricevuto messaggio da client. tipo: %s, value:%d'\n", messaggio.tipo, messaggio.value);
            close(current_sock);
            exit(0);
        }
        

        else if (pid > 0)
        {
            // processo padre
            close(current_sock);
        }
        
        
    }
    
    return 0;
}
