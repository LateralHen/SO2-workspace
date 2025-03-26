#include "Myfinger.h"

int main(int argc, char  *argv[])
{
    char option[argc];
    char **names = NULL;
    int totalNames = 0;

    //inizizlizzo la stringa vuota
    option[0] = '\0';

    //se nella riga di comando ho messo delle opzioni oppure ho cercato un utente specifico
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strncmp(argv[i], "-", 1) == 0)
            {
                char *subStr = argv[i] + 1;  // Puntiamo al secondo carattere dell'array per togliere il - che è davanti a tutte le opzioni
                strcat(option,subStr);
            }
            else
            {
                // Allocazione della memoria per il nome
                names = (char **)realloc(names, (totalNames + 1) * sizeof(char *));
                names[totalNames] = strdup(argv[i]);

                totalNames ++ ; 
            }
            
        }
        userSelection(&names, &totalNames);
        optionSelection(option, names, totalNames);
        
    }
    // quando non viene specificato nessun nome o opzione
    else
    {
        userSelection(&names, &totalNames);
        printS(names, totalNames);
               

    }
    for (int i = 0; i < totalNames; i++) {
        free(names[i]);
    }
    free(names);


    return 0;
    }

void printS(char **nameUser, int totalNames){

    struct passwd *pwd;
    struct utmp *ut;


    printf("***PRINT SMALL***\n");
    printf("Login\tName\tTty\tIdle\tLogin time\tOffice\t Office phone\n");

    for (int i = 0; i < totalNames; i++)
    {
        setutent();
        while ((ut= getutent()) != NULL )
        {
            if (ut->ut_type == USER_PROCESS)
            {
                //printf("ut:%s \t name:%s\n",ut -> ut_user, nameUser[i]);
                char utente[32];
                strncpy(utente, ut->ut_user, sizeof(utente) - 1);
                utente[sizeof(utente) - 1] = '\0';

                if (strcmp(utente,nameUser[i])==0)
                {
                    pwd = getpwnam(ut ->ut_user);
                    if (pwd == NULL)
                    {
                        printf("Informazioni su %s non trovate in passwd", ut->ut_user);
                    }
                    //username
                    printf("%s \t", ut -> ut_user );
                    
                    //Real Name
                    char **words = NULL;
                    words = (char **)malloc(10 * sizeof(char *));
                    gecos_format(pwd->pw_gecos, words);

                    printf("%s\t", words[0]);

                    for (int w = 0; w < 10 && words[w] != NULL; w++) {
                        free(words[w]);
                    }
                    free(words);
                    

                    // Terminal
                    printf("%s\t", ut ->ut_line);

                    // Idle Time
                    char tty_path[64];
                    struct stat tty_stat;

                    sprintf(tty_path, "/dev/%s", ut->ut_line);
                    if (stat(tty_path, &tty_stat) == 0) {
                        time_t now = time(NULL);
                        int idle = now - tty_stat.st_atime;

                        if (idle < 60)
                            printf("0 min\t");
                        else if (idle < 3600)
                            printf("%d min\t", idle / 60);
                        else
                            printf("%d:%02d\t", idle / 3600, (idle % 3600) / 60);
                    } else {
                        printf("n/a\t");
                    }


                    // Login time

                    // recupero i secondi passati dal login 
                    time_t login_time_sec = ut -> ut_tv.tv_sec;
                    // trasformo i secondi in struct tm, mi servirà per 
                    struct tm *login_time_tm = localtime(&login_time_sec);
                    time_t t = time(NULL);
                    char login_time_format[80];
                    if ( (t - login_time_sec) < 15552000 )
                    {   
                        strftime(login_time_format,sizeof(login_time_format), "%b %d %H:%M", login_time_tm);
                    }
                    else
                    {
                        strftime(login_time_format,sizeof(login_time_format), "%b %d %Y", login_time_tm);
                    }
                    
                    printf("%s (%s)\t",login_time_format, ut -> ut_host);

                    // Office
                    printf("\t");

                    // Office phone
                    printf("\n");
                    
                }
            }
        }
        endutent();
    }
}

