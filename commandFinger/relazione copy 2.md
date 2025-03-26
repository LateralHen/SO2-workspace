# ✅ RELAZIONE REVISIONATA – *Myfinger*

---

## 1. Obiettivo del progetto

L’obiettivo principale è stato sviluppare un programma in linguaggio C che emulasse il comportamento del comando `finger`, fornendo informazioni sugli utenti **locali** del sistema. Come richiesto dalla consegna, **non è stato implementato il supporto per utenti remoti**.

---

## 2. Vincoli e requisiti

### Vincoli tecnici

- Non era permesso utilizzare funzioni della famiglia `exec*(3)`.

### Requisiti funzionali

1. Restituire informazioni sugli utenti locali, come:

   - nome utente,
   - terminale,
   - orario di accesso,
   - directory personale,
   - shell di login.
   - shell di login.

2. Gestire correttamente le opzioni specificate dall’utente e i nomi utente passati come argomenti.

---

## 3. Struttura del codice

Il codice è stato suddiviso in più funzioni per garantire modularità, leggibilità e riutilizzabilità. Di seguito una panoramica delle componenti principali.

### 3.1 Funzioni principali

- **`main`**  
  Analizza gli argomenti passati da linea di comando, distinguendo tra opzioni (`-l`, `-s`, `-m`, `-p`) e nomi utente.  
  Gestisce due casi distinti: nessun argomento, oppure specifica di opzioni/nome utente.  
  Chiama le funzioni `userSelection()` e `optionSelection()` per proseguire l'elaborazione.

- **`optionSelection`**  
  Analizza le opzioni richieste e seleziona la modalità di stampa (`printS` o `printL`).  
  Gestisce eventuali errori dovuti a opzioni non valide.

- **`userSelection`**  
  Recupera l’elenco degli utenti attivi se non vengono specificati nomi.  
  In caso contrario, verifica la validità dei nomi passati confrontandoli con il database locale (`/etc/passwd`).

- **`gecos_format`**  
  Elabora il campo GECOS della struttura `passwd` per estrarre informazioni quali:
  nome completo, ufficio, telefono, ecc.

- **`printS`**  
  Visualizza un formato compatto con:
  - login,
  - nome reale,
  - terminale,
  - orario di accesso,
  - host.

- **`printL`**  
  Mostra informazioni dettagliate aggiuntive:
  - directory personale,
  - shell,
  - telefono,
  - presenza di file `.plan`, `.project`, `.pgpkey` nella home dell’utente.

---

### Gestione della riga di comando

## 4. Soluzioni implementative

### Gestione dinamica della memoria

Gli argomenti sono analizzati con un ciclo che separa le opzioni dai nomi utente, concatenando le opzioni in una stringa e salvando dinamicamente i nomi.

L’uso di `malloc`, `realloc` e `strdup` consente una gestione flessibile degli array di stringhe, evitando limiti fissi e permettendo la libera aggiunta di utenti. La memoria è correttamente liberata al termine dell’esecuzione.

### Accesso ai dati di sistema

Sono state utilizzate le strutture:

### Validazione input

- `struct utmp` (via `getutent`) per rilevare gli utenti attualmente loggati.

### Formattazione e output

I nomi utente forniti vengono verificati rispetto al database locale. In caso di nomi non riconosciuti, viene mostrato un messaggio d’errore senza bloccare l’esecuzione.
