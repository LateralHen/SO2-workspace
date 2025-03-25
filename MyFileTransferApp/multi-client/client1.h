#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <getopt.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

/** 
 * @brief Struttura che contiene le informazioni di connessione.
 * 
 * Questa struttura viene utilizzata per memorizzare l'indirizzo del server e la porta
 * per stabilire la connessione.
 * 
 * @param server_address Indirizzo del server a cui connettersi.
 * @param port Porta del server.
 */
typedef struct {
    char *server_address; /**< Indirizzo del server a cui connettersi. */
    int port; /**< Porta del server. */
} Address;


/**
 * @brief Struttura che contiene le informazioni del comando passato al client.
 * 
 * Questa struttura viene utilizzata per gestire i comandi da inviare al server, inclusi
 * i percorsi dei file locali e remoti, e i comandi opzionali aggiuntivi.
 * 
 * @param command Comando principale da inviare al server.
 * @param command2 Comando secondario (se necessario).
 * @param local_path Percorso locale da utilizzare per il file.
 * @param remote_path Percorso remoto per il file sul server.
 */
typedef struct ClientComm
{
    char command; /**< Comando principale da inviare al server. */
    char command2; /**< Comando secondario (se necessario). */
    char *local_path; /**< Percorso locale da utilizzare per il file. */
    char *remote_path; /**< Percorso remoto per il file sul server. */
} Command;


/** 
 * @brief Directory di base per il client.
 * 
 * Questa variabile contiene il percorso della directory di base utilizzata dal client
 * per gestire i file locali.
 */
const char *base_dir = "clientDir";


/** 
 * @brief Funzione di elaborazione principale per gestire le operazioni del client.
 * 
 * Questa funzione gestisce la comunicazione tra il client e il server, 
 * eseguendo operazioni specifiche in base ai comandi ricevuti.
 * 
 * @param socket Socket utilizzato per la comunicazione con il server.
 */
void processing(int socket);


/** 
 * @brief Riceve un file dal server e lo salva nel percorso specificato.
 * 
 * Questa funzione riceve i dati da un server attraverso una connessione socket
 * e li salva nel percorso specificato.
 * 
 * @param path Percorso dove salvare il file ricevuto.
 * @param socket Socket utilizzato per ricevere il file.
 */
void receive_file(const char *path, int socket);


/** 
 * @brief Invia un file al server dal percorso locale specificato.
 * 
 * Questa funzione legge il contenuto di un file dal percorso locale e lo invia
 * al server attraverso una connessione socket.
 * 
 * @param path Percorso del file da inviare.
 * @param socket Socket utilizzato per inviare il file.
 */
void send_file(const char *path, int socket);


/** 
 * @brief Crea un file vuoto nel percorso specificato.
 * 
 * Questa funzione crea un nuovo file vuoto nel percorso specificato.
 * 
 * @param full_path Percorso completo del file da creare.
 */
void create_file(char *full_path);


/** 
 * @brief Divide un percorso completo in due parti: il percorso e il nome del file.
 * 
 * Viene utilizzato dentro create_file per separare il path del file dal nome del file.
 * 
 * @param full_path Percorso completo del file.
 * @param path Variabile dove sarà memorizzato il percorso senza il nome del file.
 * @param filename Variabile dove sarà memorizzato solo il nome del file.
 */
void split_path(const char *full_path, char *path, char *filename);


/**
 * @brief Corregge il percorso di input per adattarlo al formato previsto.
 * 
 * Questa funzione permette di aggiungere o rimuovere la directory di base
 * (`base_dir`) da un percorso specificato. Se l'opzione `add_base` è attiva,
 * il percorso di input viene combinato con `base_dir` (se necessario).
 * Altrimenti, `base_dir` viene rimosso dal percorso di input (se presente).
 * 
 * @param input_path   [in] Percorso di input da modificare.
 *                     Deve essere una stringa valida terminata con `\0`.
 * @param output_path  [out] Puntatore al percorso risultante. La memoria per
 *                     il percorso corretto viene allocata dinamicamente
 *                     all'interno della funzione, e deve essere liberata
 *                     dal chiamante usando `free`.
 * @param add_base     [in] Flag che indica l'operazione da eseguire:
 *                     - `1` per aggiungere `base_dir` al percorso di input,
 *                       se non è già presente.
 *                     - `0` per rimuovere `base_dir` dal percorso di input,
 *                       se presente.
 */
void fix_path(const char *input_path, char **output_path, int add_base);


/** 
 * @brief Riceve un messaggio dal server e lo memorizza nel buffer.
 * 
 * Questa funzione riceve un messaggio dal server attraverso una connessione socket
 * e lo memorizza nel buffer fornito.
 * 
 * @param socket Socket attraverso cui ricevere il messaggio.
 * @param buffer Buffer in cui memorizzare i dati ricevuti.
 * @param length Lunghezza del messaggio da ricevere.
 * @return Numero di byte ricevuti, o -1 in caso di errore.
 */
ssize_t receive_message(int socket, void *buffer, size_t length);


/** 
 * @brief Invia un messaggio al server attraverso il socket.
 * 
 * Questa funzione invia un messaggio al server utilizzando il socket specificato.
 * 
 * @param socket Socket attraverso cui inviare il messaggio.
 * @param message Messaggio da inviare.
 * @param length Lunghezza del messaggio da inviare.
 * @return Numero di byte inviati, o -1 in caso di errore.
 */
int send_message(int socket, const void *message, size_t length);


// Variabili globali per la connessione e il comando dell'utente
Address connessione; /**< Variabile contenente le informazioni di connessione al server. */
Command comando_imput; /**< Variabile contenente le informazioni del comando del client. */
