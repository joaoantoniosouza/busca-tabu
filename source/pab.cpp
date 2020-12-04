#include <iostream>
#include <memory.h>
#include <string.h>
#include <time.h>
#include "pab.hpp"
#include <climits>

using namespace std;

#define MAX(X,Y) ((X > Y) ? X : Y)

// #define TEST
#define DEBUG

int main (int argc, char **argv) {

  #ifdef DEBUG
    char* nomeInstancia = "instancias/i01.txt";
  #else
    if (argc < 2) {
      cout << "é nessesário informar um instância." << endl;
      return 1;
    }

    char* nomeInstancia = argv[1];
  #endif

  // srand(time(NULL));

  #ifdef TEST
    executarTestes(10000, nomeInstancia);
  #else
    Solucao solucao;
    double tempoTotal, melhorTempo;
    lerInstancia(nomeInstancia);
    // construtivaAleatoria(solucao);
    // calcularFO(solucao);
    buscaTabu(solucao, 100, 100, tempoTotal, melhorTempo);
    escreverSolucao(solucao);
  #endif

  return 0;
}

void buscaTabu (Solucao &solucao, const int tamanhoLista, const double tempoMaximo, double &tempoTotal, double &momentoMelhorSolucao) {
  cout << "Executando busca tabu..." << endl;

  clock_t clockInicial, clockAtual;
  Solucao solucaoVizinha;

  // gerando solução inicial
  clockInicial = clock();
  construtivaAleatoria(solucao);
  calcularFO(solucao);
  clockAtual = clock();

  momentoMelhorSolucao = calcularTempo(clockInicial, clockAtual);
  tempoTotal = momentoMelhorSolucao;

  clonarSolucao(solucao, solucaoVizinha);

  // ---- lista tabu - berco 0, navio 1
  ListaTabu listaTabu;
  listaTabu.bercos = new int[tamanhoLista];
  listaTabu.navios = new int[tamanhoLista];
  listaTabu.quantidadeElementos = 0;
  listaTabu.tamanho = tamanhoLista;

  int bercoOriginal, tempoOriginal;
  int melhorTempo, melhorNavio, melhorBerco, melhorPosicao;
  int flag, posicaoNaLista;

  while(tempoTotal < tempoMaximo) {
    melhorTempo = INT_MAX;

    flag = -1;
    for (int i = 0; i < numeroNavios; i++) {
      bercoOriginal = solucaoVizinha.atendimento[i];
      tempoOriginal = solucaoVizinha.tempoTotal;

      for (int k = 0; k < numeroBercos; k++) {
        if (k == solucaoVizinha.atendimento[i]) continue;

        solucaoVizinha.atendimento[i] = k;
        calcularFO(solucaoVizinha);
        posicaoNaLista = procurarNaLista(listaTabu, i, k);

        if (posicaoNaLista == -1) {
          if (solucaoVizinha.tempoTotal < melhorTempo) {
            flag = 1;
            melhorPosicao = -1;
            melhorTempo = solucaoVizinha.tempoTotal;
            melhorBerco = k;
            melhorNavio = i;
          }
        } else {
          if (solucaoVizinha.tempoTotal < solucao.tempoTotal) {
            flag = 0;
            melhorPosicao = posicaoNaLista;
            melhorTempo = solucaoVizinha.tempoTotal;
            melhorBerco = k;
            melhorNavio = i;
          }
        }
      }

      solucaoVizinha.atendimento[i] = bercoOriginal;
      solucaoVizinha.tempoTotal = tempoOriginal;
    }

    if (flag == -1) {
      melhorBerco = listaTabu.bercos[0];
      melhorNavio = listaTabu.navios[0];
      solucaoVizinha.atendimento[melhorNavio] = melhorBerco;
      calcularFO(solucaoVizinha);
      removerDaLista(listaTabu, 0);
    } else {
      if (flag == 0) {
        removerDaLista(listaTabu, melhorPosicao);
      } else {
        inserirNaLista(listaTabu, melhorNavio, melhorBerco);
      }
    }

    if (solucaoVizinha.tempoTotal < solucao.tempoTotal) {
      clonarSolucao(solucaoVizinha, solucao);
      clockAtual = clock();
      momentoMelhorSolucao = calcularTempo(clockInicial, clockAtual);
    }

    // ----
    clockAtual = clock();
    tempoTotal = calcularTempo(clockInicial, clockAtual);
  }

  delete[] listaTabu.bercos;
  delete[] listaTabu.navios;
}

double calcularTempo (clock_t &clockInicial, clock_t &clockAtual) {
  return (double)(clockAtual - clockInicial) / CLOCKS_PER_SEC;
}

int procurarNaLista (ListaTabu &lista, const int navio, const int berco) {
  for (int i = 0; i < lista.quantidadeElementos; i++) {
    if (lista.bercos[i] == berco && lista.navios[i] == navio) {
      return i;
    }
  }

  return -1;
}

void removerDaLista (ListaTabu &lista, const int posicao) {
  for (int i = posicao; i < lista.quantidadeElementos; i++) {
    lista.bercos[i] = lista.bercos[i + 1];
    lista.navios[i] = lista.navios[i + 1];
  }

  lista.quantidadeElementos--;
}

void inserirNaLista (ListaTabu &lista, const int navio, const int berco) {
  // se a lista estiver cheia remove o primeiro elemento que entrou (FIFO)
  if (lista.quantidadeElementos == lista.tamanho) {
    removerDaLista(lista, 0);
  }

  lista.bercos[lista.quantidadeElementos] = berco;
  lista.navios[lista.quantidadeElementos] = navio;
  lista.quantidadeElementos++;
}

void construtivaAleatoria(Solucao &solucao) {
  int berco, limiteBusca;

  for (int k = 0; k < numeroBercos; k++) {
    proximoHorarioDisponivelBerco[k] = aberturaFechamento[k][ABERTURA];
  }

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

void lerInstancia (char* nomeInstancia) {
  FILE* instancia = fopen(nomeInstancia, "r");

  fscanf(instancia, "%d %d", &numeroNavios, &numeroBercos);

  for (int k = 0; k < numeroBercos; k++) {
    for (int i = 0; i < numeroNavios; i++ ) {
      fscanf(instancia, "%d", &tempoAtendimento[k][i]);
    }
  }

  for (int k = 0; k < numeroBercos; k++) {
    fscanf(instancia, "%d %d", &aberturaFechamento[k][ABERTURA], &aberturaFechamento[k][FECHAMENTO]);
  }

  for (int i = 0; i < numeroNavios; i++) {
    fscanf(instancia, "%d", &tempoChegadaNavio[i]);
  }

  for (int i = 0; i < numeroNavios; i++) {
    fscanf(instancia, "%d", &tempoSaidaNavio[i]);
  }
}

void executarTestes (int repeticoes, char* nomeInstancia) {
  Solucao solucao;
  clock_t h;
  double tempo;

  lerInstancia(nomeInstancia);

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
