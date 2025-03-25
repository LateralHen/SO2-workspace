#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

/** 
 * Struttura che contiene le informazioni di connessione.
 * 
 * @param server_address Indirizzo del server a cui connettersi.
 * @param port Porta del server.
 */
typedef struct {
    char *server_address;
    int port;

} Address;


/**
 * Struttura che contiene le informazioni del comando passato al client.
 * 
 * @param command Comando principale da inviare al server.
 * @param command2 Comando secondario (se necessario).
 * @param local_path Percorso locale da utilizzare per il file.
 * @param remote_path Percorso remoto per il file sul server.
 * 
 */
typedef struct ClientComm
{
    char command;
    char command2;
    char *local_path;
    char *remote_path;
}Command;



/** 
 * Riceve un file dal server e lo salva nel percorso specificato.
 * 
 * @param path Percorso dove salvare il file ricevuto.
 * @param socket Socket utilizzato per ricevere il file.
 */
void receive_file(const char *path, int socket);

/** 
 * Invia un file al server dal percorso locale specificato.
 * 
 * @param path Percorso del file da inviare.
 * @param socket Socket utilizzato per inviare il file.
 */
void send_file(const char *path, int socket);

/** 
 * Crea un file vuoto nel percorso specificato.
 * 
 * @param full_path Percorso completo del file da creare.
 */
void create_file(char *full_path);

/** 
 * Divide un percorso completo in due parti: il percorso e il nome del file.
 * Viene utilizzato dentro create_file per separare il path del file dal nome del file.
 * 
 * @param full_path Percorso completo del file.
 * @param path Variabile dove sarà memorizzato il percorso senza il nome del file.
 * @param filename Variabile dove sarà memorizzato solo il nome del file.
 */
void split_path(const char *full_path, char *path, char *filename);

/**
 * Corregge il percorso di input per adattarlo al formato previsto.
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
 * Riceve un messaggio dal server e lo memorizza nel buffer.
 * 
 * @param socket Socket attraverso cui ricevere il messaggio.
 * @param buffer Buffer in cui memorizzare i dati ricevuti.
 * @param length Lunghezza del messaggio da ricevere.
 * @return Numero di byte ricevuti, o -1 in caso di errore.
 */
ssize_t receive_message(int socket, void *buffer, size_t length);

/** 
 * Invia un messaggio al server attraverso il socket.
 * 
 * @param socket Socket attraverso cui inviare il messaggio.
 * @param message Messaggio da inviare.
 * @param length Lunghezza del messaggio da inviare.
 * @return Numero di byte inviati, o -1 in caso di errore.
 */
int send_message(int socket, const void *message, size_t length);


void processing(int socket);

Address connessione;
Command comando_imput;

/** 
 * Directory di base per il client.
 */
const char *base_dir = "clientDir";