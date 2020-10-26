#include <iostream>
#include <memory.h>
#include <string.h>
#include <time.h>
#include "pab.hpp"

using namespace std;

#define MAX(X,Y) ((X > Y) ? X : Y)

int main (int argc, char **argv) {
  Solucao solucao;

  // srand(time(NULL));

  lerInstancia();

  // ordenarNaviosPorChegada(ordemChegadaNavios, 0, numeroNavios);

  // for (int i = 0; i < numeroNavios; i++) {
  //   cout << i << " " << ordemChegadaNavios[i] << " " << tempoChegadaNavios[ordemChegadaNavios[i]] << endl;
  // }

  construtivaGulosa(solucao);
  // construtivaAleatoria(solucao);
  calcularFO(solucao);
  escreverSolucao(solucao);

  // executarTestes(10000);

  return 0;
}

void construtivaAleatoria (Solucao &solucao) {
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
    solucao.tempoAtracamento[i] = MAX(tempoChegadaNavios[i], proximoTempoDisponivelBerco[berco]);
    proximoTempoDisponivelBerco[berco] = solucao.tempoAtracamento[i] + tempoAtendimento[berco][i];
  }
}

void construtivaGulosa (Solucao &solucao) {
  int bercoMenorTempo;
  ordenarNaviosPorChegada(ordemChegadaNavios, 0, numeroNavios);

  for (int i = 0; i < numeroNavios; i++) {
    solucao.atendimento[ordemChegadaNavios[i]] = -1;
    solucao.tempoAtracamento[ordemChegadaNavios[i]] = -1;

    for (int k = 0; k < numeroBercos; k++) {
      if (tempoAtendimento[k][ordemChegadaNavios[i]] != 0 && proximoTempoDisponivelBerco[k] <= tempoChegadaNavios[ordemChegadaNavios[i]]) {
        solucao.atendimento[ordemChegadaNavios[i]] = k;
        solucao.tempoAtracamento[ordemChegadaNavios[i]] = tempoChegadaNavios[ordemChegadaNavios[i]];
        proximoTempoDisponivelBerco[k] = solucao.tempoAtracamento[ordemChegadaNavios[i]] + tempoAtendimento[k][ordemChegadaNavios[i]];
        break;
      }
    }

    if (solucao.atendimento[ordemChegadaNavios[i]] != -1) continue;

    for (int k = 0; k < numeroBercos; k++) {
      if (tempoAtendimento[k][ordemChegadaNavios[i]] != 0) {
        bercoMenorTempo = k;
      }
    }

    for (int k = 0; k < numeroBercos; k++) {
      if (tempoAtendimento[k][ordemChegadaNavios[i]] != 0 && proximoTempoDisponivelBerco[k] < proximoTempoDisponivelBerco[k+1]) {
        bercoMenorTempo = k;
      }
    }

    if (tempoAtendimento[bercoMenorTempo][ordemChegadaNavios[i]] != 0) {
      solucao.atendimento[ordemChegadaNavios[i]] = bercoMenorTempo;
      solucao.tempoAtracamento[ordemChegadaNavios[i]] = proximoTempoDisponivelBerco[bercoMenorTempo];
      proximoTempoDisponivelBerco[bercoMenorTempo] = solucao.tempoAtracamento[ordemChegadaNavios[i]] + tempoAtendimento[bercoMenorTempo][ordemChegadaNavios[i]];
    }
  }
}

