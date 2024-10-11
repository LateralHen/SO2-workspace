# LISTA 

Il comando finger fornisce informazioni sugli utenti del sistema, come:
- Nome completo
- Nome utente (username)
- Ora di login
- Durata della sessione
- Terminale usato
- Attività recente

# STRUTTURA DEL PROGETTO 
1. file header finger.h  
   - dichiarazione delle variabili
   - prototipi delle funzioni
2. file d'implementazione finger.c
   - Implementazione delle funzioni del file header
   - funzione main principale
   - implementazione delle funzioni per raccogliere e visualizzare le informazioni sugli utenti locali

# LIB
   - controlla le uso delle seguenti librerie:
     - #include <stdio.h>
     - #include <stdlib.h>
     - #include <string.h>
     - #include <time.h>
     - #include <utmp.h>
     - #include <pwd.h>
     - #include "finger.h"

   Ecco una spiegazione dettagliata di ciascuna delle librerie:

   ### 1. **`#include <stdio.h>`**
      La libreria **`stdio.h`** (Standard Input Output) fornisce funzioni di input/output standard in C. Contiene dichiarazioni per le operazioni più comuni, come la lettura e scrittura su console o file.
      - Funzioni principali:
      - **`printf()`**: stampa un output sulla console.
      - **`scanf()`**: legge un input da console.
      - **`fopen()`**, **`fclose()`**, **`fread()`**, **`fwrite()`**: manipolazione di file.
      
   ### 2. **`#include <stdlib.h>`**
      La libreria **`stdlib.h`** (Standard Library) fornisce funzioni di utilità generali per la gestione della memoria, la conversione di tipi e per generare numeri casuali.
      - Funzioni principali:
      - **`malloc()`**, **`free()`**: allocazione e deallocazione dinamica della memoria.
      - **`exit()`**: termina l'esecuzione di un programma.
      - **`atoi()`**, **`atof()`**: conversione di stringhe in numeri (interi e float).
      - **`rand()`**, **`srand()`**: generazione di numeri casuali.
      
   ### 3. **`#include <string.h>`**
      La libreria **`string.h`** contiene funzioni per la manipolazione di stringhe. Queste funzioni permettono di lavorare con array di caratteri (che rappresentano stringhe in C).
      - Funzioni principali:
      - **`strlen()`**: restituisce la lunghezza di una stringa.
      - **`strcpy()`**, **`strncpy()`**: copia una stringa.
      - **`strcmp()`**, **`strncmp()`**: confronta stringhe.
      - **`strcat()`**, **`strncat()`**: concatena stringhe.
      - **`memcpy()`**, **`memset()`**: operazioni su blocchi di memoria.

   ### 4. **`#include <time.h>`**
      La libreria **`time.h`** fornisce funzioni per gestire il tempo e le date. È utilizzata per ottenere e manipolare il tempo di sistema.
      - Funzioni principali:
      - **`time()`**: restituisce il tempo corrente in secondi a partire dall'epoca (1 gennaio 1970).
      - **`clock()`**: restituisce il tempo di processore consumato.
      - **`difftime()`**: calcola la differenza tra due valori di tempo.
      - **`strftime()`**: formatta date e orari in stringhe.

   ### 5. **`#include <utmp.h>`**
      La libreria **`utmp.h`** viene utilizzata per interfacciarsi con i file di sistema che registrano gli utenti connessi e le informazioni su di loro. Questa libreria è particolarmente utile su sistemi Unix-like per leggere dati su login degli utenti.
      - Funzioni principali:
      - **`utmpname()`**: specifica il file utmp da leggere.
      - **`getutent()`**, **`getutxent()`**: legge le voci del file utmp/utmpx.
      - **`pututline()`**: scrive una voce nel file utmp.

   ### 6. **`#include <pwd.h>`**
      La libreria **`pwd.h`** fornisce l'accesso alle informazioni sull'utente memorizzate nel file `/etc/passwd` su sistemi Unix-like. Questo file contiene dettagli sugli account utente, come il nome utente, UID, directory home e shell predefinita.
      - Funzioni principali:
      - **`getpwuid()`**: restituisce la struttura `passwd` per un determinato UID.
      - **`getpwnam()`**: restituisce la struttura `passwd` per un determinato nome utente.
      - **`getpwent()`**: legge una voce dal database degli utenti.

   Ogni libreria è specializzata per operazioni diverse e ti permette di gestire input/output, manipolazione della memoria, gestione di stringhe, data e ora, o accesso a informazioni sugli utenti di sistema.