#include <iostream>
#include <string>
#include <climits>
#include <memory.h>
#include <math.h>
#include "pab.hpp"

using namespace std;

#define MAX(X,Y) ((X > Y) ? X : Y)
#define MIN(X, Y) ((X < Y) ? X : Y)

int main (int argc, char **argv) {
  #ifndef TEST
    char* instancia;

    if (argc == 1) {
      cout << " =====================================================" << endl;
      cout << "Comando para execução parametrizada: " << endl;
      cout << "   $ ./exe instancia tamanho_lista tempo_processamento numero_exec maximo_troca potencia_troca seed" << endl;
      cout << endl << "Todos os parâmetros são opcionais." << endl;
      cout << "Default" << endl;
      cout << "   Instância: i01.txt." << endl;
      cout << "   Numero de execuções:" << numeroExecucoes << "." << endl;
      cout << "   Tempo máximo:" << tempoExecucaoMaximo << " seg." << endl;
      cout << "   Seed aleatória." << endl;
      cout << " =====================================================" << endl << endl;
    }

    instancia = argc > 1 ? argv[1] : (char*) "instancias/i01.txt";
    tamanhoListaTabu = argc >= 3 ? atoi(argv[2]) : tamanhoListaTabu;
    tempoExecucaoMaximo = argc >= 3 ? atof(argv[3]) : tempoExecucaoMaximo;
    numeroExecucoes = argc >= 4 ? atoi(argv[4]) : numeroExecucoes;
    maximoTrocas = argc >= 5 ? atoi(argv[5]) : maximoTrocas;
    potenciaDeTroca = argc >= 6 ? atoi(argv[6]) : potenciaDeTroca;

    cout << "Executando..." << endl << endl;
    cout << "Instância: " << instancia << endl;
    cout << "Tamanho da lista: " << tamanhoListaTabu << endl;
    cout << "Tempo máximo: " << tempoExecucaoMaximo << endl;
    cout << "Número de execuções: " << numeroExecucoes << endl;
    cout << "Máximo de trocas: " << maximoTrocas << endl;
    cout << "Potência de trocas: " << potenciaDeTroca << endl << endl;

    lerInstancia(instancia);

    Solucao solucao;
    double tempoTotal, momentoMelhorSolucao;
    int solucaoInicial, melhorFo, somaFo;
    double melhorTempoSoma, tempoSoma;

    melhorFo = INT_MAX;
    melhorTempoSoma = tempoSoma = somaFo = 0;

    escreverCabecalhoLog(instancia);
    for (int i = 0; i < numeroExecucoes; i++) {
      cout << "===== Execução " << i + 1 << " =====" << endl << endl;

      seed = argc == 8 ? atoi(argv[7]) : clock();

      srand(seed);
      buscaTabu(solucao, tamanhoListaTabu, tempoExecucaoMaximo, tempoTotal, momentoMelhorSolucao, solucaoInicial);

      tempoSoma += tempoTotal;
      melhorTempoSoma += momentoMelhorSolucao;
      somaFo += solucao.tempoAtendimentoTotal;

      if (solucao.tempoAtendimentoTotal < melhorFo) {
        melhorFo = solucao.tempoAtendimentoTotal;
      }

      atualizarExecucaoLog(solucao, seed, tempoTotal, momentoMelhorSolucao, solucaoInicial);

      cout << "Solução: " << solucao.tempoAtendimentoTotal << endl << endl;
    }

    escreverMediasLog(numeroExecucoes, melhorFo, somaFo, tempoSoma, melhorTempoSoma);
  #else
    lerInstancia("instancias/i01.txt");
    testeEstrategiasInsercao();
  #endif

  return 0;
}

void escreverMediasLog (int numeroExec, int melhorFo, int somaFo, double tempoSoma, double melhorTempoSoma) {
  double mediaFo = somaFo / numeroExec;
  double desvio = 100 * (mediaFo - melhorFo) / melhorFo;
  double tempoMedio = tempoSoma / numeroExec;
  double mediaMelhorTempo = melhorTempoSoma / numeroExec;

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
  fprintf(logFile, "INSTÂNCIA: %s\n", instancia);
  fprintf(logFile, "TAMANHO LISTA: %d\n", tamanhoListaTabu);
  fprintf(logFile, "MÁXIMO TROCAS: %d\n", maximoTrocas);
  fprintf(logFile, "POTÊNCIA DE TROCA: %d\n\n", potenciaDeTroca);
  fprintf(logFile, "SOLUÇÃO INICIAL\t\tFO\t\t\tTEMPO TOTAL\t\tTEMPO MELHOR\t\tSEED\n");
  fclose(logFile);
}

