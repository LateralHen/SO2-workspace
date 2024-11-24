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

    char *local_path;
    char *remote_path;
} InfoCon;

const char *base_dir = "clientDir";

void receive_file(const char *path, int socket);

void send_file(const char *path, int socket);

void create_file(char *full_path);

void split_path(const char *full_path, char *path, char *filename);

void fix_path(const char *input_path, char *output_path);