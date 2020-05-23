#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "hash.h"

Cliente newCliente(void)
{
Cliente cliente;
	
	cliente.id    = -1;
	cliente.chave = -1;
	sprintf(cliente.nome,"xxxx");
	sprintf(cliente.dataNasc,"00/00/0000");
	
	return cliente;
}

void imprime(FILE *f)
{
Cliente cliente;
int c = 0;
  while(c < 1024*40){
  	
  		fseek(f,c,SEEK_SET);
  		fread(&cliente,sizeof(Cliente),1,f);
  		fprintf(stderr,"Imprime : %d %d %s %s\n",cliente.id,cliente.chave,cliente.nome,cliente.dataNasc);
  		c+=sizeof(Cliente);
  }
  printf("\n");
}

FILE* copyFile(FILE *arquivo, const char *nome)
{
FILE *f = fopen(nome,"wb");
Cliente cliente;
long int a;
long int c;
	
	fclose(f);
	f = fopen(nome,"r+b");
	c = 0;
	fseek(arquivo, 0, SEEK_END);
	a = ftell(arquivo);
	rewind(f);
	while(c < a){
		
		fseek(arquivo,c,SEEK_SET);
		fseek(f,0,SEEK_END);
		fread(&cliente,sizeof(Cliente),1,arquivo);
		fwrite(&cliente,sizeof(Cliente),1,f);
		c += sizeof(Cliente);
	}

	rewind(arquivo);
	rewind(f);
	return f;
}

void escreveCliente(FILE *f,Cliente *cliente,int salto)
{
	fseek(f,salto*sizeof(Cliente),SEEK_SET);
	fwrite(cliente,sizeof(Cliente),1,f);
}

Cliente lerCliente(FILE *f,int salto)
{
Cliente cliente;

	fseek(f,salto*sizeof(Cliente),SEEK_SET);
	fread(&cliente,sizeof(Cliente),1,f);
	
	return cliente;
}

float fatorCarga(int n, int m)
{
	return (n*1.0)/(m*1.0);
}

int funcHashDispDupla2(int x,int m)
{
	return x%m;	
}

int funcHashDispDupla1(int x,int m)
{
	return 1+(x%m);
}

int funcHashQuad(int x, int k, int m)
{
	if(k == 0) return (x%m);
	return (funcHashQuad(x,k-1,m) + k)%m;
}

int funcHashLinear(int x, int k, int m)
{
	return ((x%m) + k)%m;
}

int funcHashDispDupla(int x, int k, int m)
{
	return (funcHashQuad(x,k,m) + (k*funcHashLinear(x,k,m))) % m;
}

int funcHash(int x, int k,int m, int op)
{
	switch(op){
		case 1: return funcHashLinear(x,k,m);
		case 2: return funcHashQuad(x,k,m);
		case 3: return funcHashDispDupla(x,k,m);
	}
}

int	remover(FILE *arquivo, Cliente cliente, int m,int metodo)
{

Cliente aux;
int 	jump,tempo_de_busca,end,k;

	tempo_de_busca = end = k = 0;
	while(1){

		if(end) return tempo_de_busca;
		
		if(k == m){
			fprintf(stderr,"O elemento a ser removido não existe %s %d -- %d\n",cliente.nome,cliente.chave,metodo);
			return -1;
		}
		
		jump = funcHash(cliente.chave,k++,m,metodo);
		aux  = lerCliente(arquivo,jump);
		
		if(aux.id == cliente.id){
			cliente = newCliente();
			fseek(arquivo,jump*sizeof(Cliente),SEEK_SET);
			escreveCliente(arquivo,&cliente,jump);
			fprintf(stderr,"Removido com sucesso!\n");
			end =  1;
		} 
		else tempo_de_busca++;		
	
	}
	fprintf(stderr,"Erro inesperado\n");
	return -2;
}

int	inserir(FILE *f, Cliente cliente, int m, trataColisao t_colisao)
{
	return t_colisao(f,cliente,m);
}

