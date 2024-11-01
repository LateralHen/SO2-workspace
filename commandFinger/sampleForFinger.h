#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <utmp.h>
#include <pwd.h>



void printUtmpBase(){
    struct utmp current_record;
    int utmp_file;

    // apriamo il file utmp
    utmp_file = open(_PATH_UTMP,O_RDONLY);

    if (utmp_file == -1)
    {
        perror("errore nella lettura del file utmp");
    }
    
    //leggiamo ogni record nel file utmp
    while (read(utmp_file,&current_record, sizeof(current_record)) == sizeof(current_record))
    {
        if (current_record.ut_type == USER_PROCESS)
        {
            printf("****STAMPA DA UTMP****\n");            
            printf("Utente: %s\n", current_record.ut_user);
            printf("Linea di terminale: %s\n", current_record.ut_line);
            printf("Host remoto: %s\n", current_record.ut_host);
            printf("\n");
        }
        
    }
    close(utmp_file);
    
}

void printPwdBase(){

    /* La libreria pwd.h fornisce un'interfaccia per accedere alle informazioni sugli
     utenti, come definite nel file /etc/passwd. */

    struct passwd *pw;
    uid_t uid;

    //ottieni l'UID dell'utente corrente
    uid = geteuid();

    //ottieni le informazioni dell'utente in base all'UID
    pw = getpwuid(uid);

    if (pw)
    {
        
        printf("Nome utente: %s\n", pw->pw_name);
        printf("Directory home: %s\n", pw->pw_dir);
        printf("Shell: %s\n", pw->pw_shell);
    } else {
        perror("Errore nel recuperare le info dell'utente");
    }
    

    /* 
    FUNZIONI UTILI:
        getpwuid(): Restituisce una struttura passwd basata su un UID.
        getpwnam(): Restituisce una struttura passwd basata su un nome utente.  
    */
}

void printPwdV2(){

    struct passwd *pw;
    uid_t uid;

    //ottieni l'UID dell'utente corrente
    uid = geteuid();

    //ottieni le informazioni dell'utente in base all'UID
    pw = getpwuid(uid);

    if (pw)
    {
        printf("****STAMPA DA PWD****\n");
        printf("nome: %s\n",pw->pw_name);
        printf("password: %s\n",pw->pw_passwd);
        printf("Nome reale: %s\n",pw->pw_gecos);
        printf("directory: %s\n",pw->pw_dir);
        printf("shell utilizzata: %s\n",pw->pw_shell);
        
    } else {
        perror("Errore nel recuperare le info dell'utente");

    }
}


void getAllUid(){
        struct passwd *pw;

    // Usa getpwent() per ottenere ogni entry del file /etc/passwd
    while ((pw = getpwent()) != NULL) {
        if (strcmp(pw->pw_name,"emiliano")==0)
        {
        printf("Utente: %s; \n",  pw->pw_name);
        printf("passwd: %s \n", pw->pw_passwd);
        printf("UID: %u\n", pw->pw_uid);
        printf("GID: %u \n", pw ->pw_gid);
        printf("Terminal: %s\n", pw -> pw_shell);
        printf("Real name: %s\n", pw->pw_gecos);
        printf("Home directory : %s\n", pw->pw_dir);
        printf("Shell: %s \n", pw->pw_shell);
        printf("--------------------\n");
        }
        

    }

    // Chiudi l'accesso al database degli utenti
    endpwent();
}