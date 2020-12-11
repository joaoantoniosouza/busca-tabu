#include <iostream>
#include "pab.hpp"

using namespace std;

#define MAX(X,Y) ((X > Y) ? X : Y)

int main (int argc, char **argv) {
  Solucao solucao;

  #ifdef DEBUG
    lerInstancia("./instancias/i01.txt");
  #else
    lerInstancia(argv[1]);
  #endif

  return 0;
}

void lerInstancia (char* nomeInstancia) {
  FILE* instancia = fopen(nomeInstancia, "r");

  char *erro = "Erro na leitura do arquivo";

  if (!fscanf(instancia, "%d %d", &numeroNavios, &numeroBercos)) {
    printf(erro);
  }

  for (int k = 0; k < numeroBercos; k++) {
    for (int i = 0; i < numeroNavios; i++ ) {
      if (!fscanf(instancia, "%d", &duracaoAtendimento[k][i])) {
        printf(erro);
      }
    }
  }

  for (int k = 0; k < numeroBercos; k++) {
    if (!fscanf(instancia, "%d %d", &aberturaFechamento[k][ABERTURA], &aberturaFechamento[k][FECHAMENTO])) {
      printf(erro);
    }
  }

  for (int i = 0; i < numeroNavios; i++) {
    if (!fscanf(instancia, "%d", &momentoChegadaNavio[i])) {
      printf(erro);
    }
  }

  for (int i = 0; i < numeroNavios; i++) {
    if (!fscanf(instancia, "%d", &momentoSaidaNavio[i])) {
      printf(erro);
    }
  }
}
