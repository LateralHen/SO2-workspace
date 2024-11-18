#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 


#define SERVER_PORT 20000
#define LEN_MAX 20

typedef struct _MSG
{
    int value;
    char tipo[LEN_MAX];
} MSG;

int main(int argc, char const *argv[])
{
    int client_fd;
    struct sockaddr_in server_address;
    struct hostent *hp;
    MSG messaggio;


    //socket

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd < 0 )
    {
        perror("Socket error");
        exit(-1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
   /*  
    //gethostbyname

    hp = gethostbyname("localhost");
    if (hp < NULL)
    {
        perror("Error localhost");
        exit(-1);
    }
    
    memcpy((char *)&server_address.sin_addr, (char *)hp -> h_addr, hp -> h_length); */

    // connection
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)))
    {
        perror("Error localhost");
        exit(-1);
    }

    printf("Siamo connessi al server\n");
    
    messaggio.value = 10;
    strcpy(messaggio.tipo, "Ciao");
    if(send(client_fd,&messaggio,sizeof(messaggio), 0) < 0)
    {
        perror("Send error");
        exit(-1);
    }
    printf("Messaggio inviato con successo.\n");



    return 0;
}
