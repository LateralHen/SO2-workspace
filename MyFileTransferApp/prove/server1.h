#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define MAX_CONNECTION 5

int is_numeric(char *str);

void receive_file(FILE *file, int socket);

void send_file(FILE *file, int socket);

void processing( int client_fd, char command, char *path);

void split_path(const char *full_path, char *path, char *filename);

void receive_file(FILE *file, int socket);

void send_file(FILE *file, int socket);