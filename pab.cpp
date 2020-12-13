#include <iostream>
#include <string>
#include <memory.h>
#include <climits>
#include "pab.hpp"

using namespace std;

#define MAX(X,Y) ((X > Y) ? X : Y)

// #define DEBUG

int main (int argc, char **argv) {
  int tempoMaximo, numeroExecucoes;
  char* instancia;

  tempoMaximo = 5;
  numeroExecucoes = 3;

  #ifdef DEBUG
    instancia = "instancias/i01.txt"
    lerInstancia(instancia);
  #else
    if (argc < 2) {
      cout << "#ERRO: Informe os parâmetros corretamente" << endl;
      cout << " $ ./exe caminho_instância [tempo_processamento [numero_execuções]]" << endl;
      return 0;
    }

    instancia = argv[1];
    tempoMaximo = argc >= 3 ? atoi(argv[2]) : tempoMaximo;
    numeroExecucoes = argc == 4 ? atoi(argv[3]) : numeroExecucoes;

    lerInstancia(instancia);
  #endif

  Solucao solucao;
  double tempoTotal, momentoMelhorSolucao;
  int seed, melhorFo, somaFo;
  double melhorTempoSoma, tempoSoma;

  melhorFo = INT_MAX;
  melhorTempoSoma = tempoSoma = somaFo = 0;

  escreverCabecalhoLog(instancia);
  for (int i = 0; i < numeroExecucoes; i++) {
    cout << "===== Execução " << i + 1 << " =====" << endl << endl;
    seed = time(NULL);
    srand(seed);

    buscaTabu(solucao, 1000, tempoMaximo, tempoTotal, momentoMelhorSolucao);

    tempoSoma += tempoTotal;
    melhorTempoSoma += momentoMelhorSolucao;
    somaFo += solucao.tempoAtendimentoTotal;

    if (solucao.tempoAtendimentoTotal < melhorFo) {
      melhorFo = solucao.tempoAtendimentoTotal;
    }

    atualizarExecucaoLog(solucao, i + 1, seed, tempoTotal, momentoMelhorSolucao);

    cout << "Solução: " << solucao.tempoAtendimentoTotal << endl << endl;
  }

  escreverMediasLog(numeroExecucoes, melhorFo, somaFo, tempoSoma, melhorTempoSoma);

  return 0;
}

void escreverMediasLog (int numeroExecucoes, int melhorFo, int somaFo, double tempoSoma, double melhorTempoSoma) {
  double mediaFo = somaFo / numeroExecucoes;
  double desvio = 100 * (mediaFo - melhorFo) / melhorFo;
  double tempoMedio = tempoSoma / numeroExecucoes;
  double mediaMelhorTempo = melhorTempoSoma / numeroExecucoes;

  FILE *logFile = fopen("log", "a");

  fprintf(logFile, "\nMELHOR FO...........: %d", melhorFo);
  fprintf(logFile, "\nFO MÉDIA............: %f", mediaFo);
  fprintf(logFile, "\nDESVIO..............: %f", desvio);
  fprintf(logFile, "\nTEMPO MÉDIO.........: %f", tempoMedio);
  fprintf(logFile, "\nMÉDIA MELHOR TEMPO..: %f", mediaMelhorTempo);
  fprintf(logFile, "\n\n----------------------------------------------------------------------\n\n");

  fclose(logFile);
}

void escreverCabecalhoLog (char *instancia) {
  FILE* logFile = fopen("log", "a");
  fprintf(logFile, "INSTÂNCIA: %s\n\n", instancia);
  fprintf(logFile, "N⁰\tFO\t\tTEMPO TOTAL\t\tTEMPO MELHOR\tSEED\n");
  fclose(logFile);
}

void atualizarExecucaoLog (Solucao &solucao, int it, int seed, double tempoTotal, double tempoMelhor) {
  FILE* logFile = fopen("log", "a");
  fprintf(logFile, "%d\t\t%d\t%f\t\t\t%f\t\t\t%d\n", it, solucao.tempoAtendimentoTotal, tempoTotal, tempoMelhor, seed);
  fclose(logFile);
}