void printL(char **nameUser, int totalNames){

    struct passwd *pwd;
    struct utmp *ut;

    /* 
    prendo i nomi da nameUser
    ciclo su pwd i nomi presi:
        stampo le info prese da pwd del singolo utente
        ciclo su utmp:
            stampo le info prese da utmp per tutti gli utenti con quel nome

    */

   setpwent();
    for (int i = 0; i < totalNames ; i++)
    {
        pwd = getpwnam(nameUser[i]);
        if (pwd != NULL)
        {
            // Login
            printf("Login: %s \t\t", pwd -> pw_name);

            // Name
            char **words = NULL;
            words = (char **)malloc(10 * sizeof(char *));

            gecos_format(pwd->pw_gecos, words);

            printf("Name: %s\n", words[0]);

            //office
            printf("Office: %s, %s\t", words[1],words[2]);

            //home
            printf("Home Phone: %s\n", words[3]);

            for (int w = 0; w < 10 && words[w] != NULL; w++) {
                free(words[w]);
            }
            free(words);            

            // Directory
            printf("Directory: %s\t" , pwd -> pw_dir);
            
            // Shell
            printf("Shell: %s\n" , pwd -> pw_shell);
            
            setutent();
            while ((ut = getutent()) != NULL)
            {
                if (ut->ut_type == USER_PROCESS)
                {
                    // Login time
                    
                    // recupero i secondi passati dal login 
                    time_t login_time_sec = ut -> ut_tv.tv_sec;

                    // trasformo i secondi in struct tm, mi servirà per 
                    struct tm *login_time_tm = localtime(&login_time_sec);
                    time_t t = time(NULL);
                    char login_time_format[80];
                    
                    if ( (t - login_time_sec) < SEIMESI )
                    {   
                        strftime(login_time_format,sizeof(login_time_format), "%a %b %d %H:%M", login_time_tm);
                    }
                    else
                    {
                        strftime(login_time_format,sizeof(login_time_format), "%b %d %Y", login_time_tm);
                    }
                    printf("On since %s (CET) on %s\n", login_time_format, ut -> ut_host); 
                }
                
            }
            endutent();            
            
            //Mail
            char mailPath[256];
            sprintf(mailPath, "/var/mail/%s",pwd-> pw_name);
            FILE *mailFile = fopen(mailPath, "r");
            if (mailFile)
            {
                printf("Il file esiste e è stato aperto.\n");
                fclose(mailFile);
            }
            else
            {
                printf("No mail.\n");
            }
            
            
            if (!optP)
            {
                //plan
                char planPath[256];
                sprintf(planPath, "%s/.plan",pwd-> pw_dir);

                FILE *planFile = fopen(planPath, "r");
                if (planFile) {
                    printf("Plan:\n");
                    char buffer[256];
                    while (fgets(buffer, sizeof(buffer), planFile)) {
                        printf("%s", buffer);
                    }
                    fclose(planFile);
                } else {
                    printf("No Plan.\n");
                }
                
                //project
                char projectPath[256];
                sprintf(projectPath, "%s/.project",pwd-> pw_dir);

                FILE *projectFile = fopen(projectPath, "r");
                if (projectFile) {
                    printf("Project:\n");
                    char buffer[256];
                    while (fgets(buffer, sizeof(buffer), projectFile)) {
                        printf("%s", buffer);
                    }
                    fclose(projectFile);
                } else {
                    printf("No P.\n");
                }
                
                    
                //pgpkey
                char pgpkeyPath[256];
                sprintf(pgpkeyPath, "%s/.pgpkey",pwd-> pw_dir);

                FILE *pgpkeyFile = fopen(pgpkeyPath, "r");
                if (pgpkeyFile) {
                    printf("PGP key:\n");
                    char buffer[256];
                    while (fgets(buffer, sizeof(buffer), pgpkeyFile)) {
                        printf("%s", buffer);
                    }
                    fclose(pgpkeyFile);
                } else {
                    printf("No PGP key.\n");
                }
            }
            
        } 
        endpwent();     
    }
}

void optionSelection(char *options, char **nameUser, int totalNames){
        
    int valida = 1; //flag per il corretto inserimento delle opzioni

    for (int i = 0; options[i] != '\0'; i++)
    {
        switch (options[i])
        {
        case 'l':
            optL = true;
            break;

        case 's':
            optS = true;    
            break;
        
        case 'm':
            optM = true;
            break;

        case 'p':
            optP = true;
            break;
        
        default:
            // carattere non valido trovato
            valida = 0;
            break;
        }

        if (!valida)
        {
            break; // uscita dal ciclo
        }
    }

    if (valida)
    {
        if (optL)
        {
            printL(nameUser, totalNames);
        }else
        {
            printS(nameUser, totalNames);
        }
        
        
    }else
    {
        printf("opzioni errate, riprovare\n");
    }
    
    

}

void userSelection(char ***names, int *totalnames) {
    
    // Se non sono stati specificati dei nomi utente
    if (*totalnames == 0) {

        struct utmp *ut;
        int i = 0;

        setutent();

        // Inizializza un array dinamico per i nomi degli utenti
        *names = (char **)malloc(100 * sizeof(char *)); // Allocazione per 100 utenti
        if (*names == NULL) {
            perror("Memory allocation failed");
            return;
            
        }


        while ((ut = getutent()) != NULL) {

            int valida = 0; // flag per il controllo dei duplicati

            if (ut->ut_type == USER_PROCESS) {

                char utente[32];
                strncpy(utente, ut->ut_user, sizeof(utente) - 1);
                utente[sizeof(utente) - 1] = '\0';

                for (int j = 0; j < i; j++)
                {
                    if (strcmp(utente,*names[j]) == 0)
                    {
                        valida = 1; // duplicato trovato
                        break;
                    }
                    
                }
                
                if (valida)
                {
                    continue;
                }
                
                // Alloca memoria per il nome dell'utente e lo copia nell'array names
                *names[i] = strdup(utente);
                
                if (*names[i] == NULL) {
                    perror("Memory allocation failed for username");
                    break;
                } 
                
                i++;
            }
        }
        

        *totalnames = i;
        

        endutent();
    } 

    /* 
    Nel caso in cui siano stati specificati dei nomi
    devo controllare che i nomi utenti specificati sono nomi di utenti presenti
    */

    else {

        struct passwd *pwd;

        for (int i = 0; i < *totalnames; i++)
        {
            int valida = 0 ; // flag per vedere se ho trovato il nome
            setpwent();

            while ((pwd = getpwent()) != NULL)
            {
                    char utente[32];
                    strncpy(utente, pwd ->pw_name, sizeof(utente) - 1);
                    utente[sizeof(utente) - 1] = '\0';
                    if (strcmp(utente,*names[i]) == 0)
                    {
                       valida = 1; // nome utente trovato
                       
                    }
            }
            endpwent();

            if (!valida)
            {
                printf("ERRORE: %s non trovato tra i nomi utenti\n", *names[i]);
            }
        }
        

    }

    
}

void gecos_format(char *gecos, char **words){
    
    char *token;
    int i = 0;
    // Ottieni il primo token
    token = strtok(gecos, ",");

    while (token != NULL) {

        words[i]= strdup(token);
        i ++;
        token = strtok(NULL, ","); // Ottieni il token successivo
    }
    return;
}