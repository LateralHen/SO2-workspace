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