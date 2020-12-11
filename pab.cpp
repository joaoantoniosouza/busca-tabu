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
    lerInstancia("./instancias/i03.txt");
  #else
    if (argc != 3) {
      cout << "#ERRO: Informe os parâmetros corretamente" << endl;
      cout << " $ ./exe caminho_intancia tempo_processamento" << endl;
      return 0;
    }

    lerInstancia(argv[1]);
  #endif

  double tempoTotal, momentoMelhorSolucao;
  buscaTabu(solucao, 100000, 15, tempoTotal, momentoMelhorSolucao);
  cout << "Tempo total: " << tempoTotal << endl;
  cout << "Momento melhor solucao: " << momentoMelhorSolucao << endl;
  escreverSolucao(solucao);

  return 0;
}

// --- busca tabu
void buscaTabu (Solucao &solucao, const int tamanhoLista, const double tempoMaximo, double &tempoTotal, double &momentoMelhorSolucao) {
  cout << "Executando busca tabu..." << endl;

  clock_t clockInicial, clockAtual;
  Solucao solucaoVizinha;

  // gerando solução inicial
  clockInicial = clock();
  heuristicaConstrutiva(solucao);
  calcularFO(solucao);
  clockAtual = clock();

  printf("Solução inicial: %d\n", solucao.tempoAtendimentoTotal);

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
      tempoOriginal = solucaoVizinha.tempoAtendimentoTotal;

      // ! porque k = -1?
      for (int k = 0; k < numeroBercos; k++) {
        if (k != solucaoVizinha.atendimentoNavios[i] && duracaoAtendimento[k][i] != 0) {
          inserirAtendimento(solucaoVizinha, k, i);

          calcularFO(solucaoVizinha);
          posicaoNaLista = procurarNaLista(listaTabu, i, k);
          if (posicaoNaLista != -1) {
            if (solucaoVizinha.tempoAtendimentoTotal < solucao.tempoAtendimentoTotal) {
              flag = 0;
              melhorTempo = solucaoVizinha.tempoAtendimentoTotal;
              melhorBerco = k;
              melhorNavio = i;
              melhorPosicao = posicaoNaLista;
            }
          } else {
            if (solucaoVizinha.tempoAtendimentoTotal < melhorTempo) {
              flag = 1;
              melhorTempo = solucaoVizinha.tempoAtendimentoTotal;
              melhorBerco = k;
              melhorNavio = i;
              melhorPosicao = -1;
            }
          }
        }
      }

      inserirAtendimento(solucaoVizinha, bercoOriginal, i);
      solucaoVizinha.tempoAtendimentoTotal = tempoOriginal;
    }

    // --- atualiza a lista tabu
    if (flag == -1) {
      melhorBerco = listaTabu.bercos[0];
      melhorNavio = listaTabu.navios[0];
      inserirAtendimento(solucaoVizinha, melhorBerco, melhorNavio);
      calcularFO(solucaoVizinha);
      removerDaLista(listaTabu, 0);
    } else {
      if (flag == 0) {
        removerDaLista(listaTabu, melhorPosicao);
      } else {
        inserirNaLista(listaTabu, melhorNavio, melhorBerco);
      }

      inserirAtendimento(solucaoVizinha, melhorBerco, melhorNavio);
      solucaoVizinha.tempoAtendimentoTotal = melhorTempo;
    }
    // ---

    if (solucaoVizinha.tempoAtendimentoTotal < solucao.tempoAtendimentoTotal) {
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
  for (int i = 0; i < lista.tamanho; i++) {
    if (lista.bercos[i] == berco && lista.navios[i] == navio) {
      return i;
    }
  }

  return -1;
}

