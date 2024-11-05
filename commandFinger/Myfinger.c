#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "Myfinger.h"





int main(int argc, char  *argv[])
{
    char option[argc];
    char *names[argc];
    int totalNames = 0;
    int total_lenght = 9 ;

    //inizizlizzo la stringa vuota
    option[0] = '\0';

    //se nella riga di comando ho messo delle opzioni oppure ho cercato un utente specifico
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strncmp(argv[i], "-", 1) == 0)
            {
                char *subStr = argv[i] + 1;  // Puntiamo al secondo carattere dell'array
                strcat(option,subStr);
            }
            else
            {
                names[totalNames] = argv[i];
                totalNames ++ ; 
            }
            
        }
        optionSelection(option);
        for (int i = 0; i < totalNames; i++)
        {
            printString(names[i]);
        }
        
    }
    else
    {
        printf("Nessuna opzione\n");
        printS();
    }
    
    
    // free(option);
    

    return 0;
    }




    //base per utmp
    // printUtmpBase();
    //base per pwd
    // printPwdBase();
    // printPwdV2();
    // getAllUid();



    