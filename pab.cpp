#include <iostream>
#include "pab.hpp"

using namespace std;

#define MAX(X,Y) ((X > Y) ? X : Y)

#define TEST

int main (int argc, char **argv) {
  Solucao solucao;
  return 0;
}

void lerInstancia () {
  cin >> numeroNavios >> numeroBercos;

  for (int k = 0; k < numeroBercos; k++) {
    for (int i = 0; i < numeroNavios; i++ ) {
      cin >> duracaoAtendimento[k][i];
    }
  }

  for (int k = 0; k < numeroBercos; k++) {
    cin >> aberturaFechamento[k][ABERTURA] >> aberturaFechamento[k][FECHAMENTO];
    proximoHorarioDisponivelBerco[k] = aberturaFechamento[k][ABERTURA];
  }

  for (int i = 0; i < numeroNavios; i++) {
    cin >> momentoChegadaNavio[i];
  }

  for (int i = 0; i < numeroNavios; i++) {
    cin >> momentoSaidaNavio[i];
  }
}