int tentativaLinear(FILE *arquivo, Cliente cliente,int m)
{
Cliente aux;
int 	jump,colisao,k;

	colisao = k = 0;
	
	while(1){
		
		if(k == m){
			fprintf(stderr,"Overflow - linear\n");
			return -1;
		}
		
		jump =	funcHash(cliente.chave,k++,m,1);
		aux = lerCliente(arquivo,jump);

		if(aux.id == -1){
			escreveCliente(arquivo,&cliente,jump);
			fprintf(stderr,"Inserido com sucesso - linear!\n");
			return colisao;
		}
		else colisao++;		
	
	}
	fprintf(stderr,"Erro inesperado - \n");
	return -2;
}

int tentativaQuad(FILE *arquivo, Cliente cliente, int m)
{
Cliente aux;
int 	jump,colisao,k;

	colisao = 0;
	k		= 1;
	
	while(1){
		
		if(k == m){
			fprintf(stderr,"Overflow -- quad\n");
			return -1;
		}
		
		jump = funcHash(cliente.chave,k++,m,2);
		aux  = lerCliente(arquivo,jump);

		if(aux.id == -1){
			escreveCliente(arquivo,&cliente,jump);
			fprintf(stderr,"Inserido com sucesso -- quad!\n");
			return colisao;
		}
		else colisao++;		
	}
	fprintf(stderr,"Erro inesperado --\n");
	return -2;
}

int tentativaDispDupla(FILE *arquivo, Cliente cliente, int m)
{
Cliente aux;
int 	jump,colisao,k;

	colisao = k = 0;
	
	while(1){
		
		if(k == m){
			fprintf(stderr,"Overflow --- DDD\n");
			return -1;
		}
		
		jump =	funcHash(cliente.chave,k++,m,3);
		aux = lerCliente(arquivo,jump);

		if(aux.id == -1){
			escreveCliente(arquivo,&cliente,jump);
			fprintf(stderr,"Inserido com sucesso! --- DDDD\n");
			return colisao;
		}
		else colisao++;		
	
	}
	fprintf(stderr,"Erro inesperado ---\n");
	return -2;
}

int sequenciaAcoes(FILE *f,int inser,Cliente cliente,int m,trataColisao t_colisao,int metodo)
{
	if(inser) return inserir(f,cliente,m,t_colisao);
	else	  return remover(f,cliente,m,metodo);
}

