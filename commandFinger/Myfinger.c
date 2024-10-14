#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "Myfinger.h"


void printString();



int main(int argc, char const *argv[])
{

    for (int i = 1; i < argc; i++)
    {
        printString(argv[i]);
    }
    
    return 0;
}



