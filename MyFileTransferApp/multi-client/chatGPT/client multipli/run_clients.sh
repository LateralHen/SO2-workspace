#!/bin/bash

# Numero di client da avviare
NUM_CLIENTS=5

# Avvia ogni client in background
for i in $(seq 1 $NUM_CLIENTS); do
  ./client &  # Esegui il client in background
  echo "Client $i avviato"
done

# Aspetta che tutti i client finiscano
wait
echo "Tutti i client hanno terminato."
