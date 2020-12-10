#include <iostream>
#include "pab.hpp"
#include <memory.h>

using namespace std;

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

void clonarSolucao(Solucao &original, Solucao &copia) {
  memcpy(&copia, &original, sizeof(original));
}

void escreverSolucao(Solucao &solucao) {
  // int numeroBercosUtilizados , numeromNaviosAtendidos;
  // int totalViolacaoNavios, totalViolacaoBercos;
  // int diferenca;

  // numeroBercosUtilizados = numeroNaviosAtendidos = 0;
  // totalViolacaoBercos = totalViolacaoNavios = 0;

  // for (int k = 0; k < numeroBercos; k++) {
  //   if (proximoHorarioDisponivelBerco[k] - aberturaFechamento[k][ABERTURA] != 0) {
  //     numeroBercosUtilizados++;
  //   }

  //   diferenca = proximoHorarioDisponivelBerco[k] - aberturaFechamento[k][FECHAMENTO];
  //   if (diferenca > 0) {
  //     totalViolacaoNavios += diferenca;
  //   }
  // }

  // for (int i = 0; i < numeroNavios; i++) {
  //   if (solucao.atendimentoNavios[i] != -1) {
  //     numeroNaviosAtendidos++;
  //   }

  //   diferenca = solucao.tempoAtracamento[i] + duracaoAtendimento[solucao.atendimento[i]][i] - momentoSaidaNavio[i];
  //   if (diferenca > 0) {
  //     totalViolacaoNavios += diferenca;
  //   }
  // }

  // cout << "==== Solução ====" << endl;
  // cout << "Número de berços utilizados.........................: " << numeroBercosUtilizados << endl;
  // cout << "Número de navios atendidos..........................: " << numeroNaviosAtendidos << endl;
  // cout << "Total de viol. nas jan. de tempo dos bercos.........: " << totalViolacaoBercos << endl;
  // cout << "Total de viol. nas jan. de tempo dos navios.........: " << totalViolacaoNavios << endl;
  // cout << "Tempo total de operação.............................: " << solucao.tempoTotal << endl;

  // cout << endl << "Atendimentos: " << endl;
  // for(int i = 0; i < numeroNavios; i++) {
  //   cout << "Navio " << i + 1 << " -> berço " << solucao.atendimento[i] + 1 << ": ";
  //   cout << "\tHA " << solucao.tempoAtracamento[i] << "\t";
  //   cout << "HD " << solucao.tempoAtracamento[i] + duracaoAtendimento[solucao.atendimento[i]][i];
  //   cout << endl;
  // }
}

void removerAtendimento(Solucao &solucao, int navioARemover) {
  int berco = solucao.atendimentoNavios[navioARemover];
  int navio;

  if (berco != -1) {
    /**
     * Exemplo explicativo
     * berco x - 1 4 8 6 10 7
     */
    for (int i = solucao.atendimentoBercos[berco].tamanho; i > 1; i--) {
      navio = solucao.atendimentoBercos[berco].navios[i];
      solucao.atendimentoBercos[berco].navios[i] = solucao.atendimentoBercos[berco].navios[i + 1];

      if (navio == navioARemover) break;
    }

    solucao.atendimentoBercos[berco].tamanho--;
    solucao.atendimentoNavios[navioARemover] = -1;
  }
}

void inserirAtendimento(Solucao &solucao, int berco, int navio) {
  removerAtendimento(solucao, navio);

  solucao.atendimentoNavios[navio] = berco;
  solucao.atendimentoBercos[berco].navios[solucao.atendimentoBercos[berco].tamanho] = navio;

  /**
   * Exemplo explicativo
   * berco x - 1 4 8 6 9 10 7
   */
  for (int i = solucao.atendimentoBercos[berco].tamanho; i > 1; i--) {
    if (momentoChegadaNavio[navio] >= momentoChegadaNavio[solucao.atendimentoBercos[berco].navios[i]]) {
      break;
    }

    solucao.atendimentoBercos[berco].navios[i + 1] = solucao.atendimentoBercos[berco].navios[i];
    solucao.atendimentoBercos[berco].navios[i] = navio;
  }

  solucao.atendimentoBercos[berco].tamanho++;
}
