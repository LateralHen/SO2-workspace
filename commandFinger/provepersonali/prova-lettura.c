#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc, char const *argv[])
{
    int fd = open("file.txt", O_RDONLY ); // Apri il file in lettura
    char buffer[100];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read == -1) {
        perror("Errore in lettura");
    } else {
        int i = 0;
        while (i < (sizeof(buffer)/sizeof(buffer[0])))
        {
            printf("%c", buffer[i]);
            i++;
        }
        
        
    }
    close(fd); // Chiudi il file
    return 0;
}

