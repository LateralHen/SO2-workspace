#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "Myfinger.h"





int main(int argc, char  *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        printString(argv[i]);
    }
    printS();
    printf("\n");
    printL();

    //base per utmp
    printUtmpBase();

    //base per pwd
    // printPwdBase();
    printPwdV2();

    return 0;
    }



