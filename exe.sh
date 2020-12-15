#!/bin/sh
inst="1 2 3 4 5"

for i in $inst;
do
  #EXEC	  INST				                TAMANHO LISTA   TEMPO	  N EXE   MAX TROCA   POT TROCA  SEED
  ./pab   'instancias/i0'"${i}"'.txt'	10000	          30      3       0           0
done
