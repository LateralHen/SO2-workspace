#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

typedef struct InfoCon {
    char *server_address;
    int port;
    char command;
    char command2;

    char *local_file;
    char *remote_file;
} InfoCon;



void receive_file(FILE *file, int socket);

void send_file(FILE *file, int socket);