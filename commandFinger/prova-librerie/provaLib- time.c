#include <stdio.h>
#include <time.h>
#include <string.h>

// int main() {
//     time_t current_time;
    
//     // Ottieni il tempo corrente
//     current_time = time(NULL);
    
//     if (current_time == -1) {
//         printf("Errore nell'ottenere il tempo corrente.\n");
//         return 1;
//     }
    
//     // Converti il tempo in formato leggibile
//     printf("Tempo corrente (secondi da Epoch): %ld\n", current_time);
//     printf("Tempo corrente (formato leggibile): %s", ctime(&current_time));
    
//     return 0;
// }





void main(){
    time_t currentTime;
    clock_t clocktime;

    currentTime = time(NULL);
    char *gigi = ctime(&currentTime);
    char pluto[80];
    strftime(pluto, sizeof(pluto), "%d-%m-%Y", localtime(&currentTime));

    clocktime = clock();
    char pippi = 


    printf("ciao\n");
    printf("Tempo : %ld \n ", currentTime);
    printf("Tempo leggibile (con funzione diretta) :  %s \n", ctime(&currentTime));
    printf("tempo leggibile (con varibile definita): %s", gigi);

    printf("tempo di clock : %ld \n", clocktime );
    printf("ma cos'è sta roba: %s \n ", pluto);
    return ;
}