void calcularFO (Solucao &solucao) {
    solucao.tempoTotal = 0;

    for(int i = 0; i < numeroNavios; i++) {
      if (solucao.atendimento[i] != -1) {
        solucao.tempoTotal += solucao.tempoAtracamento[i] - tempoChegadaNavios[i] + tempoAtendimento[solucao.atendimento[i]][i];

        if (solucao.tempoAtracamento[i] < tempoChegadaNavios[i]) {
          cout << "puniu1" << endl;
         solucao.tempoTotal += PENALIDADE_ATRACAMENTO_IMPOSSIVEL;
        } else {
          if ((solucao.tempoAtracamento[i] + tempoAtendimento[solucao.atendimento[i]][i]) > tempoSaidaNavios[i]) {
          cout << "puniu2" << endl;
            solucao.tempoTotal += PENALIDADE_HORARIO_LIMITE_NAVIO;
          }
        }
      } else {
        cout << "puniu3" << endl;
        solucao.tempoTotal += PENALIDADE_NAVIO_NAO_ATENDIDO;
      }
    }

    for(int k = 0; k < numeroBercos; k++) {
      if ((proximoTempoDisponivelBerco[k] - aberturaFechamento[k][FECHAMENTO]) > 0) {
        solucao.tempoTotal += PENALIDADE_HORARIO_LIMITE_BERCO;
      }
    }
}

void clonarSolucao (Solucao &original, Solucao &copia) {
  memcpy(&copia, &original, sizeof(original));
}

void escreverSolucao (Solucao &solucao) {
  int numeroBercosUtilizados , numeroNaviosAtendidos;
  int totalViolacaoNavios, totalViolacaoBercos;
  int diferenca;

  numeroBercosUtilizados = numeroNaviosAtendidos = 0;
  totalViolacaoBercos = totalViolacaoNavios = 0;

  for (int k = 0; k < numeroBercos; k++) {
    if (proximoTempoDisponivelBerco[k] - aberturaFechamento[k][ABERTURA] != 0) {
      numeroBercosUtilizados++;
    }

    diferenca = proximoTempoDisponivelBerco[k] - aberturaFechamento[k][FECHAMENTO];
    if (diferenca > 0) {
      totalViolacaoNavios += diferenca;
    }
  }

  for (int i = 0; i < numeroNavios; i++) {
    if (solucao.atendimento[i] != -1) {
      numeroNaviosAtendidos++;
    }

    diferenca = solucao.tempoAtracamento[i] + tempoAtendimento[solucao.atendimento[i]][i] - tempoSaidaNavios[i];
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
    if (solucao.atendimento[i] != -1) {
      cout << "Navio " << i + 1 << " -> berço " << solucao.atendimento[i] + 1 << ": ";
      cout << "\tHA " << solucao.tempoAtracamento[i] << "\t";
      cout << "HD " << solucao.tempoAtracamento[i] + tempoAtendimento[solucao.atendimento[i]][i];
    } else {
      cout << "Navio " << i + 1 << " -> não atendido";
    }

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
    proximoTempoDisponivelBerco[k] = aberturaFechamento[k][ABERTURA];
  }

  for (int i = 0; i < numeroNavios; i++) {
    cin >> tempoChegadaNavios[i];
    // inserindo desordenado;
    ordemChegadaNavios[i] = i;
  }

  for (int i = 0; i < numeroNavios; i++) {
    cin >> tempoSaidaNavios[i];
  }
}

void particao(int *vetor, int esquerda, int direita, int *esq, int *dir) {
    int pivo, aux;

    *esq = esquerda;
    *dir = direita;

    pivo = tempoChegadaNavios[vetor[(esquerda + direita) / 2]];

    while(*esq <= *dir) {

        while(tempoChegadaNavios[vetor[*esq]] < pivo && *esq < direita) {
            (*esq)++;
        }

        while(tempoChegadaNavios[vetor[*dir]] > pivo && *dir > esquerda) {
            (*dir)--;
        }

        if(*esq <= *dir) {
            aux = vetor[*esq];
            vetor[*esq] = vetor[*dir];
            vetor[*dir] = aux;
            (*esq)++;
            (*dir)--;
        }
    }
}

void ordenarNaviosPorChegada(int *vetor, int esquerda, int direita) {
    int esq, dir;

    particao(vetor, esquerda, direita, &esq, &dir);

    if(dir > esquerda) {
        ordenarNaviosPorChegada(vetor, esquerda, dir);
    }

    if(esq < direita) {
        ordenarNaviosPorChegada(vetor,  esq, direita);
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
