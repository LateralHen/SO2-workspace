#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <utmp.h>
#include <pwd.h>
#include <time.h>
#include <stdbool.h>

    bool optL = false;
    bool optS = false;
    bool optM = false;
    bool optP = false;



void printString (char *string){
    
    int lenght = strlen(string);

    for (int i = 0; i < lenght; i++)
    {
        putchar(string[i]);       
    }
    putchar('\n');
    
}





void printS();

void printL();

void optionSelection(char *options);


/*
serve per trovare gli gli id degli utenti specificati nel comando;
nel caso non fossero selezionati gli utenti allora prende tutti gli utenti attivi.

*/
void userSelection (char **names, int *totalnames);

void gecos_format (char *gecos);
