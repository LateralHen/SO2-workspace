#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int opt;
    
    // Definire le opzioni che ci interessano
    while ((opt = getopt(argc, argv, "a:b:c")) != -1) {
        switch (opt) {
            case 'a':
                printf("Opzione -a con argomento: %s\n", optarg);
                break;
            case 'b':
                printf("Opzione -b con argomento: %s\n", optarg);
                break;
            case 'c':
                printf("Opzione -c senza argomento\n");
                break;
            case '?':
                printf("Opzione sconosciuta: %c\n", optopt);
                break;
            default:
                break;
        }
    }
    
    // Stampa gli argomenti rimanenti non gestiti
    for (int index = optind; index < argc; index++) {
        printf("Argomento non gestito: %s\n", argv[index]);
    }

    return 0;
}

// per prova usa questo comando bash:
// ./commandFinger/ChatGPT/input-comando -a arg1 -b arg2 -c arg3