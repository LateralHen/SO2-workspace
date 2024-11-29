#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAX_CONNECTION 5

char *server_address = NULL;
int server_port = 0;
char *root_dir = NULL;

/**
 * Riceve un file dal client e lo salva sul server.
 * 
 * Questa funzione riceve un file dal client attraverso un socket, 
 * e lo scrive nella posizione specificata dal parametro `path`.
 * Utilizza il socket per ricevere i dati dal client e li scrive nel file.
 * 
 * @param path Percorso dove salvare il file.
 * @param socket Socket utilizzato per ricevere il file.
 */
void receive_file(const char *path, int socket);

/**
 * Invia un file a un client tramite il socket.
 * 
 * Questa funzione invia il file specificato nel parametro `path` al client 
 * attraverso il socket. I dati vengono letti dal file e inviati in pacchetti 
 * di dimensione definita da `BUFFER_SIZE`.
 * 
 * @param path Percorso del file da inviare.
 * @param socket Socket utilizzato per la trasmissione.
 */
void send_file(const char *path, int socket);

/**
 * Gestisce i comandi ricevuti dal client.
 * 
 * Questa funzione riceve un comando dal client, verifica cosa il client 
 * stia cercando di fare (ad esempio, inviare un file, ricevere un file, 
 * o ottenere una lista di file) e invoca la funzione corrispondente.
 * 
 * @param socket Socket della connessione con il client.
 */
void processing(int socket);

/**
 * Divide un percorso completo in path e filename.
 * 
 * Questa funzione prende un percorso completo di un file e lo divide in due 
 * parti: la parte del percorso senza il nome del file (`path`) e il nome del 
 * file stesso (`filename`).
 * 
 * @param full_path Percorso completo del file.
 * @param path Output: parte del percorso senza il filename.
 * @param filename Output: solo il nome del file.
 */
void split_path(const char *full_path, char *path, char *filename);

/**
 * Crea un file vuoto nel percorso specificato, se il file non esiste già.
 * 
 * Se il file non esiste nel percorso specificato, la funzione crea il file 
 * vuoto. La funzione controlla anche la struttura delle cartelle necessarie 
 * e le crea se non esistono.
 * 
 * @param full_path Percorso completo dove il file deve essere creato.
 */
void create_file(char *full_path);

/**
 * Crea una directory di root se non esiste.
 * 
 * Questa funzione verifica se la directory root esiste. Se non esiste, 
 * crea la directory con i permessi specificati. La directory root è quella 
 * in cui il server memorizza i file ricevuti dai client.
 * 
 * @param dir Directory da creare.
 */
void make_root(char *dir);

/**
 * Modifica un percorso per adattarlo alla struttura serverDir.
 * 
 * Questa funzione modifica il percorso in modo che si adatti alla struttura 
 * di directory del server. Se il percorso inizia con un nome di directory 
 * del client, questo viene rimosso. Se il percorso non include la root del 
 * server, questa viene aggiunta.
 * 
 * @param full_path Percorso da modificare.
 * @param clientDir directory del client da rimuovere.
 */
void fix_path(char *path, char *full_path);

/**
 * Riceve un messaggio dal client e lo salva nel buffer.
 * 
 * Questa funzione legge i dati inviati dal client attraverso il socket, 
 * e li copia nel buffer passato come parametro. La funzione riceve 
 * i dati fino a una lunghezza specificata.
 * 
 * @param socket Socket utilizzato per ricevere il messaggio.
 * @param buffer Buffer dove memorizzare i dati ricevuti.
 * @param length Lunghezza del buffer, ovvero il numero massimo di byte 
 *               da ricevere.
 * @return Numero di byte ricevuti, o -1 in caso di errore.
 */
ssize_t receive_message(int socket, void *buffer, size_t length);

/**
 * Invia un messaggio al client tramite il socket.
 * 
 * Questa funzione invia i dati contenuti nel messaggio attraverso il socket. 
 * I dati sono inviati in blocchi fino a una lunghezza specificata nel parametro 
 * `length`. La funzione gestisce l'invio del messaggio in modo che tutto 
 * il contenuto venga inviato correttamente.
 * 
 * @param socket Socket utilizzato per inviare il messaggio.
 * @param message Messaggio da inviare al client.
 * @param length Lunghezza del messaggio da inviare.
 * @return Numero di byte inviati, o -1 in caso di errore.
 */
int send_message(int socket, const void *message, size_t length);

/**
 * Cerca la directory del path e effettua un listing di essa (nel caso esista).
 * 
 * @param path Il percorso della directory da analizzare.
 *             Deve essere un percorso valido e accessibile.
 * 
 * @return Un puntatore a una stringa allocata dinamicamente contenente i nomi
 *         degli elementi nella directory, separati da virgole. 
 *         Se si verifica un errore (ad esempio directory non accessibile o 
 *         errore di allocazione della memoria), la funzione restituisce `NULL`.
 * 
 * @note La stringa restituita deve essere liberata manualmente dal chiamante
 *       utilizzando `free` per evitare perdite di memoria.
 * 
 * @warning La funzione non gestisce directory molto grandi in maniera ottimale.
 *          Potrebbe essere necessario aumentare il buffer dinamicamente più volte.
 */
char *listing_directory(char *path);