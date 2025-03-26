#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <utmp.h>
#include <pwd.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define SEIMESI 15552000 /**< Definisce il numero di secondi in sei mesi. */

/* Variabili globali per la gestione delle opzioni del programma */
bool optL = false; /**< Opzione per il formato dettagliato di output. */
bool optS = false; /**< Opzione per il formato breve di output. */
bool optM = false; /**< Opzione per il formato compatto (una riga per utente). */
bool optP = false; /**< Opzione per mostrare solo gli utenti loggati tramite tty. */

/**
 * @brief Stampa i dettagli degli utenti in formato breve.
 * 
 * @param nameUser Array di stringhe contenente i nomi degli utenti selezionati.
 * @param totalNames Numero totale di utenti nell'array.
 */
void printS(char **nameUser, int totalNames);

/**
 * @brief Stampa i dettagli degli utenti in formato dettagliato.
 * 
 * @param nameUser Array di stringhe contenente i nomi degli utenti selezionati.
 * @param totalNames Numero totale di utenti nell'array.
 */
void printL(char **nameUser, int totalNames);

/**
 * @brief Seleziona e applica le opzioni specificate dall'utente.
 * 
 * @param options Stringa contenente le opzioni specificate (ad esempio, "-l -s").
 * @param nameUser Array di stringhe contenente i nomi degli utenti selezionati.
 * @param totalNames Numero totale di utenti nell'array.
 */
void optionSelection(char *options, char **nameUser, int totalNames);

/**
 * @brief Seleziona gli utenti da analizzare in base ai parametri specificati.
 * 
 * Se non vengono specificati utenti, seleziona tutti gli utenti attivi.
 * 
 * @param names Puntatore a un array di stringhe che conterrà i nomi degli utenti selezionati.
 * @param totalnames Puntatore a un intero che conterrà il numero totale di utenti selezionati.
 */
void userSelection(char ***names, int *totalnames);

/**
 * @brief Converte il campo GECOS in un formato leggibile.
 * 
 * @param gecos Stringa contenente il campo GECOS dell'utente.
 * @param words Array di stringhe che conterrà i dati estratti dal GECOS.
 */
void gecos_format(char *gecos, char **words);