void atualizarExecucaoLog (Solucao &solucao, int seedUtilizada, double tempoTotal, double tempoMelhor, int solucaoInicial) {
  FILE* logFile = fopen("log", "a");
  fprintf(logFile, "%d\t\t\t\t\t\t\t%d\t\t%f\t\t\t%f\t\t\t\t%d\n", solucaoInicial, solucao.tempoAtendimentoTotal, tempoTotal, tempoMelhor, seedUtilizada);
  fclose(logFile);
}

// --- busca tabu
void buscaTabu (Solucao &solucao, const int tamanhoLista, const double tempoMaximo, double &tempoTotal, double &momentoMelhorSolucao, int &solucaoInicial) {
  cout << "-> Executando busca tabu" << endl;

  clock_t clockInicial, clockAtual;
  Solucao solucaoVizinha;

  // gerando solução inicial
  clockInicial = clock();
  heuristicaConstrutiva(solucao);
  calcularFO(solucao);
  clockAtual = clock();

  momentoMelhorSolucao = calcularTempo(clockInicial, clockAtual);
  tempoTotal = momentoMelhorSolucao;

  solucaoInicial = solucao.tempoAtendimentoTotal;
  cout << "-> Solução inicial: " << solucaoInicial << endl << endl;

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

    ordenarBerco(solucao, berco);
    fazerTrocasAleatorias(solucao, berco);
  }
}

void inserirAtendimento (Solucao &solucao, int berco, int navio) {
  removerAtendimento(solucao, navio);

  solucao.atendimentoNavios[navio] = berco;
  solucao.atendimentoBercos[berco].navios[solucao.atendimentoBercos[berco].tamanho] = navio;
  solucao.atendimentoBercos[berco].tamanho++;

  ordenarBerco(solucao, berco);
  fazerTrocasAleatorias(solucao, berco);
}

void ordenarBerco (Solucao &solucao, int berco) {
  int j;
  int aux;
  for (int i = 1; i < solucao.atendimentoBercos[berco].tamanho; i++) {
    aux = solucao.atendimentoBercos[berco].navios[i];
    j = i - 1;

    while ((j >= 0) && (momentoChegadaNavio[aux] < momentoChegadaNavio[solucao.atendimentoBercos[berco].navios[j]] || (momentoChegadaNavio[aux] == momentoChegadaNavio[solucao.atendimentoBercos[berco].navios[j]] && duracaoAtendimento[berco][aux] < duracaoAtendimento[berco][solucao.atendimentoBercos[berco].navios[j]]))) {
      solucao.atendimentoBercos[berco].navios[j+1] = solucao.atendimentoBercos[berco].navios[j];
      j--;
    }

    if (j != (i-1)) solucao.atendimentoBercos[berco].navios[j+1] = aux;
  }
}

void fazerTrocasAleatorias (Solucao &solucao, int berco) {
  int fo, melhorFo, posicao1, posicao2, navioTroca;

  if (solucao.atendimentoBercos[berco].tamanho > 1) {
    srand(seed+berco);

    melhorFo = calcularFoBerco(solucao, berco);

    for (int i = 0; i < MIN(maximoTrocas, pow(solucao.atendimentoBercos[berco].tamanho, potenciaDeTroca)); i++) {
      posicao1 = rand() % solucao.atendimentoBercos[berco].tamanho;
      posicao2 = rand() % solucao.atendimentoBercos[berco].tamanho;

      navioTroca = solucao.atendimentoBercos[berco].navios[posicao1];
      solucao.atendimentoBercos[berco].navios[posicao1] = solucao.atendimentoBercos[berco].navios[posicao2];
      solucao.atendimentoBercos[berco].navios[posicao2] = navioTroca;

      fo = calcularFoBerco(solucao, berco);

      if (fo < melhorFo) {
        melhorFo = fo;
      } else {
        solucao.atendimentoBercos[berco].navios[posicao2] = solucao.atendimentoBercos[berco].navios[posicao1];
        solucao.atendimentoBercos[berco].navios[posicao1] = navioTroca;
      }
    }
  }
}

