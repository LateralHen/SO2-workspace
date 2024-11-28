#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

char *list_directory_as_string(const char *path) {
    struct dirent *entry;
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Errore nell'aprire la directory");
        return NULL;
    }

    size_t buffer_size = 256;
    char *result = malloc(buffer_size);
    if (result == NULL) {
        perror("Errore nell'allocazione della memoria");
        closedir(dir);
        return NULL;
    }

    result[0] = '\0'; // Inizializza la stringa
    size_t current_length = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            size_t name_length = strlen(entry->d_name);
            if (current_length + name_length + 2 >= buffer_size) { // +2 per separatore e terminatore
                buffer_size *= 2;
                result = realloc(result, buffer_size);
                if (result == NULL) {
                    perror("Errore nel ridimensionamento della memoria");
                    closedir(dir);
                    return NULL;
                }
            }

            if (current_length > 0) {
                strcat(result, ", ");
                current_length += 2;
            }
            strcat(result, entry->d_name);
            current_length += name_length;
        }
    }

    closedir(dir);
    return result;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <directory_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *directory_content = list_directory_as_string(argv[1]);
    if (directory_content) {
        printf("Contenuto di '%s': %s\n", argv[1], directory_content);
        free(directory_content);
    }

    return EXIT_SUCCESS;
}
