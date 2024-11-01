#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <utmp.h>
#include <pwd.h>
#include <time.h>
#include <stdbool.h>



void printString (char *string){
    
    int lenght = strlen(string);

    for (int i = 0; i < lenght; i++)
    {
        putchar(string[i]);       
    }
    putchar('\n');
    
}





void printS(){

    struct passwd *pwd;
    struct utmp *ut;
    int utmp_file;
    int pwd_file;

    printf("***PRINT SMALL***\n");
    printf("Login\tName\tTty\tIdle\tLogin time\tOffice\t Office phone\n");
    setutent();
    while ((ut= getutent()) != NULL )
    {
        if (ut->ut_type == USER_PROCESS)
        {
            pwd = getpwnam(ut ->ut_user);
            if (pwd == NULL)
            {
                printf("Informazioni su %s non trovate in passwd", ut->ut_user);
            }
            //username
            printf("%s\t", ut -> ut_user );
            
            //Real Name
            printf("%s\t", pwd->pw_gecos);

            // Terminal
            printf("%s\t", ut ->ut_line);

            // Idle Time
            printf(" \t");

            // Login time

            // recupero i secondi passati dal login 
            time_t login_time_sec = ut -> ut_tv.tv_sec;
            // trasformo i secondi in struct tm, mi servirà per 
            struct tm *login_time_tm = localtime(&login_time_sec);
            char login_time_format[80];

             strftime(login_time_format,sizeof(login_time_format), "%B-%d %H:%M", login_time_tm);

            printf("%s\t",login_time_format);

            // Office
            printf("\t");

            // Office phone
            printf("\n");


        }
        
    }
    endutent();







/*     
    // leggo il file utmp dove ci sono le info di tutti gli utenti
    utmp_file = open(_PATH_UTMP,O_RDONLY);

    //nel caso l'apertura desse errore
    if (utmp_file == -1)
    {
        perror("errore nella lettura del file utmp");
    }

    // stampo l'intestazione della tabella
    printf("Login\tName\tTty\tIdle\tLogin time\tOffice\t Office phone\n");

    //leggo tutti gli utenti nel file usando un while loop
    while (read(utmp_file,&current_record, sizeof(current_record)) == sizeof(current_record))
    {
        
        if (current_record.ut_type == USER_PROCESS)
        {
            //prendo le info dell'utente preso in considerazione
            pwd = getpwnam(current_record.ut_user);
            
            
            
            //username
            printf("%s\t", current_record.ut_pid );
            
            //Real Name
            printf("%s\t", pwd->pw_gecos);

            // Terminal
            printf("\t", current_record.ut_line);

            // Idle Time
            printf("\t");

            // Login time

            // recupero i secondi passati dal login 
            time_t login_time_sec = current_record.ut_tv.tv_sec;
            // trasformo i secondi in struct tm, mi servirà per 
            struct tm *login_time_tm = localtime(&login_time_sec);
            char login_time_format[80];

             strftime(login_time_format,sizeof(login_time_format), "%B-%d %H:%M", login_time_tm);

            printf("%s\t",login_time_format);

            // Office
            printf("\t");

            // Office phone
            printf("\n");
            
            
            
            
        }
        
    } */
    
}

void printL(){

    printf("***PRINT LARGE***\n");
    printf("Login:[]\t\tName:[]\n");
    printf("Directory:[]\t\tShell:[]\n");
    printf("Login time:[]\n");
    printf("Mail:[]\n");
    printf("plan:[]\n");
    printf("\n");
    
}

void optionSelection(char *options){
    
    bool optL = false;
    bool optS = false;
    bool optM = false;
    bool optP = false;
    int valida = 1; //flag per il corretto inserimento delle opzioni


    // visualizza sul terminale che opzioni ho scritto sul terminale
    printString(options);


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
        printf("le opzioni sono corrette\n");
        if (optL)
        {
            printL();
        }else
        {
            printS();
        }
        
        
    }else
    {
        printf("opzioni errate, riprovare");
    }
    
    

}
