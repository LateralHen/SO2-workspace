#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <utmp.h>
#include <pwd.h>
#include <time.h>
#include <stdbool.h>

#define SEIMESI 15552000

bool optL = false;
bool optS = false;
bool optM = false;
bool optP = false;

void printS(char **nameUser, int totalNames);

void printL(char **nameUser, int totalNames);

void optionSelection(char *options, char **nameUser, int totalNames);


/*
serve per trovare gli gli id degli utenti specificati nel comando;
nel caso non fossero selezionati gli utenti allora prende tutti gli utenti attivi.

*/
void userSelection (char ***names, int *totalnames);

void gecos_format (char *gecos, char **words);
