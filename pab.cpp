#include <iostream>
#include <string>
#include <memory.h>
#include <climits>
#include "pab.hpp"

using namespace std;

#define MAX(X,Y) ((X > Y) ? X : Y)

#define DEBUG

int main (int argc, char **argv) {
  Solucao solucao;

  #ifdef DEBUG
    lerInstancia("./instancias/i01.txt");
  #else
    if (argc != 2) {
      cout << "#ERRO: Informe corretamente o caminho para a instância..." << endl;
      return 0;
    }

    lerInstancia(argv[1]);
  #endif

  // lerInstancia(argv[1]);
  int seed, menorTempo, melhorSeed;
  menorTempo = INT_MAX;
  for(int i = 0; i < 10000; i++) {
    seed = time(NULL);
    srand(i);
    heuristicaConstrutiva(solucao);
    calcularFO(solucao);

    if (solucao.tempoAtendimentoTotal < menorTempo) {
      menorTempo = solucao.tempoAtendimentoTotal;
      melhorSeed = i;
    }
  }

  srand(melhorSeed);
  heuristicaConstrutiva(solucao);
  calcularFO(solucao);
  escreverSolucao(solucao);

  return 0;
}

void heuristicaConstrutiva (Solucao &solucao) {
  int berco, limiteBusca;

  for (int k = 0; k < numeroBercos; k++) {
    solucao.atendimentoBercos[k].tamanho = 0;
  }

  memset(&solucao.atendimentoNavios, -1, sizeof(solucao.atendimentoNavios));

  limiteBusca = MAX(100, numeroBercos * numeroNavios);
  for (int i = 0; i < numeroNavios; i++) {
    for (int l = 0; l < limiteBusca; l++) {
      berco = rand()%(numeroBercos);

      if (duracaoAtendimento[berco][i] == 0) {
        berco = -1;
      } else {
        break;
      }
    }

    inserirAtendimento(solucao, berco, i);
  }
}

void calcularFO(Solucao &solucao) {
  int navio, proximoHorarioDisponivelBerco, momentoAtracamentoNavio;
  solucao.tempoAtendimentoTotal = 0;

  for (int k = 0; k < numeroBercos; k++) {
    proximoHorarioDisponivelBerco = aberturaFechamento[k][ABERTURA];

    for (int i = 0; i < solucao.atendimentoBercos[k].tamanho; i++) {
      navio = solucao.atendimentoBercos[k].navios[i];
      momentoAtracamentoNavio = MAX(proximoHorarioDisponivelBerco, momentoChegadaNavio[navio]);
      proximoHorarioDisponivelBerco = momentoAtracamentoNavio + duracaoAtendimento[k][navio];

      solucao.tempoAtendimentoTotal += momentoAtracamentoNavio - momentoChegadaNavio[navio] + duracaoAtendimento[k][navio];

      if (proximoHorarioDisponivelBerco > momentoSaidaNavio[navio]) {
        solucao.tempoAtendimentoTotal += PENALIDADE_HORARIO_LIMITE_NAVIO;
      }
    }

    if (proximoHorarioDisponivelBerco > aberturaFechamento[k][FECHAMENTO]) {
      solucao.tempoAtendimentoTotal += PENALIDADE_HORARIO_LIMITE_BERCO;
    }
  }
}

void removerAtendimento (Solucao &solucao, int navioARemover) {
  int berco = solucao.atendimentoNavios[navioARemover];
  int navio, aux;

  if (berco != -1) {
    /**
     * Exemplo explicativo
     * berco x - 1 4 8 6 10 7 0
     */
    int i = solucao.atendimentoBercos[berco].tamanho - 1;
    navio = -1;
    do {
      aux = navio;
      navio = solucao.atendimentoBercos[berco].navios[i];
      solucao.atendimentoBercos[berco].navios[i] = aux;
      i--;
    } while (navio != navioARemover);

    solucao.atendimentoBercos[berco].tamanho--;
    solucao.atendimentoNavios[navioARemover] = -1;
  }
}

void inserirAtendimento (Solucao &solucao, int berco, int navio) {
  removerAtendimento(solucao, navio);

  solucao.atendimentoNavios[navio] = berco;
  solucao.atendimentoBercos[berco].navios[solucao.atendimentoBercos[berco].tamanho] = navio;

  /**
   * Exemplo explicativo
   * berco x - 1 4 8 6 9 10 7
   */
  for (int i = solucao.atendimentoBercos[berco].tamanho; i > 0; i--) {
    if (momentoChegadaNavio[navio] >= momentoChegadaNavio[solucao.atendimentoBercos[berco].navios[i - 1]]) {
      break;
    }

    solucao.atendimentoBercos[berco].navios[i] = solucao.atendimentoBercos[berco].navios[i - 1];
    solucao.atendimentoBercos[berco].navios[i - 1] = navio;
  }

  solucao.atendimentoBercos[berco].tamanho++;
}

void clonarSolucao (Solucao &original, Solucao &copia) {
  memcpy(&copia, &original, sizeof(original));
}

void escreverSolucao (Solucao &solucao) {
  int numeroBercosUtilizados , numeroNaviosAtendidos;
  int totalViolacaoNavios, totalViolacaoBercos;

  numeroBercosUtilizados = numeroNaviosAtendidos = 0;
  totalViolacaoBercos = totalViolacaoNavios = 0;

  for (int k = 0; k < numeroBercos; k++) {
    if (solucao.atendimentoBercos[k].tamanho > 0) {
      numeroBercosUtilizados++;
    }
  }

  for (int i = 0; i < numeroNavios; i++) {
    if (solucao.atendimentoNavios[i] != -1) {
      numeroNaviosAtendidos++;
    }
  }

  cout << "==== Solução ====" << endl;
  cout << "Número de berços utilizados.........................: " << numeroBercosUtilizados << endl;
  cout << "Número de navios atendidos..........................: " << numeroNaviosAtendidos << endl;
  cout << "Total de viol. nas jan. de tempo dos bercos.........: " << totalViolacaoBercos << endl;
  cout << "Total de viol. nas jan. de tempo dos navios.........: " << totalViolacaoNavios << endl;
  cout << "Tempo total de operação.............................: " << solucao.tempoAtendimentoTotal << endl;

  cout << endl << "Atendimentos: " << endl;
  for(int i = 0; i < numeroNavios; i++) {
    cout << "Navio " << i + 1 << " -> berço " << solucao.atendimentoNavios[i] + 1 << ": ";
    cout << endl;
  }
}

void lerInstancia (char* nomeInstancia) {
  FILE* instancia = fopen(nomeInstancia, "r");

  string erro = "Erro na leitura do arquivo";

  if (!fscanf(instancia, "%d %d", &numeroNavios, &numeroBercos)) {
     cout << erro << endl;
  }

  for (int k = 0; k < numeroBercos; k++) {
    for (int i = 0; i < numeroNavios; i++ ) {
      if (!fscanf(instancia, "%d", &duracaoAtendimento[k][i])) {
         cout << erro << endl;
      }
    }
  }

  for (int k = 0; k < numeroBercos; k++) {
    if (!fscanf(instancia, "%d %d", &aberturaFechamento[k][ABERTURA], &aberturaFechamento[k][FECHAMENTO])) {
      cout << erro << endl;
    }
  }

  for (int i = 0; i < numeroNavios; i++) {
    if (!fscanf(instancia, "%d", &momentoChegadaNavio[i])) {
       cout << erro << endl;
    }
  }

  for (int i = 0; i < numeroNavios; i++) {
    if (!fscanf(instancia, "%d", &momentoSaidaNavio[i])) {
       cout << erro << endl;
    }
  }
}
