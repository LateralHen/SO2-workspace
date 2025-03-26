# Relazione su Myfinger

## 1. Obiettivo del progetto

L’obiettivo principale è stato sviluppare un programma in linguaggio C che emulasse il comportamento del comando `finger`, fornendo informazioni sugli utenti **locali** del sistema. Come richiesto dalla consegna, non è stato implementato il supporto per utenti remoti.

## 2. Vincoli e requisiti

### Vincoli tecnici

- Non era permesso utilizzare funzioni della famiglia `exec*(3)`.

### Requisiti funzionali

1. Restituire informazioni sugli utenti locali, come nome, terminale, orario di accesso, directory personale e shell.

2. Gestire correttamente le opzioni specificate dall’utente e i nomi utente passati come argomenti.

## 3. Struttura del codice

Il codice è stato suddiviso in più funzioni per garantire modularità, leggibilità e riutilizzabilità.

### Funzioni principali

- **`main`**  
  Analizza gli argomenti della riga di comando, distinguendo tra opzioni (`-l`, `-s`, `-m`, `-p`) e nomi utente. Invoca le funzioni `userSelection()` e `optionSelection()`.

- **`optionSelection`**  
  Elabora le opzioni richieste e seleziona la funzione di stampa appropriata (`printS` o `printL`).

- **`userSelection`**  
  Recupera automaticamente l’elenco degli utenti loggati o valida i nomi utente specificati rispetto a quelli del sistema.

- **`gecos_format`**  
  Estrae i dati dal campo GECOS (nome completo, ufficio, telefono, ecc.).

- **`printS`**  
  Visualizza in formato compatto le informazioni: login, nome reale, terminale, orario di accesso, host e **Idle Time**.  
  L'idle time è calcolato dinamicamente attraverso il file `/dev/<tty>` mediante `stat()`.

- **`printL`**
  Visualizza informazioni dettagliate come directory personale, shell, telefono e il contenuto dei file `.plan`, `.project`, `.pgpkey` se presenti nella home dell'utente.

### Gestione della riga di comando

Parsing manuale delle opzioni e dei nomi utente, con costruzione dinamica di array di stringhe.

### Gestione dinamica della memoria

Utilizzo di `malloc`, `realloc`, `strdup` e `free` per gestire l’allocazione e la liberazione della memoria utilizzata per gli array di nomi e dati GECOS.

### Accesso ai dati di sistema

Uso delle strutture:

- `struct passwd` per le informazioni statiche degli utenti;
- `struct utmp` per gli utenti attualmente loggati.

### Validazione dell'input

Verifica che i nomi utente forniti corrispondano a utenti reali presenti nel sistema (`getpwent()`).

### Formattazione dell'output

Presentazione delle informazioni in formato leggibile, compatibile con l’output del comando `finger`.  
La gestione dell’Idle Time e la visualizzazione del contenuto dei file utente rendono il comportamento aderente a `finger -l`.
