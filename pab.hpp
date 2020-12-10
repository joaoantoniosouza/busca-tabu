#ifndef PAB_HPP
#define PAB_HPP

#define MAX_NAVIOS 110
#define MAX_BERCOS 25

#define PENALIDADE_HORARIO_LIMITE_BERCO 1000
#define PENALIDADE_HORARIO_LIMITE_NAVIO 1000
#define PENALIDADE_NAVIO_NAO_ATENDIDO 10000
#define PENALIDADE_ATRACAMENTO_IMPOSSIVEL 100000

int numeroNavios, numeroBercos;

int duracaoAtendimento[MAX_BERCOS][MAX_NAVIOS];
int momentoChegadaNavio[MAX_NAVIOS];
int momentoSaidaNavio[MAX_NAVIOS];

#define ABERTURA 0
#define FECHAMENTO 1
int aberturaFechamento[MAX_BERCOS][2];
int proximoHorarioDisponivelBerco[MAX_BERCOS];

typedef struct _atendimentoBerco {
  int tamanho;
  int navios[MAX_NAVIOS];
} AtendimentoBerco;

typedef struct _solucao {
  int tempoTotal;
  int atendimentoNavios[MAX_NAVIOS];
  AtendimentoBerco atendimentoBercos[MAX_BERCOS];
} Solucao;

void lerInstancia ();
void inserirAtendimento (Solucao &solucao, int berco, int navio);
void removerAtendimento (Solucao &solucao, int berco, int navio);
void construtivaAleatoria (Solucao &solucao);
void calcularFO (Solucao &solucao);
void escreverSolucao (Solucao &solucao);
void clonarSolucao (Solucao &original, Solucao &copia);
void executarTestes (int repeticoes);

#endif
