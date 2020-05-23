#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hash.h"

#define	 RAND(N) 		rand()%N
#define  DIAS	 		31   //qtd de dias do mes
#define  MES     		12   //qtd de meses no ano
#define  ANO     		99   //diff max p data de nascimento (1920~ 2019) diff de 99

Cliente escreveStructAleatoria(int id);

char getNomesAleatorios[49][20] = {
"Aline",
"Alberto",
"Anaa",
"Beatriz",
"Bruno",
"Bianca",
"Cesar",
"Caroline",
"Caio",
"Damares",
"Diego",
"Everton",
"Elias",
"Eloah",
"Felipe",
"Fabiana",
"Gustavo",
"Gabriele",
"Hudson",
"Helena",
"Italo",
"Isabela",
"Juliana",
"Julio",
"Karen",
"Kaique",
"Lorena",
"Lucas",
"Maria",
"Marcos",
"Nicolas",
"Naiara",
"Osana",
"oscar",
"Paula",
"Pablo",
"Quezia",
"Raiane",
"Rodrigo",
"Sabrina",
"Silvio",
"Tabita",
"Teoo",
"Uallace",
"Vanessa",
"Wanda",
"Xaulim",
"Yohan",
"Zatle"
};

int numNomes = 49;

Cliente escreveStructAleatoria(int id)
{
Cliente cliente;

	cliente.id    = id;
	cliente.chave = RAND(10000);
	sprintf(cliente.nome,"%s",getNomesAleatorios[RAND(numNomes)]);
	sprintf(cliente.dataNasc,"%d/%d/%d",RAND(DIAS+1),RAND(MES+1),1920+(RAND(ANO+1))); 
	
	return cliente;
}

void inicializaArqs(FILE *f, int m){
int i = 1;
Cliente cliente = newCliente();
	
	rewind(f);
	
	while(i <= m){
		fseek(f,(i-1)*sizeof(Cliente),SEEK_SET);
		fwrite(&cliente,sizeof(Cliente),1,f);
		i++;
	}
	i =1;
	while(i <= m){
		fseek(f,(i-1)*sizeof(Cliente),SEEK_SET);
		fread(&cliente,sizeof(Cliente),1,f);
		i++;
	}
	
}

int getAleatorioSeq(int ordem,int fator_de_carga)
{
	return RAND(CAST_FC(ordem,fator_de_carga));
}

FILE* geraSeqInsRm(FILE *f,int fator_de_carga,int ordem, const char *nome)
{
FILE *seq;
Cliente cliente,cliente2,cliente_aux[10];
int c,i,j,repetidas ;

	seq = fopen(nome,"wb");
	c 	= -1;
	i = j = 0;
	repetidas  = 1;
	
	while(++c < 20){
		
		fseek(seq,0,SEEK_END);

		if(c%2 == 0){
			cliente = escreveStructAleatoria( CAST_FC(ordem,fator_de_carga) + (c/2) + 1);
			fwrite(&cliente,sizeof(Cliente),1,seq);
			printf("cliente 1 - %s	%d\n",cliente.nome,cliente.chave);
		}
		else{
		
			fseek(f,getAleatorioSeq(ordem,fator_de_carga)*sizeof(Cliente),SEEK_SET);
			fread(&cliente2,sizeof(Cliente),1,f);
			
			while(!cliente2.id || repetidas){
				
				while(!cliente2.id){
						fseek(f,getAleatorioSeq(ordem,fator_de_carga)*sizeof(Cliente),SEEK_SET);
						fread(&cliente2,sizeof(Cliente),1,f);
				}
							
				for(i = 0; i < j ; i++){
					if(cliente2.id == cliente_aux[i].id) cliente2.id = 0;
				}	
				
				if(!cliente2.id) continue;
				else {
					cliente_aux[j++] = cliente2;
					repetidas = 0;
				}

			}
			
			printf("cliente 2 - %s	%d %d\n",cliente2.nome,cliente2.chave,cliente2.id);
			fwrite(&cliente2,sizeof(Cliente),1,seq);
			repetidas = 1;
		}
		
		printf("c >>> %d\n",c);
	}
	
	fclose(seq);
	seq = fopen(nome,"r+b");
	return seq;
}

void geraAleatorio(int fator_de_carga)
{
int     i;
Cliente cliente1,cliente2,cliente3;
FILE    *ordem32, *ordem256, *ordem1024;
	
	i = 1;
	ordem32   = fopen("ordem32.dat","wb");
	ordem256  = fopen("ordem256.dat","wb");
	ordem1024 = fopen("ordem1024.dat","wb");
	
	inicializaArqs(ordem32,32);
	inicializaArqs(ordem256,256);
	inicializaArqs(ordem1024,1024);
	
	rewind(ordem32);
	rewind(ordem256);
	rewind(ordem1024);
	
	
	while(i <= CAST_FC(1024,fator_de_carga)){
		
		fseek(ordem32,(i-1)*sizeof(Cliente),SEEK_SET);
		fseek(ordem256,(i-1)*sizeof(Cliente),SEEK_SET);
		fseek(ordem1024,(i-1)*sizeof(Cliente),SEEK_SET);
	
		if(i <= CAST_FC(32,fator_de_carga))  cliente1 = escreveStructAleatoria(i);
		if(i <= CAST_FC(256,fator_de_carga)) cliente2 = escreveStructAleatoria(i);
											 cliente3 = escreveStructAleatoria(i);
											 
		if(i <= CAST_FC(256,fator_de_carga)){
			fwrite(&cliente2,sizeof(Cliente),1,ordem256);
			if(i <= CAST_FC(32,fator_de_carga)) fwrite(&cliente1,sizeof(Cliente),1,ordem32);
		}
		fwrite(&cliente3,sizeof(Cliente),1,ordem1024);
		
		i++;
	}
	
	
	fclose(ordem32);
	fclose(ordem256);
	fclose(ordem1024);
}
