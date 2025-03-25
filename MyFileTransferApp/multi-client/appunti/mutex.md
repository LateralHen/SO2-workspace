# MUTEX PER RISORSE

## OBIETTIBVO

Gestire la concorrenza dal lato server per le **directory** e **file**. Devo gestire:

- lettura;
- scrittura;
- listing per le directory.

## MUTEX PER LA GESTIONE DELLE DIRECTORY

- **Mutex per ogni risorsa:** Ogni directory o file avrà un mutex per la proteggere la sua struttura.

## FUNZIONI CREATE

```c
 pthread_mutex_t* create_resource_mutex(const char *resource)
```

```c
pthread_mutex_t* get_resource_mutex(const char *resource)
```

```c
void destroy_resource_mutex(const char *resource)
```
