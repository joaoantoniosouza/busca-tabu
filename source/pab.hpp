#ifndef PAB_HPP
#define PAB_HPP

#include <time.h>

#define MAX_NAVIOS 110
#define MAX_BERCOS 25

#define PENALIDADE_HORARIO_LIMITE_BERCO 10000
#define PENALIDADE_HORARIO_LIMITE_NAVIO 5000
#define PENALIDADE_NAVIO_NAO_ATENDIDO 50000
#define PENALIDADE_ATRACAMENTO_IMPOSSIVEL 100000
#define PENALIDADE_ATENDIMENTO_IMPOSSIVEL 100000
#define PENALIDADE_SOBREPOSICAO_HORARIO 100000

int numeroNavios, numeroBercos;

int tempoAtendimento[MAX_BERCOS][MAX_NAVIOS];
int momentoChegadaNavio[MAX_NAVIOS];
int momentoSaidaNavio[MAX_NAVIOS];

#define ABERTURA 0
#define FECHAMENTO 1
int aberturaFechamento[MAX_BERCOS][2];
int proximoHorarioDisponivelBerco[MAX_BERCOS];

typedef struct estruturaSolucao {
  int tempoTotalAtendimento;
  int atendimentoNavios[MAX_NAVIOS];
  int atendimentoBercos[MAX_BERCOS][MAX_NAVIOS];
  int momentoAtracamento[MAX_NAVIOS];
} Solucao;

typedef struct estruturaListaTabu {
  int quantidadeElementos;
  int tamanho;
  int *bercos;
  int *navios;
} ListaTabu;

void verificarParametrosEntrada (char *argv[]);
void lerInstancia (char *nomeInstancia);
void construtivaAleatoria(Solucao &solucao);
void calcularFO(Solucao &solucao);
void adicionarAtendimento (Solucao &solucao, int berco, int navio);
void removerAtendimento (Solucao &solucao, int berco, int navio);
void clonarSolucao(Solucao &original, Solucao &copia);
double calcularTempo (clock_t &clockInicial, clock_t &clockFinal);
void escreverSolucao(Solucao &solucao);
void executarTestes(int repeticoes);

void buscaTabu (Solucao &solucao, const int tamanhoLista, const double tempoMaximo, double &tempoTotal, double &momentoMelhorSolucao);
int procurarNaLista (ListaTabu &lista, const int navio, const int berco);
void removerDaLista (ListaTabu &lista, const int posicao);
void inserirNaLista (ListaTabu &lista, const int berco, const int navio);

#endif