void removerAtendimentoOrdemChegada (Solucao &solucao, int navioARemover) {
  int berco = solucao.atendimentoNavios[navioARemover];
  int navio, aux;

  if (berco != -1) {
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

void inserirAtendimentoOrdemChegada (Solucao &solucao, int berco, int navio) {
  removerAtendimento(solucao, navio);

  solucao.atendimentoNavios[navio] = berco;
  solucao.atendimentoBercos[berco].navios[solucao.atendimentoBercos[berco].tamanho] = navio;
  solucao.atendimentoBercos[berco].tamanho++;

  int j;
  int aux;
  for (int i = 1; i < solucao.atendimentoBercos[berco].tamanho; i++) {
    aux = solucao.atendimentoBercos[berco].navios[i];
    j = i - 1;

    while ((j >= 0) && (momentoChegadaNavio[aux] < momentoChegadaNavio[solucao.atendimentoBercos[berco].navios[j]] || (momentoChegadaNavio[aux] == momentoChegadaNavio[solucao.atendimentoBercos[berco].navios[j]] && duracaoAtendimento[berco][aux] < duracaoAtendimento[berco][solucao.atendimentoBercos[berco].navios[j]]))) {
      solucao.atendimentoBercos[berco].navios[j+1] = solucao.atendimentoBercos[berco].navios[j];
      j--;
    }

    if (j != (i-1)) solucao.atendimentoBercos[berco].navios[j+1] = aux;
  }
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

void testeEstrategiasInsercao () {
  Solucao s1, s2, s3;
  int berco = 1, fo1, fo2, nav, flag;
  int n = 10;
  int insert[10];

  seed = clock();

  for (int k = 0; k < numeroBercos; k++) {
    s1.atendimentoBercos[k].tamanho = 0;
    s2.atendimentoBercos[k].tamanho = 0;
    s3.atendimentoBercos[k].tamanho = 0;
  }

  memset(&s1.atendimentoNavios, -1, sizeof(s1.atendimentoNavios));
  memset(&s2.atendimentoNavios, -1, sizeof(s2.atendimentoNavios));
  memset(&s3.atendimentoNavios, -1, sizeof(s3.atendimentoNavios));

  srand(seed);
  for (int i = 0; i < n; i++) {
    do {
      flag = 0;
      nav = rand() % numeroNavios;

      for (int j = 0; j < s1.atendimentoBercos[berco].tamanho; j++) {
        if (nav == s1.atendimentoBercos[berco].navios[j]) {
          flag = 1;
          break;
        }
      }
    } while (duracaoAtendimento[berco][nav] == 0 || flag);

    insert[i] = nav;

    inserirAtendimento(s1, berco, nav);
    inserirAtendimentoOrdemChegada(s2, berco, nav);
    fo1 = calcularFoBerco(s1, berco);
    fo2 = calcularFoBerco(s2, berco);

    cout << "I -> " << nav << " | ";

    for (int j = 0; j < s1.atendimentoBercos[berco].tamanho; j++) {
      cout << s1.atendimentoBercos[berco].navios[j] << " ";
    }

    cout << "| Aleatória: FO = " << fo1 << endl;

    cout << "I -> " << nav << " | ";
    for (int j = 0; j < s2.atendimentoBercos[berco].tamanho; j++) {
      cout << s2.atendimentoBercos[berco].navios[j] << " ";
    }

    cout << "| Ordem de chegada FO = " << fo2 << endl << endl;
  }

  for (int i = n - 1; i > 0; i--) {
    removerAtendimento(s1, insert[i]);
    removerAtendimentoOrdemChegada(s2, insert[i]);

    fo1 = calcularFoBerco(s1, berco);
    fo2 = calcularFoBerco(s2, berco);

    cout << "R -> " << insert[i] << " | ";

    for (int j = 0; j < s1.atendimentoBercos[berco].tamanho; j++) {
      cout << s1.atendimentoBercos[berco].navios[j] << " ";
    }

    cout << "| Aleatória: FO = " << fo1 << endl;

    cout << "R -> " << insert[i] << " | ";
    for (int j = 0; j < s2.atendimentoBercos[berco].tamanho; j++) {
      cout << s2.atendimentoBercos[berco].navios[j] << " ";
    }

    cout << "| Ordem de chegada FO = " << fo2 << endl << endl;
  }
}
