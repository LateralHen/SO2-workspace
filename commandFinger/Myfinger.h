#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <utmp.h>
#include <pwd.h>
#include <time.h>
#include <stdbool.h>



void printString (char *string){
    
    int lenght = strlen(string);

    for (int i = 0; i < lenght; i++)
    {
        putchar(string[i]);       
    }
    putchar('\n');
    
}





void printS(){}

void printL(){}

void optionSelection(char *options){}
