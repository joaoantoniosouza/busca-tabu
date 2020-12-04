#ifndef PAB_HPP
#define PAB_HPP

#define MAX_NAVIOS 110
#define MAX_BERCOS 25

#define PENALIDADE_HORARIO_LIMITE_BERCO 1000
#define PENALIDADE_HORARIO_LIMITE_NAVIO 1000
#define PENALIDADE_NAVIO_NAO_ATENDIDO 10000
#define PENALIDADE_ATRACAMENTO_IMPOSSIVEL 100000

int numeroNavios, numeroBercos;

int tempoAtendimento[MAX_BERCOS][MAX_NAVIOS];
int tempoChegadaNavio[MAX_NAVIOS];
int tempoSaidaNavio[MAX_NAVIOS];

#define ABERTURA 0
#define FECHAMENTO 1
int aberturaFechamento[MAX_BERCOS][2];
int proximoHorarioDisponivelBerco[MAX_BERCOS];

typedef struct estruturaSolucao {
  int tempoTotal;
  int atendimento[MAX_NAVIOS];
  int tempoAtracamento[MAX_NAVIOS];
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
void clonarSolucao(Solucao &original, Solucao &copia);
double calcularTempo (clock_t &clockInicial, clock_t &clockFinal);
void escreverSolucao(Solucao &solucao);
void executarTestes(int repeticoes);

void buscaTabu (Solucao &solucao, const int tamanhoLista, const double tempoMaximo, double &tempoTotal, double &momentoMelhorSolucao);
int procurarNaLista (ListaTabu &lista, const int navio, const int berco);
void removerDaLista (ListaTabu &lista, const int posicao);
void inserirNaLista (ListaTabu &lista, const int berco, const int navio);

#endif
