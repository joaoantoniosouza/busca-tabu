#include <iostream>
#include <memory.h>
#include <string.h>
#include <time.h>
#include "pab.hpp"

using namespace std;

#define MAX(X,Y) ((X > Y) ? X : Y)

#define TEST

int main (int argc, char **argv) {

  // srand(time(NULL));
  
  #ifdef TEST
    executarTestes(10000);
  #else
    Solucao solucao;
    lerInstancia();
    construtivaAleatoria(solucao);
    calcularFO(solucao);
    escreverSolucao(solucao);
  #endif

  return 0;
}

void construtivaAleatoria(Solucao &solucao) {
  int berco, limiteBusca;

  limiteBusca = MAX(100, numeroBercos * numeroNavios);

  for (int i = 0; i < numeroNavios; i++) {
    for (int l = 0; l < limiteBusca; l++) {

      berco = rand()%(numeroBercos);

      if (tempoAtendimento[berco][i] == 0) {
        berco = -1;
      } else {
        break;
      }
    }

    solucao.atendimento[i] = berco;
   
    if (berco != -1) {
      solucao.tempoAtracamento[i] = MAX(tempoChegadaNavio[i], proximoHorarioDisponivelBerco[berco]);
      proximoHorarioDisponivelBerco[berco] = solucao.tempoAtracamento[i] + tempoAtendimento[berco][i];
    }
  }
}

void calcularFO(Solucao &solucao) {
    solucao.tempoTotal = 0; 

    for(int i = 0; i < numeroNavios; i++) {
      if (solucao.atendimento[i] != -1) {
        solucao.tempoTotal += solucao.tempoAtracamento[i] - tempoChegadaNavio[i] + tempoAtendimento[solucao.atendimento[i]][i];

        if (solucao.tempoAtracamento[i] < tempoChegadaNavio[i]) {
         solucao.tempoTotal += PENALIDADE_ATRACAMENTO_IMPOSSIVEL;
        } else {
          if ((solucao.tempoAtracamento[i] + tempoAtendimento[solucao.atendimento[i]][i]) > tempoSaidaNavio[i]) {
            solucao.tempoTotal += PENALIDADE_HORARIO_LIMITE_NAVIO;
          }
        }
      } else {
        solucao.tempoTotal += PENALIDADE_NAVIO_NAO_ATENDIDO;
      }
    }

    for(int k = 0; k < numeroBercos; k++) {
      if ((proximoHorarioDisponivelBerco[k] - aberturaFechamento[k][FECHAMENTO]) > 0) {
        solucao.tempoTotal += PENALIDADE_HORARIO_LIMITE_BERCO;
      }
    }
}

void clonarSolucao(Solucao &original, Solucao &copia) {
  memcpy(&copia, &original, sizeof(original));
}

void escreverSolucao(Solucao &solucao) {
  int numeroBercosUtilizados , numeroNaviosAtendidos;
  int totalViolacaoNavios, totalViolacaoBercos;
  int diferenca;
  
  numeroBercosUtilizados = numeroNaviosAtendidos = 0;
  totalViolacaoBercos = totalViolacaoNavios = 0;

  for (int k = 0; k < numeroBercos; k++) {
    if (proximoHorarioDisponivelBerco[k] - aberturaFechamento[k][ABERTURA] != 0) {
      numeroBercosUtilizados++;
    }

    diferenca = proximoHorarioDisponivelBerco[k] - aberturaFechamento[k][FECHAMENTO];
    if (diferenca > 0) {
      totalViolacaoNavios += diferenca;
    }
  }

  for (int i = 0; i < numeroNavios; i++) {
    if (solucao.atendimento[i] != -1) {
      numeroNaviosAtendidos++;
    }
    
    diferenca = solucao.tempoAtracamento[i] + tempoAtendimento[solucao.atendimento[i]][i] - tempoSaidaNavio[i];
    if (diferenca > 0) {
      totalViolacaoNavios += diferenca;
    }
  }

  cout << "==== Solução ====" << endl;
  cout << "Número de berços utilizados.........................: " << numeroBercosUtilizados << endl;
  cout << "Número de navios atendidos..........................: " << numeroNaviosAtendidos << endl;
  cout << "Total de viol. nas jan. de tempo dos bercos.........: " << totalViolacaoBercos << endl;
  cout << "Total de viol. nas jan. de tempo dos navios.........: " << totalViolacaoNavios << endl;
  cout << "Tempo total de operação.............................: " << solucao.tempoTotal << endl;
  
  cout << endl << "Atendimentos: " << endl;
  for(int i = 0; i < numeroNavios; i++) {
    cout << "Navio " << i + 1 << " -> berço " << solucao.atendimento[i] + 1 << ": "; 
    cout << "\tHA " << solucao.tempoAtracamento[i] << "\t";
    cout << "HD " << solucao.tempoAtracamento[i] + tempoAtendimento[solucao.atendimento[i]][i];
    cout << endl;
  }
}

void lerInstancia () {
  cin >> numeroNavios >> numeroBercos;
  
  for (int k = 0; k < numeroBercos; k++) {
    for (int i = 0; i < numeroNavios; i++ ) {
      cin >> tempoAtendimento[k][i];
    }
  }

  for (int k = 0; k < numeroBercos; k++) {
    cin >> aberturaFechamento[k][ABERTURA] >> aberturaFechamento[k][FECHAMENTO];
    proximoHorarioDisponivelBerco[k] = aberturaFechamento[k][ABERTURA];
  }

  for (int i = 0; i < numeroNavios; i++) {
    cin >> tempoChegadaNavio[i];
  }

  for (int i = 0; i < numeroNavios; i++) {
    cin >> tempoSaidaNavio[i];
  }
}

void executarTestes (int repeticoes) {
  Solucao solucao;
  clock_t h;
  double tempo;

  lerInstancia();

  h = clock();
  construtivaAleatoria(solucao);
  h = clock() - h;
  tempo = (double) h / CLOCKS_PER_SEC;
 
  calcularFO(solucao);

  cout << "Valor da FO: " << solucao.tempoTotal << endl;
  cout << "Tempo para gerar solução: " << tempo << " seg." << endl;

  h = clock();
  for (int t = 0; t < repeticoes; t++) {
    construtivaAleatoria(solucao);  
  }
  h = clock() - h;
  tempo = (double) h / CLOCKS_PER_SEC;

  cout << "Tempo de execução para " << repeticoes << " repetições:" << endl;

  cout << "\tHeuristica contrutiva: " << tempo << " seg." << endl;

  h = clock();
  for (int t = 0; t < repeticoes; t++) {
    calcularFO(solucao);  
  }
  h = clock() - h;
  tempo = (double) h / CLOCKS_PER_SEC;

  cout << "\tCalculo FO: " << tempo << " seg." << endl;
}