void removerDaLista (ListaTabu &lista, const int posicao) {
  for (int i = posicao; i < lista.tamanho; i++) {
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
// ---

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
  int navioAtendido;
  removerAtendimento(solucao, navio);

  solucao.atendimentoNavios[navio] = berco;
  solucao.atendimentoBercos[berco].navios[solucao.atendimentoBercos[berco].tamanho] = navio;

  /**
   * Exemplo explicativo
   * berco x - 1 4 8 6 9 10 7
   */
  for (int i = solucao.atendimentoBercos[berco].tamanho; i > 0; i--) {
    navioAtendido = solucao.atendimentoBercos[berco].navios[i - 1];

    if (momentoChegadaNavio[navio] < momentoChegadaNavio[navioAtendido]) {
      solucao.atendimentoBercos[berco].navios[i] = navioAtendido;
      solucao.atendimentoBercos[berco].navios[i - 1] = navio;
    } else {
      if (momentoChegadaNavio[navio] == momentoChegadaNavio[navioAtendido]) {
        if (duracaoAtendimento[berco][navio] < duracaoAtendimento[berco][navioAtendido]) {
          solucao.atendimentoBercos[berco].navios[i] = navioAtendido;
          solucao.atendimentoBercos[berco].navios[i - 1] = navio;
        } else {
          solucao.atendimentoBercos[berco].navios[i] = navio;
          solucao.atendimentoBercos[berco].navios[i - 1] = navioAtendido;
        }
      }
      break;
    }
  }

  solucao.atendimentoBercos[berco].tamanho++;
}

void clonarSolucao (Solucao &original, Solucao &copia) {
  memcpy(&copia, &original, sizeof(original));
}

void escreverSolucao (Solucao &solucao) {
  int numeroBercosUtilizados , numeroNaviosAtendidos;
  int totalViolacaoNavios, totalViolacaoBercos;
  int violacaoNavios[MAX_BERCOS], violacaoBercos[MAX_BERCOS], foBerco[MAX_BERCOS];
  int proximoHorarioDisponivelBerco, momentoAtracamentoNavio, navio;

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

  for (int k = 0; k < numeroBercos; k++) {
    proximoHorarioDisponivelBerco = aberturaFechamento[k][ABERTURA];

    violacaoNavios[k] = violacaoBercos[k] = foBerco[k] = 0;
    for (int i = 0; i < solucao.atendimentoBercos[k].tamanho; i++) {
      navio = solucao.atendimentoBercos[k].navios[i];
      momentoAtracamentoNavio = MAX(proximoHorarioDisponivelBerco, momentoChegadaNavio[navio]);
      proximoHorarioDisponivelBerco = momentoAtracamentoNavio + duracaoAtendimento[k][navio];

      foBerco[k] += momentoAtracamentoNavio - momentoChegadaNavio[navio] + duracaoAtendimento[k][navio];

      if (proximoHorarioDisponivelBerco > momentoSaidaNavio[navio]) {
        totalViolacaoNavios += MAX(proximoHorarioDisponivelBerco - momentoSaidaNavio[navio], 0);
        violacaoNavios[k] += MAX(proximoHorarioDisponivelBerco - momentoSaidaNavio[navio], 0);
      }
    }

    if (proximoHorarioDisponivelBerco > aberturaFechamento[k][FECHAMENTO]) {
      totalViolacaoBercos += MAX(proximoHorarioDisponivelBerco - aberturaFechamento[k][FECHAMENTO], 0);
      violacaoBercos[k] += MAX(proximoHorarioDisponivelBerco - aberturaFechamento[k][FECHAMENTO], 0);
    }
  }

  cout << "==== Solução ====" << endl;
  cout << "Número de berços utilizados.........................: " << numeroBercosUtilizados << endl;
  cout << "Número de navios atendidos..........................: " << numeroNaviosAtendidos << endl;
  cout << "Total de viol. nas jan. de tempo dos bercos.........: " << totalViolacaoBercos << endl;
  cout << "Total de viol. nas jan. de tempo dos navios.........: " << totalViolacaoNavios << endl;
  cout << "Tempo total de operação.............................: " << solucao.tempoAtendimentoTotal << endl;

  cout << endl << "Atendimentos: " << endl;
  for (int k = 0; k < numeroBercos; k++) {
    cout << "Berco " << k + 1 << ":" << endl;
    cout << "  Número de navios atendidos..................: " << solucao.atendimentoBercos[k].tamanho << endl;
    cout << "  Horário de abertura.........................: " << aberturaFechamento[k][ABERTURA] << endl;
    cout << "  Horário de fechamento.......................: " << aberturaFechamento[k][FECHAMENTO] << endl;
    cout << "  Violações nas janelas de tempo do berco.....: " << violacaoBercos[k] << endl;
    cout << "  Violações nas janelas de tempo dos navios...: " << violacaoNavios[k] << endl;
    cout << "  FO do berco.................................: " << foBerco[k] << endl;

    cout << "  Sequencia de atendimento: I -> ";
    for (int i = 0; i < solucao.atendimentoBercos[k].tamanho; i++) {
      cout << solucao.atendimentoBercos[k].navios[i] << " -> ";
    }
    cout << "F" << endl << endl;
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
