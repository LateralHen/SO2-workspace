#ifndef FINGER_H
#define FINGER_H

// Includiamo le librerie necessarie
#include <utmp.h>  // Per ottenere le informazioni sugli utenti
#include <sys/types.h>
#include <pwd.h>    // Per ottenere informazioni come nome completo

// Definiamo una struttura per rappresentare un utente
struct user_info {
    char *username;
    char *tty;
    char *login_time;
    char *idle_time;
    char *full_name;
};

// Funzione per ottenere le informazioni sugli utenti
struct user_info *get_user_info();

// Funzione per stampare le informazioni di un utente
void print_user_info(struct user_info *user);

#endif
