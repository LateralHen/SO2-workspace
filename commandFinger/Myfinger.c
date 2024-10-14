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
    
    return 0;
}



