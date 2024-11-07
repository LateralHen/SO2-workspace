#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "Myfinger.h"





int main(int argc, char  *argv[])
{
    char option[argc];
    char *names[argc];
    int totalNames = 0;
    int total_lenght = 9 ;

    //inizizlizzo la stringa vuota
    option[0] = '\0';

    //se nella riga di comando ho messo delle opzioni oppure ho cercato un utente specifico
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strncmp(argv[i], "-", 1) == 0)
            {
                char *subStr = argv[i] + 1;  // Puntiamo al secondo carattere dell'array
                strcat(option,subStr);
            }
            else
            {
                names[totalNames] = argv[i];
                totalNames ++ ; 
            }
            
        }
        optionSelection(option);
        for (int i = 0; i < totalNames; i++)
        {
            printString(names[i]);
        }
        
    }
    else
    {
        printf("Nessuna opzione\n");
        printS();
    }
    
    
    // free(option);
    

    return 0;
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
            printf("%s \t", ut -> ut_user );
            
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
    endutent();


}


void printL(){

    struct passwd *pwd;
    struct utmp *ut;
    int utmp_file;
    int pwd_file;


    /*
    printf("***PRINT LARGE***\n");
    printf("Login:[]\t\tName:[]\n");
    printf("Directory:[]\t\tShell:[]\n");
    printf("Login time:[]\n");
    printf("Mail:[]\n");
    printf("plan:[]\n");
    printf("\n");
    */

    while ((ut = getutent()) != NULL)
    {
        if (ut->ut_type == USER_PROCESS)
        {
            pwd = getpwnam(ut ->ut_user);
            if (pwd != NULL)
            {
                // Login
                printf("Login: %s \t\t", ut -> ut_name);

                // Name
                printf("Name: %s\n", pwd -> pw_gecos);

                // Directory
                printf("Directory: %s\t" , pwd -> pw_dir);
                
                // Shell
                printf("Shell: %s\n" , pwd -> pw_shell);

                // Login time
                // recupero i secondi passati dal login 
                time_t login_time_sec = ut -> ut_tv.tv_sec;
                // trasformo i secondi in struct tm, mi servirà per 
                struct tm *login_time_tm = localtime(&login_time_sec);
                time_t t = time(NULL);
                char login_time_format[80];
                if ( (t - login_time_sec) < 15552000 )
                {   
                    strftime(login_time_format,sizeof(login_time_format), "%a %b %d %H:%M", login_time_tm);
                }
                else
                {
                    strftime(login_time_format,sizeof(login_time_format), "%b %d %Y", login_time_tm);
                }
                printf("On since %s (CET) on %s\n", login_time_format, ut -> ut_host);
                
                
                
                // Mail
                printf("Mail:[]\n");
                // plan
                printf("plan:[]\n");
                printf("\n");
            }
            else
            {
               printf("Informazioni su %s non trovate in passwd", ut->ut_user);
            }
            
            
        }
        
    }


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



    