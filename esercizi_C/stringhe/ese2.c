#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    //inizializzazione varibili 
    char buffer[1024];
    ssize_t readbytes;

    readbytes = read(STDIN_FILENO, buffer, 1024-1);
    if (readbytes == -1){
        perror("Errore lettura"); // serve per risolvere nel caso ci fosse errore nella lettura
        return 1;
    }
    buffer[readbytes] = '\0'; // aggiungo \0 per far diventare buffer una stringa valida
    
    for (int i = 0; i < readbytes; i++)
    {
        putchar(buffer[i]);
        if (buffer[i] == '\n')
        {
            break;
        }
        
    }
    
    return 0;
}

