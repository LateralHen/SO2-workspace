#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){
int opt;

while ((opt = getopt(argc, argv, "s:l:p:m:")) != -1){
    
    switch (opt){
    case 's':
        printf("Opzione -s con argomento: %s \n", optarg);
        break;
    case 'l':
        printf("Opzione -l con argomento: %s \n", optarg);;
        break;
    case 'p':
        printf("Opzione -p con argomento: %s \n", optarg);
        break;
    case 'm':
        printf("Opzione -p con argomento: %s \n", optarg);
        break;
    default:
        printf("caso di default \n");
        break;
    }
}





return 0;
};