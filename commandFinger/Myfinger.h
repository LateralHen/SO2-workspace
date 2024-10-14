#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>



void printString (char *string){
    int lenght = strlen(string);    
    for (int i = 0; i < lenght+1; i++)
    {
        putchar(string[i]);
        if (string[i] == '\0')
        {
            putchar('\n');
            break;
        }
        
    }
    
}