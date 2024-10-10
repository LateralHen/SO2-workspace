
**INDICE**
- [Suggerimenti per il linguaggio C](#suggerimenti-per-il-linguaggio-c)
  - [Compilazione](#compilazione)
  - [Tipi di dati](#tipi-di-dati)
  - [Puntatori](#puntatori)



# Suggerimenti per il linguaggio C

## Compilazione

I file nominati dalla direttiva `#include` sono le librerie/file che devono essere inclusi nel documento.
- se possiedono l'estensione .h allora il file viene chiamato header file;
- se circondati da < > allora sono librerie standard;
- se circondati da " " allora sono file creati dall'utente e che si trovano nella directory corrente.

Per compilare un programma C, è possibile utilizzare il comando gcc program1.c program2.c ..., dove uno dei solo file contiene la funzione main() del programma.
  Il programma gcc include molte opzioni:
  - [-Wall] mostra tutti i messaggi di avvertimento presenti alla compilazione (abbreviazione di Warning All)
  - [-Wextra] mostra ulteriori warning "non-standard"
  - [-o filename] permette di specificare il nome del file di output. Se non utilizzata, il nome utilizzato sarà a.out.
  - [-c] effettua solo la compilazione, dando per assunto che il file in input sia stato precompilato.
    
  - [-lm] permette di includere librerie matematiche come `<math.h>`


## Tipi di dati
  - I tipi di dati vengono gestiti come con java;
  - devono essere dichiarati prima di poterne assegnare un valore ad essi; **!!N.B.!!** ciò non significa che devo usare più righe per dichiarare e per poi assegnare un valore: \ *qui dovrei aggiungere una table che mostra la differenza ma non riesco a metterla quindi mi attacco al cazzo*
| col                 | col |
| ------------------- | --- |
| perche non funziona | sad |

    
## Puntatori