int main(void){
FILE 		 *o32,*o256,*o1024,*tabela_hash,*seq1,*seq2,*seq3,*t1,*t2,*t3;
char 		 *nome_metodo;
int  		 controle,metodo,tamanho,m,colisao1,colisao2,colisao3,soma1,soma2,soma3,fator_de_carga,busca1,busca2,busca3;
Cliente 	 cliente1,cliente2,cliente3;
trataColisao t_linear,t_quad,t_disp;
	 
	colisao1 = colisao2 = colisao3 = soma1 = soma2 = soma3 = 0;
	metodo   = tamanho  = controle = -1;
	t_linear = tentativaLinear;
	t_quad   = tentativaQuad;
	t_disp   = tentativaDispDupla;
	fator_de_carga = 6;
	
	tabela_hash = fopen("tabela_hash","w");
			 t1 = fopen("t1","w");
			 t2 = fopen("t2","w");
		     t3 = fopen("t3","w");
		     
	srand((unsigned) time(NULL));
	
	
	while(++fator_de_carga < 10){
	
		geraAleatorio(fator_de_carga);
		o32		= copyFile(fopen("ordem32.dat","r+b"),"ordem32cp.dat");
		o256	= copyFile(fopen("ordem256.dat","r+b"),"ordem256cp.dat");
		o1024	= copyFile(fopen("ordem1024.dat","r+b"),"ordem1024cp.dat");
		seq1    = geraSeqInsRm(o32,  fator_de_carga,  32, "seq1.dat");
		seq2	= geraSeqInsRm(o256, fator_de_carga, 256, "seq2.dat");
		seq3    = geraSeqInsRm(o1024,fator_de_carga,1024, "seq3.dat");
		
		while(++metodo < 3){
			
			while(++controle < 20){
				
				fseek(seq1,controle*sizeof(Cliente),SEEK_SET);
				fseek(seq2,controle*sizeof(Cliente),SEEK_SET);
				fseek(seq3,controle*sizeof(Cliente),SEEK_SET);
			
				fread(&cliente1,sizeof(Cliente),1,seq1);
				fread(&cliente2,sizeof(Cliente),1,seq2);
				fread(&cliente3,sizeof(Cliente),1,seq3);
					
				if(controle % 2 == 0){
					
					colisao1 = sequenciaAcoes(o32,  1,cliente1,  32, metodo == 0 ? t_linear: metodo == 1 ? t_quad:t_disp,0);	
					colisao2 = sequenciaAcoes(o256, 1,cliente2, 256, metodo == 0 ? t_linear: metodo == 1 ? t_quad:t_disp,0);
					colisao3 = sequenciaAcoes(o1024,1,cliente3,1024, metodo == 0 ? t_linear: metodo == 1 ? t_quad:t_disp,0);
					nome_metodo = strdup(metodo == 0 ? "Tentativa Linear":metodo == 1? "Tentativa Quadratica":"Tentantiva Disp Dupla");
					fprintf(t1,"Insersao : Fator de Carga : %.1f | Metodo : %s | Ordem : 32   | Chave : %d |Colisoes : %d\n",(float) fator_de_carga/10,nome_metodo,cliente1.chave,colisao1);
					fprintf(t2,"Insersao : Fator de Carga : %.1f | Metodo : %s | Ordem : 256  | Chave : %d |Colisoes : %d\n",(float) fator_de_carga/10,nome_metodo,cliente2.chave,colisao2);
					fprintf(t3,"Insersao : Fator de Carga : %.1f | Metodo : %s | Ordem : 1024 | Chave : %d |Colisoes : %d\n",(float) fator_de_carga/10,nome_metodo,cliente3.chave,colisao3);
					free(nome_metodo);
				}	
				else{
					nome_metodo = strdup(metodo == 0 ? "Tentativa Linear":metodo == 1? "Tentativa Quadratica":"Tentantiva Disp Dupla");
					busca1 = sequenciaAcoes(o32,  0,cliente1,32,  NULL,metodo+1);
					busca2 = sequenciaAcoes(o256, 0,cliente2,256, NULL,metodo+1);
					busca3 = sequenciaAcoes(o1024,0,cliente3,1024,NULL,metodo+1);
					fprintf(t1,"Busca : Fator de Carga : %.1f | Metodo : %s | Ordem : 32   | Chave : %d |Tempo : %d\n",(float) fator_de_carga/10,nome_metodo,cliente1.chave,busca1);
					fprintf(t2,"Busca : Fator de Carga : %.1f | Metodo : %s | Ordem : 256  | Chave : %d |Tempo : %d\n",(float) fator_de_carga/10,nome_metodo,cliente2.chave,busca2);
					fprintf(t3,"Busca : Fator de Carga : %.1f | Metodo : %s | Ordem : 1024 | Chave : %d |Tempo : %d\n",(float) fator_de_carga/10,nome_metodo,cliente3.chave,busca3);
					free(nome_metodo);
				}
				
			}

			fclose(o32);
			fclose(o256);
			fclose(o1024);
			o32		= copyFile(fopen("ordem32.dat","r+b"),"ordem32cp.dat");
			o256	= copyFile(fopen("ordem256.dat","r+b"),"ordem256cp.dat");
			o1024	= copyFile(fopen("ordem1024.dat","r+b"),"ordem1024cp.dat");

			controle = -1;	
		}
		metodo = -1;
	}
	fclose(tabela_hash);
	
	return 0;
}
