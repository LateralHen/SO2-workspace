#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

void listDirectory(const char *path) {
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL) {
        perror("Errore nell'aprire la directory");
        return;
    }

    int first = 1; // Per gestire la virgola iniziale
    printf("Contenuto della directory '%s':\n", path);

    while ((entry = readdir(dir)) != NULL) {
        // Escludi i puntini
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            if (!first) {
                printf(", "); // Aggiunge una virgola prima del prossimo nome
            }
            printf("%s", entry->d_name);
            first = 0; // Dopo il primo elemento, stampa la virgola
        }
    }

    printf("\n"); // Aggiungi una nuova riga alla fine
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <percorso_directory>\n", argv[0]);
        return 1;
    }

    listDirectory(argv[1]);
    return 0;
}
