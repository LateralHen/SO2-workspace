#include <stdio.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>

// questa funzione serve per strutturare il messaggio da visualizzare a schermo 
void print_utmp_record(struct utmp *record) {
    printf("User: %s\n", record->ut_user);
    printf("Terminal: %s\n", record->ut_line);
    printf("Host: %s\n", record->ut_host);
    printf("Login time: %d\n", record->ut_tv.tv_sec);
    printf("Session type: %d\n\n", record->ut_type);
}

int main() {
    struct utmp current_record;
    int utmp_fd;

    // Apri il file utmp
    utmp_fd = open(_PATH_UTMP, O_RDONLY);
    if (utmp_fd == -1) {
        perror("Error opening utmp file");
        return 1;
    }

    // Leggi e stampa le voci dal file utmp
    while (read(utmp_fd, &current_record, sizeof(struct utmp)) == sizeof(struct utmp)) {
        if (current_record.ut_type == USER_PROCESS) { // Solo utenti attivi
            print_utmp_record(&current_record);
        }
    }

    close(utmp_fd);
    return 0;
}