// --- busca tabu
void buscaTabu (Solucao &solucao, const int tamanhoLista, const double tempoMaximo, double &tempoTotal, double &momentoMelhorSolucao) {
  cout << "-> Executando busca tabu" << endl;

  clock_t clockInicial, clockAtual;
  Solucao solucaoVizinha;

  // gerando solução inicial
  clockInicial = clock();
  heuristicaConstrutiva(solucao);
  calcularFO(solucao);
  clockAtual = clock();

  cout << "-> Solução inicial: " << solucao.tempoAtendimentoTotal << endl << endl;

  momentoMelhorSolucao = calcularTempo(clockInicial, clockAtual);
  tempoTotal = momentoMelhorSolucao;

  clonarSolucao(solucao, solucaoVizinha);

  // ---- lista tabu - berco 0, navio 1
  int quantidadeElementosLista = 0;
  int **listaTabu = new int*[2];
  listaTabu[0] = new int[tamanhoLista];
  listaTabu[1] = new int[tamanhoLista];

  int bercoOriginal, tempoOriginal;
  int melhorTempo, melhorNavio, melhorBerco, melhorPosicao;
  int flag, posicaoNaLista;

  while(tempoTotal < tempoMaximo) {
    melhorTempo = INT_MAX;

    flag = -1;
    for (int i = 0; i < numeroNavios; i++) {
      bercoOriginal = solucaoVizinha.atendimentoNavios[i];
      tempoOriginal = solucaoVizinha.tempoAtendimentoTotal;

      for (int k = 0; k < numeroBercos; k++) {
        if (k != solucaoVizinha.atendimentoNavios[i] && duracaoAtendimento[k][i] != 0) {
          inserirAtendimento(solucaoVizinha, k, i);

          calcularFO(solucaoVizinha);
          posicaoNaLista = procurarNaLista(listaTabu, quantidadeElementosLista, k, i);
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
      melhorBerco = listaTabu[0][0];
      melhorNavio = listaTabu[1][0];
      inserirAtendimento(solucaoVizinha, melhorBerco, melhorNavio);
      calcularFO(solucaoVizinha);
      removerDaLista(listaTabu, quantidadeElementosLista, 0);
    } else {
      if (flag == 0) {
        removerDaLista(listaTabu, quantidadeElementosLista, melhorPosicao);
      } else {
        inserirNaLista(listaTabu, quantidadeElementosLista, tamanhoLista, melhorNavio, melhorBerco);
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

  delete[] listaTabu[0];
  delete[] listaTabu[1];
}

double calcularTempo (clock_t &clockInicial, clock_t &clockAtual) {
  return (double)(clockAtual - clockInicial) / CLOCKS_PER_SEC;
}

int procurarNaLista (int **lista, int quantidadeElementos, const int navio, const int berco) {
  for (int i = 0; i < quantidadeElementos; i++) {
    if (lista[0][i] == berco && lista[1][i] == navio) {
      return i;
    }
  }

  return -1;
}

void removerDaLista (int **lista, int &quantidadeElementos, const int posicao) {
  for (int i = posicao+1; i < quantidadeElementos; i++) {
    lista[0][i-1] = lista[0][i];
    lista[1][i-1] = lista[1][i];
  }

  quantidadeElementos--;
}

void inserirNaLista (int **lista, int &quantidadeElementos, int tamanho, const int navio, const int berco) {
  // se a lista estiver cheia remove o primeiro elemento que entrou (FIFO)
  if (quantidadeElementos == tamanho) {
    removerDaLista(lista, quantidadeElementos, 0);
  }

  lista[0][quantidadeElementos] = berco;
  lista[1][quantidadeElementos] = navio;
  quantidadeElementos++;
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

int calcularFoBerco (Solucao &solucao, int berco) {
  int foBerco, navio, momentoAtracamentoNavio, proximoHorarioDisponivelBerco;

  foBerco = 0;
  proximoHorarioDisponivelBerco = aberturaFechamento[berco][ABERTURA];
  for (int i = 0; i < solucao.atendimentoBercos[berco].tamanho; i++) {
    navio = solucao.atendimentoBercos[berco].navios[i];

    if (navio != -1) {
      momentoAtracamentoNavio = MAX(proximoHorarioDisponivelBerco, momentoChegadaNavio[navio]);
      proximoHorarioDisponivelBerco = momentoAtracamentoNavio + duracaoAtendimento[berco][navio];

      foBerco += momentoAtracamentoNavio - momentoChegadaNavio[navio] + duracaoAtendimento[berco][navio];

      if (proximoHorarioDisponivelBerco > momentoSaidaNavio[navio]) {
        foBerco += PENALIDADE_HORARIO_LIMITE_NAVIO;
      }
    } else {
      foBerco += PENALIDADE_NAVIO_NAO_ATENDIDO;
    }
  }

  if (proximoHorarioDisponivelBerco > aberturaFechamento[berco][FECHAMENTO]) {
    foBerco += PENALIDADE_HORARIO_LIMITE_BERCO;
  }

  return foBerco;
}

void calcularFO(Solucao &solucao) {
  solucao.tempoAtendimentoTotal = 0;
  for (int k = 0; k < numeroBercos; k++) {
    solucao.tempoAtendimentoTotal += calcularFoBerco(solucao, k);
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

  fclose(instancia);
}
