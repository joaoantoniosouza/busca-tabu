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
    char* nomeInstancia = "instancias/i05.txt";
  #else
    if (argc < 2) {
      cout << "é nessesário informar um instância." << endl;
      return 1;
    }

    char* nomeInstancia = argv[1];
  #endif

  // melhorFO = 2771;
  // seed = 17081198;
  srand(110);

  #ifdef TEST
    executarTestes(10000, nomeInstancia);
  #else
    Solucao solucao;
    double tempoTotal, melhorTempo;
    lerInstancia(nomeInstancia);
    // construtivaAleatoria(solucao);
    // calcularFO(solucao);
    buscaTabu(solucao, 100000, 10, tempoTotal, melhorTempo);
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

  printf("Solução inicial: %d\n", solucao.tempoTotalAtendimento);

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
      bercoOriginal = solucaoVizinha.atendimentoNavios[i];
      tempoOriginal = solucaoVizinha.tempoTotalAtendimento;

      // ! porque k = -1?
      for (int k = 0; k < numeroBercos; k++) {
        if (k != solucaoVizinha.atendimentoNavios[i]) {
          adicionarAtendimento(solucaoVizinha, k, i);

          calcularFO(solucaoVizinha);
          posicaoNaLista = procurarNaLista(listaTabu, i, k);
          if (posicaoNaLista != -1) {
            if (solucaoVizinha.tempoTotalAtendimento < solucao.tempoTotalAtendimento) {
              flag = 0;
              melhorTempo = solucaoVizinha.tempoTotalAtendimento;
              melhorBerco = k;
              melhorNavio = i;
              melhorPosicao = posicaoNaLista;
            }
          } else {
            if (solucaoVizinha.tempoTotalAtendimento < melhorTempo) {
              flag = 1;
              melhorTempo = solucaoVizinha.tempoTotalAtendimento;
              melhorBerco = k;
              melhorNavio = i;
              melhorPosicao = -1;
            }
          }
        }
      }

      adicionarAtendimento(solucaoVizinha, bercoOriginal, i);
      solucaoVizinha.tempoTotalAtendimento = tempoOriginal;
    }

    // --- atualiza a lista tabu
    if (flag == -1) {
      melhorBerco = listaTabu.bercos[0];
      melhorNavio = listaTabu.navios[0];
      adicionarAtendimento(solucaoVizinha, melhorBerco, melhorNavio);
      calcularFO(solucaoVizinha);
      removerDaLista(listaTabu, 0);
    } else {
      if (flag == 0) {
        removerDaLista(listaTabu, melhorPosicao);
      } else {
        inserirNaLista(listaTabu, melhorNavio, melhorBerco);
      }

      adicionarAtendimento(solucaoVizinha, melhorBerco, melhorNavio);
      solucaoVizinha.tempoTotalAtendimento = melhorTempo;
    }
    // ---

    if (solucaoVizinha.tempoTotalAtendimento < solucao.tempoTotalAtendimento) {
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


void removerAtendimento (Solucao &solucao, int berco, int navio) {
  for (int l = 0; solucao.atendimentoBercos[berco][l] != -1; l++) {
    if (solucao.atendimentoBercos[berco][l] == navio) {
      for (int j = l; solucao.atendimentoBercos[berco][j] != -1; j++) {
        solucao.atendimentoBercos[berco][l] = solucao.atendimentoBercos[berco][l + 1];
      }

      break;
    }
  }

  solucao.atendimentoNavios[navio] = -1;
}

void adicionarAtendimento (Solucao &solucao, int berco, int navio) {
  removerAtendimento(solucao, solucao.atendimentoNavios[navio], navio);

  solucao.atendimentoNavios[navio] = berco;

  int i = 0;
  while (solucao.atendimentoBercos[berco][i] != -1) {
    i++;
  }

  solucao.atendimentoBercos[berco][i] = navio;
  solucao.atendimentoBercos[berco][i + 1] = -1;
}

void construtivaAleatoria(Solucao &solucao) {
  int berco, limiteBusca, j;

  for (int k = 0; k < numeroBercos; k++) {
    proximoHorarioDisponivelBerco[k] = aberturaFechamento[k][ABERTURA]; // inicializa horário de atendimento
    solucao.atendimentoBercos[k][0] = -1; // nenhum berço atendeu a navios
  }

  limiteBusca = MAX(100, numeroBercos * numeroNavios);

  for (int i = 0; i < numeroNavios; i++) {
    // encontra um berço para atender o navio i
    for (int l = 0; l < limiteBusca; l++) {

      berco = rand()%(numeroBercos);

      if (tempoAtendimento[berco][i] == 0) {
        berco = -1;
      } else {
        break;
      }
    }
    // ----

    solucao.atendimentoNavios[i] = berco;

    // se o berço atendeu o navio i
    if (berco != -1) {
      // insere o atendimento do berço para o navio i
      j = 0;
      while(solucao.atendimentoBercos[berco][j] != -1) {
        j++;
      }

      solucao.atendimentoBercos[berco][j] = i;
      solucao.atendimentoBercos[berco][j + 1] = -1;
      // ----

      // calcula o momento de atracamento
      solucao.momentoAtracamento[i] = MAX(momentoChegadaNavio[i], proximoHorarioDisponivelBerco[berco]);
      proximoHorarioDisponivelBerco[berco] = solucao.momentoAtracamento[i] + tempoAtendimento[berco][i];
      // ---
    }
  }
}

void calcularFO(Solucao &solucao) {
  int i, k, j; // variáveis de iteração - instanciândo uma única vez.

  solucao.tempoTotalAtendimento = 0;

  for(i = 0; i < numeroNavios; i++) {
    if (solucao.atendimentoNavios[i] != -1) {
      if (tempoAtendimento[solucao.atendimentoNavios[i]][i] != 0) {
        solucao.tempoTotalAtendimento += solucao.momentoAtracamento[i] - momentoChegadaNavio[i] + tempoAtendimento[solucao.atendimentoNavios[i]][i];

        if (solucao.momentoAtracamento[i] < momentoChegadaNavio[i]) {
          solucao.tempoTotalAtendimento += PENALIDADE_ATRACAMENTO_IMPOSSIVEL;
        } else {
          if ((solucao.momentoAtracamento[i] + tempoAtendimento[solucao.atendimentoNavios[i]][i]) > momentoSaidaNavio[i]) {
            solucao.tempoTotalAtendimento += PENALIDADE_HORARIO_LIMITE_NAVIO;
          }
        }
      } else {
        solucao.tempoTotalAtendimento += PENALIDADE_ATENDIMENTO_IMPOSSIVEL;
      }
    } else {
      solucao.tempoTotalAtendimento += PENALIDADE_NAVIO_NAO_ATENDIDO;
    }
  }

  int horarioInicioAtendimentoBase, horarioFimAtendimentoBase;
  int horarioInicioAtendimento, horarioFimAtendimento;
  for(k = 0; k < numeroBercos; k++) {
    if ((proximoHorarioDisponivelBerco[k] - aberturaFechamento[k][FECHAMENTO]) > 0) {
      solucao.tempoTotalAtendimento += PENALIDADE_HORARIO_LIMITE_BERCO;
    }

    if (solucao.atendimentoBercos[k][0] != -1) {
      for (i = 0; (i < numeroNavios && solucao.atendimentoBercos[k][i] != -1); i++) {
        horarioInicioAtendimentoBase = solucao.momentoAtracamento[solucao.atendimentoBercos[k][i]];
        horarioFimAtendimentoBase = solucao.momentoAtracamento[solucao.atendimentoBercos[k][i]] + tempoAtendimento[k][solucao.atendimentoBercos[k][i]];

        j = i + 1;
        while (solucao.atendimentoBercos[k][j] != -1) {
          horarioInicioAtendimento = solucao.momentoAtracamento[solucao.atendimentoBercos[k][j]];
          horarioFimAtendimento = solucao.momentoAtracamento[solucao.atendimentoBercos[k][j]] + tempoAtendimento[k][j];

           /**
            * - - - - - - - - - - - - - - - - -
            *
            * 1 - 100 150
            * 2 -          160   180
            *
            * 1 - 100    150
            * 2 -        150   180
            *
            * 1 -     110  150
            * 2 - 70  110
            *
            * 1 -         110  150
            * 2 - 70 100
            *
            * - - - - - - - - - - - - - - - - -
            * 1 - 100     150
            * 2 - 100 140
            *
            * 1 -     100 150
            * 2 - 70          160
            *
            * 1 - 70          160
            * 2 -     100 150
            *
            * --- condição pensada inicialmente ---
            * ? !(horarioInicioAtendimento >= horarioFimAtendimentoBase || horarioFimAtendimento <= horarioInicioAtendimentoBase)
            *
            */

          if (horarioInicioAtendimento < horarioFimAtendimentoBase && horarioFimAtendimento > horarioInicioAtendimentoBase) {
            solucao.tempoTotalAtendimento += PENALIDADE_SOBREPOSICAO_HORARIO;
          }

          j++;
        }
      }
    }
  }
}

void clonarSolucao(Solucao &original, Solucao &copia) {
  memcpy(&copia, &original, sizeof(original));
}

void escreverSolucao(Solucao &solucao) {
  int numeroBercosUtilizados , numeroNaviosAtendidos;
  int totalViolacaoSaidaNavios, totalViolacaoFechamentoBercos;
  int quantidadeViolacaoSaidaNavios, quantidadeViolacaoFechamentoBercos, quantidadeViolacaoSobreposicao;
  int horarioInicioAtendimentoBase, horarioFimAtendimentoBase;
  int horarioInicioAtendimento, horarioFimAtendimento;
  int diferenca, j;

  numeroBercosUtilizados = numeroNaviosAtendidos = 0;
  totalViolacaoFechamentoBercos = totalViolacaoSaidaNavios = 0;
  quantidadeViolacaoSaidaNavios = quantidadeViolacaoFechamentoBercos = quantidadeViolacaoSobreposicao = 0;

  for (int k = 0; k < numeroBercos; k++) {
    if (proximoHorarioDisponivelBerco[k] - aberturaFechamento[k][ABERTURA] != 0) {
      numeroBercosUtilizados++;
    }

    diferenca = proximoHorarioDisponivelBerco[k] - aberturaFechamento[k][FECHAMENTO];
    if (diferenca > 0) {
      quantidadeViolacaoFechamentoBercos++;
      totalViolacaoFechamentoBercos += diferenca;
    }

    if (solucao.atendimentoBercos[k][0] != -1) {
      for (int i = 0; (i < numeroNavios && solucao.atendimentoBercos[k][i] != -1); i++) {
        horarioInicioAtendimentoBase = solucao.momentoAtracamento[solucao.atendimentoBercos[k][i]];
        horarioFimAtendimentoBase = solucao.momentoAtracamento[solucao.atendimentoBercos[k][i]] + tempoAtendimento[k][solucao.atendimentoBercos[k][i]];

        j = i + 1;
        while (solucao.atendimentoBercos[k][j] != -1) {
          horarioInicioAtendimento = solucao.momentoAtracamento[solucao.atendimentoBercos[k][j]];
          horarioFimAtendimento = solucao.momentoAtracamento[solucao.atendimentoBercos[k][j]] + tempoAtendimento[k][j];

          if (horarioInicioAtendimento < horarioFimAtendimentoBase && horarioFimAtendimento > horarioInicioAtendimentoBase) {
            quantidadeViolacaoSobreposicao++;
          }

          j++;
        }
      }
    };
  }

  for (int i = 0; i < numeroNavios; i++) {
    if (solucao.atendimentoNavios[i] != -1) {
      numeroNaviosAtendidos++;
    }

    diferenca = solucao.momentoAtracamento[i] + tempoAtendimento[solucao.atendimentoNavios[i]][i] - momentoSaidaNavio[i];
    if (diferenca > 0) {
      quantidadeViolacaoSaidaNavios++;
      totalViolacaoSaidaNavios += diferenca;
    }
  }

  cout << "==== Solução ====" << endl;
  cout << "Número de berços utilizados.........................: " << numeroBercosUtilizados << endl;
  cout << "Número de navios atendidos..........................: " << numeroNaviosAtendidos << endl;
  cout << "Total de viol. nas jan. de tempo dos bercos.........: " << totalViolacaoFechamentoBercos << endl;
  cout << "Quantidade de viol. nas jan. de tempo dos bercos....: " << quantidadeViolacaoFechamentoBercos << endl;
  cout << "Total de viol. nas jan. de tempo dos navios.........: " << totalViolacaoSaidaNavios << endl;
  cout << "Quantidade de viol. nas jan. de tempo dos navios....: " << quantidadeViolacaoSaidaNavios << endl;
  cout << "Quantidade de viol. de sobreposicao de horários.....: " << quantidadeViolacaoSobreposicao << endl;
  cout << "Tempo total de operação.............................: " << solucao.tempoTotalAtendimento << endl;

  cout << endl << "Atendimentos: " << endl;
  for(int i = 0; i < numeroNavios; i++) {
    cout << "Navio " << i + 1 << " -> berço " << solucao.atendimentoNavios[i] + 1 << ": ";
    cout << "\tHA " << solucao.momentoAtracamento[i] << "\t";
    cout << "HD " << solucao.momentoAtracamento[i] + tempoAtendimento[solucao.atendimentoNavios[i]][i];
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
    fscanf(instancia, "%d", &momentoChegadaNavio[i]);
  }

  for (int i = 0; i < numeroNavios; i++) {
    fscanf(instancia, "%d", &momentoSaidaNavio[i]);
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

  cout << "Valor da FO: " << solucao.tempoTotalAtendimento << endl;
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
