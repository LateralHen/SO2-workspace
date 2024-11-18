#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
    if (argc != 7)
    {
        printf("Errore negli argomenti\n");
        printf("Uso corretto: %s -a server_address -p server_port -d ft_root_directory", argv[0]);
        exit(1);
    }
    
    return 0;
}
