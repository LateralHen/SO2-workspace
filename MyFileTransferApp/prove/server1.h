#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

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
void fix_path(char *full_path, const char *clientDir);
