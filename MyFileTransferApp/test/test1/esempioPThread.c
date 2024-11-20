#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* thread_function(void* arg) {
    int id = *(int*)arg;
    printf("Thread %d in esecuzione\n", id);
    return NULL;
}

int main() {
    pthread_t threads[2];
    int thread_ids[2] = {1, 2};

    // Creazione dei thread
    for (int i = 0; i < 2; i++) {
        if (pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]) != 0) {
            perror("Errore nella creazione del thread");
            exit(1);
        }
    }

    // Attesa del termine dei thread
    for (int i = 0; i < 2; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Errore nella join del thread");
            exit(1);
        }
    }

    printf("Thread completati\n");
    return 0;
}

