#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct passwd *pwd;

    // Se non vengono forniti argomenti, stampa tutti gli utenti
    if (argc == 1) {
        while ((pwd = getpwent()) != NULL) {
            printf("Nome utente: %s\n", pwd->pw_name);
            printf("Nome completo: %s\n", pwd->pw_gecos);
            printf("Directory home: %s\n", pwd->pw_dir);
            printf("Shell: %s\n", pwd->pw_shell);
            printf("\n");
        }
        endpwent();
    }
    // Se viene fornito un nome utente, stampa le informazioni di un utente specifico
    else if (argc == 2) {
        pwd = getpwnam(argv[1]);
        if (pwd == NULL) {
            printf("Utente %s non trovato.\n", argv[1]);
            return 1;
        }
        printf("Nome utente: %s\n", pwd->pw_name);
        printf("Nome completo: %s\n", pwd->pw_gecos);
        printf("Directory home: %s\n", pwd->pw_dir);
        printf("Shell: %s\n", pwd->pw_shell);
    } else {
        printf("Uso: %s [nome utente]\n", argv[0]);
        return 1;
    }

    return 0;
}
