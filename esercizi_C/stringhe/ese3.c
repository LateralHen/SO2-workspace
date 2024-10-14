#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    /*
    argc => numero di argomenti passati

    *argv[] => array di array di caratteri;
    in C non esiste il tipo stringa => le parole vengono interpretate come array di caratteri;

    PER STAMPARE LE STRINGHE ==> crea un ciclo e prendi carattere per carattere;
    */
    
    for (int i = 1; i < argc; i++)
    {
        int s = 0;
        while (1)
        {
            putchar(argv[i][s]);
            if (argv[i][s] == '\0')
            {
                putchar('\n');
                break;
            }
            s++;
        }
        
        
    }
    
    return 0;
}
