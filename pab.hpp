#ifndef PAB_HPP
#define PAB_HPP

#define MAX_NAVIOS 110
#define MAX_BERCOS 25

#define PENALIDADE_HORARIO_LIMITE_BERCO 2000
#define PENALIDADE_HORARIO_LIMITE_NAVIO 2000
#define PENALIDADE_NAVIO_NAO_ATENDIDO 2000

int seed;
int numeroNavios, numeroBercos;

int duracaoAtendimento[MAX_BERCOS][MAX_NAVIOS];
int momentoChegadaNavio[MAX_NAVIOS];
int momentoSaidaNavio[MAX_NAVIOS];

#define ABERTURA 0
#define FECHAMENTO 1
int aberturaFechamento[MAX_BERCOS][2];

typedef struct _atendimentoBerco {
  int tamanho;
  int navios[MAX_NAVIOS];
} AtendimentoBerco;

typedef struct _solucao {
  int tempoAtendimentoTotal;
  int atendimentoNavios[MAX_NAVIOS];
  AtendimentoBerco atendimentoBercos[MAX_BERCOS];
} Solucao;

void inserirAtendimento (Solucao &solucao, int berco, int navio);
void removerAtendimento (Solucao &solucao, int navioARemover);

void removerAtendimentoOrdemChegada (Solucao &solucao, int navioARemover);
void inserirAtendimentoOrdemChegada (Solucao &solucao, int berco, int navio);

void heuristicaConstrutiva (Solucao &solucao);

void calcularFO (Solucao &solucao);
int calcularFoBerco (Solucao &solucao, int berco);

void buscaTabu (Solucao &solucao, const int tamanhoLista, const double tempoMaximo, double &tempoTotal, double &momentoMelhorSolucao, int &solucaoInicial);
void removerDaLista (int **lista, int &quantidadeElementos, const int posicao);
void inserirNaLista (int **lista, int &quantidadeElementos, int tamanho, const int navio, const int berco);
int procurarNaLista (int **lista, int quantidadeElementos, const int navio, const int berco);
double calcularTempo (clock_t &clockInicial, clock_t &clockAtual);

void escreverCabecalhoLog (char *instancia);
void atualizarExecucaoLog (Solucao &solucao, int seed, double tempoTotal, double tempoMelhor, int solucaoInicial);
void escreverMediasLog (int numeroExecucoes, int melhorFo, int somaFO, double tempoSoma, double melhorTempoSoma);

void lerInstancia (char* instancia);
void escreverSolucao (Solucao &solucao);
void clonarSolucao (Solucao &original, Solucao &copia);

void testeEstrategiasInsercao ();

#endif
