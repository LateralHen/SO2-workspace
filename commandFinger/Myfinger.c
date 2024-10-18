#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "Myfinger.h"





int main(int argc, char  *argv[])
{
    char *option;
    int total_lenght = 9 ;

    // for (int i = 1; i < argc; i++)
    // {
    //     total_lenght += strlen(argv[i]);
    // }
    
    // 
    option = (char *)malloc(total_lenght+1);
    
    //inizizlizzo la stringa vuota
    option[0] = '\0';

    //se ci sono specifiche 
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strncmp(argv[i], "-", 1) == 0)
            {
                strcat(option,argv[i]);
            }
            
        }

    }

    free(option);
    





    //base per utmp
    // printUtmpBase();
    //base per pwd
    // printPwdBase();
    // printPwdV2();
    // getAllUid();

    return 0;
    }



