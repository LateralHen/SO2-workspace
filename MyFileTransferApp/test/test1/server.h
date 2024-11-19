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

#define MAX_CONNECTION 5
#define BUFFER_SIZE 1024

void make_directory(char *dir);

int is_numeric(char *str);

