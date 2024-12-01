#include "server1.h"


#define THREAD_POOL_SIZE 5

// Variabili globali
int server_fd;
pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

int client_queue[MAX_CONNECTION];
//indici della coda di attesa dei thread
int queue_front = 0, queue_rear = 0, queue_size = 0;

// Funzioni x pool thread
void *thread_worker(void *arg);
void enqueue_client(int client_socket);
int dequeue_client();



// Funzione principale
int main(int argc, char *argv[]) {

    int opt;
    struct sockaddr_in server_addr, client_addr;
    int server_fd;
    socklen_t addr_len = sizeof(client_addr);

    while ((opt = getopt(argc, argv, "a:p:d:")) != -1) {
        switch (opt) {
            case 'a':
                if (inet_addr(optarg) == INADDR_NONE)
                {
                    perror("Indirizzo Ip non valido.\n");
                    return 1;
                }
                server_address = optarg;
                break;
            case 'p':
                server_port = atoi(optarg);
                if (server_port < 1 || server_port > 65535) {
                    fprintf(stderr, "Errore: La porta deve essere tra 1 e 65535.\n");
                    return 1;
                }
                break;
            case 'd':
                root_dir = optarg;
                make_root(root_dir);
                break;
            default:
                fprintf(stderr, "Uso: %s -a indirizzo -p porta -d root Directory\n", argv[0]);
                return 1;
        }
    }

    if (!server_address || server_port == 0 || !root_dir) {
        fprintf(stderr, "Errore nei parametri.\n");
        exit(1);
    }

    
    // Creazione del socket del server
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Errore nella creazione del socket");
        exit(1);
    }

    // Configurazione dell'indirizzo
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_address);



    // Binding
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore nel binding");
        exit(1);
    }

    // Ascolto delle connessioni
    if (listen(server_fd, MAX_CONNECTION) == -1) {
        perror("Errore nell'ascolto");
        exit(1);
    }

    printf("Server in ascolto su %s:%d\n", server_address, server_port);

    /* 
    Creazione del thread pool.
    Ad ogni thread viene associata la funzione thread_worker.
    */

    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        if (pthread_create(&thread_pool[i], NULL, thread_worker, NULL) != 0) {
            perror("Errore nella creazione del thread");
            exit(1);
        }
    }

    // Ciclo di accettazione connessioni
    while (1) {
        int client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Errore nell'accettare la connessione");
            continue;
        }
        printf("Connessione accettata da %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        enqueue_client(client_socket);
    }

    return 0;
}

// Implementazione delle funzioni correlate ai file

void split_path(const char *full_path, char *path, char *filename) {
    const char *last_slash = strrchr(full_path, '/');
    if (last_slash != NULL) {
        size_t path_length = last_slash - full_path + 1;
        strncpy(path, full_path, path_length);
        path[path_length] = '\0';
        strcpy(filename, last_slash + 1);
    } else {
        strcpy(path, "");
        strcpy(filename, full_path);
    }
}

int send_message(int socket, const void *message, size_t length) {
    ssize_t bytes_sent = 0;
    while (bytes_sent < length) {
        ssize_t result = send(socket, (char*)message + bytes_sent, length - bytes_sent, 0);
        if (result < 0) {
            perror("Errore durante l'invio del messaggio");
            return -1;  // Errore durante l'invio
        }
        bytes_sent += result;
    }
    return bytes_sent;  // Ritorna il numero totale di byte inviati
}

ssize_t receive_message(int socket, void *buffer, size_t length) {
    ssize_t bytes_received = 0;
    while (bytes_received < length) {
        ssize_t result = recv(socket, (char*)buffer + bytes_received, length - bytes_received, 0);
        if (result < 0) {
            perror("Errore durante la ricezione del messaggio");
            return -1;  // Errore durante la ricezione
        }
        if (result == 0) {
            printf("Connessione chiusa dal client\n");
            return 0;  // Connessione chiusa
        }
        bytes_received += result;
    }
    return bytes_received;  // Ritorna il numero totale di byte ricevuti
}


void send_file(const char *path, int socket) {
    
    printf("file da inviare: %s...\n", path);
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Errore nell'aprire il file %s: %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        ssize_t bytes_sent = send(socket, buffer, bytes_read, 0);
        
        // Controlla se l'invio ha avuto successo
        if (bytes_sent < 0) {
            perror("Errore durante l'invio dei dati");
            fclose(file);
            return;
        }

        // Verifica che siano stati inviati tutti i byte letti
        if (bytes_sent < bytes_read) {
            fprintf(stderr, "Non tutti i byte sono stati inviati al socket\n");
            fclose(file);
            return;
        }
    }
    
    printf("file inviato con successo \n");
    fclose(file);
}

