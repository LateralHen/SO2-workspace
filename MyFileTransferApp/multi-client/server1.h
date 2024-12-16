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
#define THREAD_POOL_SIZE 5

// Variabili globali per la connessione lato server
char *server_address = NULL;
int server_port = 0;
char *root_dir = NULL;

// Variabili globali per pool di thread
pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

int client_queue[MAX_CONNECTION];  ///< Coda di attesa per i client
int queue_front = 0, queue_rear = 0, queue_size = 0;  ///< Indici della coda di attesa

// Gestione delle risorse

/**
 * Struttura per la gestione dei mutex associati alle risorse.
 * 
 * @param resource_name Nome della risorsa.
 * @param mutex Mutex associato alla risorsa.
 * @param next Puntatore al prossimo elemento nella lista di mutex.
 */
typedef struct ResourceMutex {
    char *resource_name; ///< Nome della risorsa
    pthread_mutex_t mutex; ///< Mutex associato alla risorsa
    struct ResourceMutex *next; ///< Puntatore al prossimo elemento
} ResourceMutex;

// Funzioni per la gestione delle risorse
pthread_mutex_t* create_resource_mutex(const char *resource);
pthread_mutex_t* get_resource_mutex(const char *resource);
void destroy_resource_mutex(const char *resource);

// Variabili globali per la gestione delle risorse
ResourceMutex *resource_mutex_list = NULL;  ///< Puntatore alla lista dei mutex delle risorse

/**
 * Riceve un file dal client e lo salva sul server.
 * 
 * @param path Percorso dove salvare il file.
 * @param socket Socket utilizzato per ricevere il file.
 */
void receive_file(const char *path, int socket);

/**
 * Invia un file a un client tramite il socket.
 * 
 * @param path Percorso del file da inviare.
 * @param socket Socket utilizzato per la trasmissione.
 */
void send_file(const char *path, int socket);

/**
 * Gestisce i comandi ricevuti dal client.
 * 
 * @param socket Socket della connessione con il client.
 */
void processing(int socket);

/**
 * Divide un percorso completo in path e filename.
 * 
 * @param full_path Percorso completo del file.
 * @param path Parte del percorso senza il nome del file.
 * @param filename Nome del file.
 */
void split_path(const char *full_path, char *path, char *filename);

/**
 * Crea un file vuoto nel percorso specificato, se il file non esiste già.
 * 
 * @param full_path Percorso completo dove il file deve essere creato.
 */
void create_file(char *full_path);

/**
 * Crea una directory di root se non esiste.
 * 
 * @param dir Directory da creare.
 */
void make_root(char *dir);

/**
 * Modifica un percorso per adattarlo alla struttura serverDir.
 * 
 * @param path Percorso da modificare.
 * @param full_path Percorso completo.
 */
void fix_path(char *path, char *full_path);

/**
 * Riceve un messaggio dal client e lo salva nel buffer.
 * 
 * @param socket Socket utilizzato per ricevere il messaggio.
 * @param buffer Buffer dove memorizzare i dati ricevuti.
 * @param length Lunghezza del buffer, ovvero il numero massimo di byte da ricevere.
 * @return Numero di byte ricevuti, o -1 in caso di errore.
 */
ssize_t receive_message(int socket, void *buffer, size_t length);

/**
 * Invia un messaggio al client tramite il socket.
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
 * @return Un puntatore a una stringa allocata dinamicamente contenente i nomi 
 *         degli elementi nella directory, separati da virgole. Restituisce `NULL` 
 *         in caso di errore.
 * 
 * @note La stringa restituita deve essere liberata manualmente dal chiamante 
 *       utilizzando `free` per evitare perdite di memoria.
 */
char *listing_directory(char *path);

// Funzioni per il pool di thread

/**
 * Funzione che esegue il lavoro per ogni thread nella pool.
 * 
 * @param arg Argomento passato al worker thread.
 * @return Puntatore a NULL.
 */
void *thread_worker(void *arg);

/**
 * Aggiunge un client alla coda di attesa.
 * 
 * @param client_socket Socket del client da aggiungere alla coda.
 */
void enqueue_client(int client_socket);

/**
 * Rimuove un client dalla coda di attesa.
 * 
 * @return Socket del client rimosso dalla coda.
 */
int dequeue_client();
