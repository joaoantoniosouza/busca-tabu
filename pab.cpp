#include <iostream>
#include "pab.hpp"

using namespace std;

#define MAX(X,Y) ((X > Y) ? X : Y)

#define TEST

int main (int argc, char **argv) {
  Solucao solucao;
  return 0;
}

void lerInstancia (char* nomeInstancia) {
  FILE* instancia = fopen(nomeInstancia, "r");

  fscanf(instancia, "%d %d", &numeroNavios, &numeroBercos);

  for (int k = 0; k < numeroBercos; k++) {
    for (int i = 0; i < numeroNavios; i++ ) {
      fscanf(instancia, "%d", &duracaoAtendimento[k][i]);
    }
  }

  for (int k = 0; k < numeroBercos; k++) {
    fscanf(instancia, "%d %d", &aberturaFechamento[k][ABERTURA], &aberturaFechamento[k][FECHAMENTO]);
  }

  for (int i = 0; i < numeroNavios; i++) {
    fscanf(instancia, "%d", &momentoChegadaNavio[i]);
  }

  for (int i = 0; i < numeroNavios; i++) {
    fscanf(instancia, "%d", &momentoSaidaNavio[i]);
  }
}
