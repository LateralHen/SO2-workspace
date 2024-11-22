#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

void listDirectory(const char *path) {
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL) {
        perror("Errore nell'aprire la directory");
        return;
    }

    printf("Contenuto della directory '%s':\n", path);
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

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