void receive_file(const char *path, int socket) {
    char buffer[BUFFER_SIZE];
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        perror("Errore nell'aprire il file");
        return;
    }

    ssize_t bytes_received;
    while ((bytes_received = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    if (bytes_received < 0) {
        perror("Errore durante la ricezione dei dati");
    }

    fclose(file);
}

// Gestione delle connessioni

void processing(int socket) {
    
    char command;
    if (receive_message(socket, &command, sizeof(char)) < 0){
        perror("Errore durante l'invio del comando");
        close(socket);
        return;
    }

    int pathlen;
    if (receive_message(socket, &pathlen, sizeof(pathlen)) < 0){
        perror("Errore durante la ricezione della dimensione del path");
        close(socket);
        return;
    }
    printf("pathlen: %d\n", pathlen);

    char *path = (char *)malloc(pathlen);
    if (receive_message(socket, path, pathlen) < 0)
    {
        perror("Errore durante la ricezione del path");
        free(path);
        close(socket);
        return;
    }
    char *full_path = (char *)malloc(256 * sizeof(char));

    fix_path(path, full_path);
    printf("path: %s\n ", full_path);

    switch (command) {
        case 'w':
            printf("***FUNZIONE DI WRITE***\n");
            receive_file(full_path, socket);
            break;
        case 'r':
            printf("***FUNZIONE DI READ***\n");
            send_file(full_path, socket);
            break;
        case 'l':
            printf("***FUNZIONE DI LISTING***\n");
            printf("path: %s\n", full_path);


            char *output;
            output = listing_directory(full_path);
            int output_len = strlen(output);
            send_message(socket, &output_len, sizeof(output_len));
            printf("%s \n dimensione: %d\n", output, output_len);
            send_message(socket, output, output_len);
            break;
        default:
            fprintf(stderr, "Comando non riconosciuto\n");
            break;
    }

    free(full_path);
    close(socket);
}

// Funzioni di gestione della root e percorsi

void make_root(char *dir) {
    if (access(dir, F_OK) == -1 && mkdir(dir, 0777) == -1) {
        perror("Errore nella creazione della directory");
        exit(1);
    }
}

void create_file(char *full_path) {
    FILE *file = fopen(full_path, "wb");
    if (file == NULL) {
        perror("Errore nella creazione del file");
        return;
    }
    fclose(file);
}

void fix_path(char *path, char *full_path) {

/*     // Aggiungi serverDir all'inizio del percorso
    int new_len = strlen(root_dir) + strlen(full_path) + 2;  // +2 per la barra e il terminatore di stringa
    char temp_path[new_len];

    // Costruisci il nuovo percorso
    snprintf(temp_path, sizeof(temp_path), "%s/%s", root_dir, full_path);

    // Copia il nuovo percorso in full_path
    strcpy(full_path, temp_path); */

    size_t len_root = strlen(root_dir);
    size_t len_path = strlen(path);
    full_path = realloc(full_path,(len_path + len_root + 2));
    snprintf(full_path, len_path + len_root + 2, "%s/%s", root_dir, path);



}



char *listing_directory(char *path){
    // path = "serverDir/prova_di_listing";
    struct dirent *entry;  // Per leggere ogni elemento della directory
    DIR *dir = opendir(path);  // Apre la directory
    
    printf("\nPATH: %s\n\n",path);
    if (dir == NULL) {
        perror("Errore nell'aprire la directory");
        return NULL;
    }

    // Verifica se il path è effettivamente una directory
    struct stat path_stat;
    if (stat(path, &path_stat) != 0 || !S_ISDIR(path_stat.st_mode)) {
        perror("Il percorso fornito non è una directory");
        closedir(dir);
        return NULL;
    }

    int size_output = 256;
    char *output = malloc(size_output);
    if (output == NULL){
        perror("Errore nell'allocazione della memoria");
        closedir(dir);
        return NULL;
    }
    
    output[0] = '\0';
    int size_temp = 0;

    while ((entry = readdir(dir)) != NULL){
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
           
            int len_filename = strlen(entry->d_name);
            //aumento di memoria per output nel caso non fosse sufficiente
            if (size_temp + len_filename + 2 >= size_output){ // +2 per il separatore e per il terminatore
                
                size_output *= 2;
                output = realloc(output, size_output);

                if (output == NULL){
                    perror("Errore nel ridimensionamento della memoria");
                    closedir(dir);
                    return NULL;
                }
            }

            if (size_temp > 0) {
                strcat(output, ", ");
                size_temp += 2;
            }
            strcat(output, entry -> d_name);
            size_temp += len_filename;
        }
    }

    closedir(dir);
    return output;
}


void *thread_processing(void *arg){}// l'ho aggiunta io questa funzione.


/* lavora in un loop infinito. preleva il primo client dalla coda d'attesa-> lavora come prima.   */
void *thread_worker(void *arg){
    //loop infinito
    while (1) {
        int client_socket = dequeue_client();
        printf("Thread %lu sta processando il client %d\n", pthread_self(), client_socket); // lu = insigned long integer

        // Esegui la gestione del client
        char buffer[1024];
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Messaggio ricevuto dal client %d: %s\n", client_socket, buffer);

            // Rispondi al client
            send(client_socket, "Ricevuto\n", 9, 0);
        }

        // Chiudi il socket del client
        close(client_socket);
        printf("Connessione chiusa con il client %d\n", client_socket);
    }
    return NULL;
}

/* serve per mettere in coda d'attesa i client in più. */
void enqueue_client(int client_socket){
    // acquisizione del mutex
    pthread_mutex_lock(&queue_lock);
    //Attesa nella coda
    while (queue_size == MAX_CONNECTION) {
        pthread_cond_wait(&queue_cond, &queue_lock);
    }

    // inserisci il socket nella coda
    client_queue[queue_rear] = client_socket;
    queue_rear = (queue_rear + 1) % MAX_CONNECTION;
    queue_size++;

    pthread_cond_signal(&queue_cond); // Notifica ai thread
    pthread_mutex_unlock(&queue_lock);

}

/* prelava il primo client dalla coda di attesa  */
int dequeue_client(){
    //acquisizione del mutex
    pthread_mutex_lock(&queue_lock);

    while (queue_size == 0) {
        pthread_cond_wait(&queue_cond, &queue_lock); // Aspetta che ci siano client
    }

    int client_socket = client_queue[queue_front];
    queue_front = (queue_front + 1) % MAX_CONNECTION;
    queue_size--;

    pthread_cond_signal(&queue_cond); // Notifica che c'è spazio libero
    pthread_mutex_unlock(&queue_lock);

    return client_socket;
}


