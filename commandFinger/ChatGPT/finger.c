#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utmp.h>
#include <pwd.h>
#include "finger.h"

// Funzione per ottenere le informazioni sugli utenti locali
struct user_info *get_user_info() {
    struct utmp *utmp_entry;
    struct user_info *users = NULL;
    int user_count = 0;

    // Apriamo il file utmp per ottenere le informazioni di login
    setutent();

    while ((utmp_entry = getutent()) != NULL) {
        if (utmp_entry->ut_type == USER_PROCESS) {
            users = realloc(users, sizeof(struct user_info) * (user_count + 1));
            struct user_info *user = &users[user_count];

            // Popoliamo le informazioni dell'utente
            user->username = strdup(utmp_entry->ut_user);
            user->tty = strdup(utmp_entry->ut_line);
            
            // Otteniamo l'ora di login
            time_t login_time = utmp_entry->ut_tv.tv_sec;
            user->login_time = strdup(ctime(&login_time));

            // Simuliamo l'inattività dell'utente (per semplicità, impostiamo a "0")
            user->idle_time = strdup("0");

            // Otteniamo il nome completo dell'utente
            struct passwd *pw = getpwnam(utmp_entry->ut_user);
            if (pw && pw->pw_gecos) {
                user->full_name = strdup(pw->pw_gecos);
            } else {
                user->full_name = strdup("N/A");
            }

            user_count++;
        }
    }

    // Chiudiamo il file utmp
    endutent();

    return users;
}

// Funzione per stampare le informazioni dell'utente
void print_user_info(struct user_info *user) {
    printf("User: %s\n", user->username);
    printf("TTY: %s\n", user->tty);
    printf("Login time: %s", user->login_time);
    printf("Idle time: %s\n", user->idle_time);
    printf("Full name: %s\n\n", user->full_name);
}

int main() {
    // Otteniamo le informazioni sugli utenti locali
    struct user_info *users = get_user_info();

    if (!users) {
        printf("No users logged in.\n");
        return 0;
    }

    // Stampiamo le informazioni per ogni utente
    for (int i = 0; users[i].username != NULL; i++) {
        print_user_info(&users[i]);
    }

    // Libera la memoria allocata
    free(users);

    return 0;
}
