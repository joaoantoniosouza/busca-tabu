#!/bin/sh
inst="1 2 3 4 5"

for i in $inst;
do
  #EXEC	  INST				                TEMPO	  N EXE   MAX TROCA   POT TROCA   PERTUBACAO  SEED
  ./pab   'instancias/i0'"${i}"'.txt'	5		    3       1000        4           100
done
