#include <stdio.h>
#include <ctype.h> // Per isdigit()

int is_numeric(char *str) {

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) {
            return 0; // Se un carattere non è numerico, ritorna falso
            }
    }
    
    
/*     for (char *ptr = str; *ptr != '\0'; ++ptr) {
        if (!isdigit((unsigned char)*ptr)) {
            return 0; // Se un carattere non è numerico, ritorna falso
        }
    } */

    return 1; // Tutti i caratteri sono numerici
}

int main() {
    char *test1 = "12345";
    char *test2 = "12a45";


    printf("Test1: %s -> %s\n", test1, is_numeric(test1) ? "Numerico" : "Non numerico");
    printf("Test2: %s -> %s\n", test2, is_numeric(test2) ? "Numerico" : "Non numerico");


    return 0;
}
