#include <stdio.h>
#include <stdlib.h>

#define  CAST_FC(N,FC)  (((int) (N*(((float)FC)/10))) + 1)

typedef struct cliente{
	int  id;
	int	 chave;
	char nome[21];
	char dataNasc[10];
}Cliente;

typedef int (*trataColisao)(FILE*,Cliente,int);

Cliente newCliente(void);
Cliente lerCliente(FILE *f,int salto);
float 	fatorCarga(int n, int m);
int	  	inserir(FILE *f, Cliente cliente, trataColisao t_colisao);
int	  	remover(FILE *f, Cliente cliente,int m,int metodo);
int   	funcHash(int x,int m);
int  	tentativaLinear(FILE *arquivo, Cliente cliente);
int  	tentativaQuad(FILE *arquivo, Cliente cliente);
int  	tentativaDispDupla(FILE *arquivo, Cliente cliente);
FILE* 	geraSeqInsRm(FILE *f,int fator_de_carga,int ordem, const char *nome);
void 	geraAleatorio(int fator_de_carga);
Cliente escreveStructAleatoria(int id);


