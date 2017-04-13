#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define iA 843314861
#define iB 453816693
#define iM 1073741824

#define ARQUIVO "100000"
#define QTDEMOCHILA 40000
#define QTDEESCOLHA 2000 /*USE NUMERO PAR para definir a quantidade de pais escolhidos para o cruzamento e.g, 4 pais = 2 cruzamentos*/
#define PMUTACAO 0.5 /*taxa de mutaçcao*/
#define NUMGERACOES 40000


typedef struct item {
    char label[10];
    int peso;
    int beneficio;
  }item;

typedef struct individuo{
    int *itens;/*Vetor de itens*/
    int PesoInd;/*Peso total do individuo*/
    int SomaBen; /*Soma do beneficio total do individuo*/
    int funcPen;/*Valor da função de Penalidade do Individuo*/
    float Aptidao;
    int Qtditens;
}INDIVIDUO;

int isem=123456789;

int K,Valor_Max,Alpha=0;/*Capacidade da mochila, e valor de itens na sala*/
int Escolhido[QTDEESCOLHA];/*vertor que contem o indice das mochilas escolhidas para cruzamento*/
INDIVIDUO mochila[QTDEMOCHILA];
int rank[QTDEMOCHILA];

INDIVIDUO Filhos[QTDEESCOLHA];
item* objetos;/*ponteiro que aponta para o inicio de uma struct*/
FILE *dados;


float rand0();
void Cruzamento();
int Le_Arquivo();
void IniciaPopulacao();
void FuncaoPenalidade(INDIVIDUO x[],int indice);
void FuncaoAptidao(INDIVIDUO x[]);
void FuncaoSelecao(INDIVIDUO x[],int* Escolha);
void Mutacao();


int main ()
{
   int i,j,geracao,auxPeso;
   /*int auxPesoF1,auxPesoF2;*/


    Le_Arquivo();
    IniciaPopulacao();
    FuncaoAptidao(mochila);


    for (geracao=0;geracao<NUMGERACOES;geracao++)
    {
       FuncaoSelecao(mochila,Escolhido);
       Cruzamento();
       Mutacao();

        for (i=0;i<QTDEESCOLHA;i++)
        {
            for (j=0;j<QTDEMOCHILA;j++)
            {
                if(Filhos[i].funcPen>mochila[rank[j]].funcPen)
                {
                    mochila[rank[j]]=Filhos[i];
                    break;
                }
                FuncaoAptidao(mochila);
            }

        }

    }


 /*for (i=0;i<QTDEMOCHILA;i++)
 {
     soma+=objetos[i].beneficio;
     printf("indice %d  valor=%d \n",rank[i],mochila[rank[i]].funcPen);
 }
printf("\n");*/



    /*for (i=0;i<QTDEMOCHILA;i++)
    {*/
        auxPeso=mochila[rank[QTDEMOCHILA-1]].PesoInd;
        j=0;
        while (auxPeso)
        {
            printf("%d ",mochila[rank[QTDEMOCHILA-1]].itens[j]);
            auxPeso-=objetos[mochila[rank[QTDEMOCHILA-1]].itens[j]].peso;
            /*printf("Peso da Mochila - %d Peso Objeto Tirado %d - Indice Objeto %d \n",auxPeso,objetos[mochila[i].itens[j]].peso,mochila[i].itens[j]);*/
            j++;
        }
        printf("Peso=%d  Beneficio=%d\n",mochila[rank[QTDEMOCHILA-1]].PesoInd,mochila[rank[QTDEMOCHILA-1]].SomaBen);
        printf("\n");



   /* for (i=0;(i<(QTDEESCOLHA/2));i++)
    {
        auxPesoF1=Filhos[i*2].PesoInd;
        auxPesoF2=Filhos[(i*2)+1].PesoInd;
        j=0;
        while (auxPesoF1)
        {
            printf("%d ",Filhos[i*2].itens[j]);
            auxPesoF1-=objetos[Filhos[i*2].itens[j]].peso;
            j++;
        }
        printf(" - Penalidade %d - Qtd Itens=%d",Filhos[i*2].funcPen,Filhos[i*2].Qtditens);
        j=0;
        printf("\n");
        while (auxPesoF2)
        {
            printf("%d ",Filhos[(i*2)+1].itens[j]);
            auxPesoF2-=objetos[Filhos[(i*2)+1].itens[j]].peso;
            j++;
        }
        printf(" - Penalidade %d - Qtd Itens=%d",Filhos[(i*2)+1].funcPen,Filhos[(i*2)+1].Qtditens);
    printf("\n");
    }*/


free(objetos);
fclose(dados);
return 0;
}

float rand0()
{
  float aux, x;
  aux = 0.5 / iM;
  isem = isem*iA + iB;
  if (isem < 0)
    isem = (isem + iM) + iM;

  x=isem*aux;
  return x;
}

int Le_Arquivo()/*FUNCIONA*/
{
    int i = 0;
    char file[50] = "KNAPDATA";
    strcat(file,ARQUIVO);
    strcat(file,".txt");
    FILE *arquivo;
    if ((arquivo = fopen(file, "r")) == NULL) {
        printf("Erro ao abrir o arquivo.\n");
    } else {
        fscanf(arquivo, "%d[^\n]", &K);
        fscanf(arquivo, "%d[^\n]", &Valor_Max);
        objetos=(item*)malloc(sizeof(item)*Valor_Max);/*Para alocar a struct objetos*/
        while (!feof(arquivo))
        {
            fscanf(arquivo, " %[^,]*c", objetos[i].label);
            fseek(arquivo, +1, SEEK_CUR);
            fscanf(arquivo, "%d[^,]", &objetos[i].peso);
            fseek(arquivo,+1,SEEK_CUR);
            fscanf(arquivo, "%d[^\n]", &objetos[i].beneficio);
            i++;
        }
        i--;
        fclose(arquivo);
    }
    for (i=0;i<Valor_Max;i++)
    {
        Alpha+=objetos[i].beneficio;

    }
    return i;
}

void IniciaPopulacao()/*FUNCIONA*/
{
    int QtdeItens,i,j,k;
    int status=1,indice=0;
    for (j=0;j<QTDEMOCHILA;j++)
    {
       mochila[j].Qtditens=0;
       mochila[j].PesoInd=0;
       mochila[j].SomaBen=0;
       QtdeItens=(int)(rand0()*Valor_Max);
       /*Tracaria em typedef struct individuos o primeiro elemento por int* itens*/
       mochila[j].itens=(int*)calloc(QtdeItens,sizeof(int)); /*Melhor usar o calloc pois inicia os elementos com zero, e malloc não*/
       for (i=0;i<QtdeItens;i++)
       {
           do {
                indice=(int)(rand0()*(Valor_Max-1));
                mochila[j].itens[i]=indice;
                status=1;
                for (k = 0; (k < i) && (status==1); ++k)
                    if (mochila[j].itens[i] == mochila[j].itens[k])
                        status = 0;
               }while (status==0);
                mochila[j].PesoInd += objetos[indice].peso;
                mochila[j].SomaBen += objetos[indice].beneficio;
                mochila[j].Qtditens++;
                if (mochila[j].PesoInd>(K-8))
                {
                   /*void *realloc (void *area_alocada, int novo_tamanho)*/
                    mochila[j].itens = (int *) realloc (mochila[j].itens, (i+1)*sizeof(int));
                    break;
                }
                /*printf("%d ", mochila[j].itens[i]);*/
       }
       FuncaoPenalidade(mochila,j);
       /*printf("%d \n", mochila[j].funcPen);*/
       /*printf("Peso %d Beneficio %d Funcao Penalidade %d\n  ",mochila[j].PesoInd,mochila[j].SomaBen,mochila[j].funcPen);*/
    }
}


void Cruzamento()/*FUNCIONA*/
{
    int auxPeso1=0,auxPeso2=0,i,j,k1=0,k2=0,QtdeCruzamentos,numAleat;
    QtdeCruzamentos=QTDEESCOLHA/2;


    for (i=0;i<QtdeCruzamentos;i++)/*A quantidade de cruzamentos é total de individuos escolhidos divididos por 2 */
    {
        auxPeso1=mochila[Escolhido[(i*2)]].PesoInd;
        auxPeso2=mochila[Escolhido[(i*2)+1]].PesoInd;
        Filhos[(i*2)].itens=(int*)calloc(1,sizeof(int));
        Filhos[(i*2)+1].itens=(int*)calloc(1,sizeof(int));
        Filhos[(i*2)].PesoInd=0;
        Filhos[(i*2)+1].PesoInd=0;
        Filhos[(i*2)].SomaBen=0;
        Filhos[(i*2)+1].SomaBen=0;

        numAleat=(int)(rand0()*(Valor_Max-1));
        j=0;
        k1=0;
        k2=0;
        while (auxPeso1)/*MELHORAR ESSE CODIGO COLOCAR UM FOR E O WHILE DENTRO, NÃO PRECISA DESSE indFilho*/
        {
            if (mochila[Escolhido[i*2]].itens[j]>numAleat)
            {
               Filhos[i*2].itens = (int *) realloc (Filhos[i*2].itens, (k1+1)*sizeof(int));
               Filhos[i*2].itens[k1]=mochila[Escolhido[i*2]].itens[j];
               Filhos[i*2].PesoInd+=objetos[mochila[Escolhido[i*2]].itens[j]].peso;
               Filhos[i*2].SomaBen+=objetos[mochila[Escolhido[i*2]].itens[j]].beneficio;
               k1++;
            }
            else
            {
               Filhos[(i*2)+1].itens = (int *) realloc (Filhos[(i*2)+1].itens, (k2+1)*sizeof(int));
               Filhos[(i*2)+1].itens[k2]=mochila[Escolhido[i*2]].itens[j];
               Filhos[(i*2)+1].PesoInd+=objetos[mochila[Escolhido[i*2]].itens[j]].peso;
               Filhos[(i*2)+1].SomaBen+=objetos[mochila[Escolhido[i*2]].itens[j]].beneficio;
               k2++;
            }
            auxPeso1-=objetos[mochila[Escolhido[i*2]].itens[j]].peso;
            j++;
        }
        j=0;
        while (auxPeso2)
        {
            if (mochila[Escolhido[(i*2)+1]].itens[j]>numAleat)
            {
               Filhos[(i*2)+1].itens = (int *) realloc (Filhos[(i*2)+1].itens, (k2+1)*sizeof(int));
               Filhos[(i*2)+1].itens[k2]=mochila[Escolhido[(i*2)+1]].itens[j];
               Filhos[(i*2)+1].PesoInd+=objetos[mochila[Escolhido[(i*2)+1]].itens[j]].peso;
               Filhos[(i*2)+1].SomaBen+=objetos[mochila[Escolhido[(i*2)+1]].itens[j]].beneficio;
               k2++;
            }
            else
            {
               Filhos[i*2].itens = (int *) realloc (Filhos[i*2].itens, (k1+1)*sizeof(int));
               Filhos[i*2].itens[k1]=mochila[Escolhido[(i*2)+1]].itens[j];
               Filhos[i*2].PesoInd+=objetos[mochila[Escolhido[(i*2)+1]].itens[j]].peso;
               Filhos[i*2].SomaBen+=objetos[mochila[Escolhido[(i*2)+1]].itens[j]].beneficio;
               k1++;
            }
            auxPeso2-=objetos[mochila[Escolhido[(i*2)+1]].itens[j]].peso;
            j++;
        }
        FuncaoPenalidade(Filhos,(i*2));
        FuncaoPenalidade(Filhos,((i*2)+1));
        Filhos[i*2].Qtditens=k1;
        Filhos[(i*2)+1].Qtditens=k2;

    }

}

void FuncaoPenalidade(INDIVIDUO x[],int indice)/*FUNCIONA*/
{
   int Max, Numero;
   Numero=x[indice].PesoInd-K;
   if (Numero>=0)
        Max=Numero;
    else
        Max=0;

   x[indice].funcPen=x[indice].SomaBen-(Alpha*Max);
   /*printf("%d\n",x[indice].funcPen);*/
}

void FuncaoAptidao(INDIVIDUO x[])/*FUNCIONA*/
{
    int i,j,nTroca,auxInd,auxIndp;/*auxInd é uma variavel para auxiliar encontrar os valorse*/
    float Max, Min;
    Max=1;
    Min=0;
    for (i=0;i<QTDEMOCHILA;i++)/*inicializando o vetor rank*/
    {
      rank[i]=i;
    }
    nTroca=QTDEMOCHILA-1;
    for(j=0; j<QTDEMOCHILA; j++)
    {
      for(i=0; i<nTroca; i++)
      {
        auxInd=rank[i];
        auxIndp=rank[i+1];
        if(x[auxInd].funcPen > x[auxIndp].funcPen)
        {
          /*nAux = auxInd;*/
          rank[i] = auxIndp;
          rank[i+1]=auxInd;
        }
      }
      nTroca--;
    }
    j=QTDEMOCHILA-1;
    auxInd=0;
    for (i=0;i<QTDEMOCHILA;i++)
    {
      auxInd=rank[j];
      x[auxInd].Aptidao=(Min+((Max-Min)*((float)(QTDEMOCHILA-(i+1))/(QTDEMOCHILA-1))));
      j--;
    }
}

void FuncaoSelecao(INDIVIDUO x[],int* Escolha)
{
    float SumAptidao[QTDEMOCHILA];
    int i,j;
    float NumAleat;
    int Max;


    SumAptidao[0]=x[0].Aptidao;
    for (i=1;i<QTDEMOCHILA;i++)
    {
        SumAptidao[i]=x[i].Aptidao+SumAptidao[i-1];
    }

    Max=(int)SumAptidao[QTDEMOCHILA-1];


    for (j=0;j<QTDEESCOLHA;j++)
    {
        NumAleat=(rand0()*Max);
        Escolha[j]=0;
        i=0;
        while (SumAptidao[i]<=NumAleat)
             {
                i++;
             }
         Escolha[j]=i;

    }


}

void Mutacao()
{
    int i,j,k,QtdeAlelosMutaveis,AleloMutavel,status=1,aux,NewIndice;

    for (i=0;i<QTDEESCOLHA;i++)
    {
         aux=(int)Filhos[i].Qtditens*PMUTACAO;
         QtdeAlelosMutaveis=(int)(rand0()*aux);
         for (j=0;j<QtdeAlelosMutaveis;j++)

         {
               AleloMutavel=(int)(rand0()*(Filhos[i].Qtditens-1));
               Filhos[i].PesoInd-=objetos[Filhos[i].itens[AleloMutavel]].peso;/*Tira o peso de uma vez*/
               Filhos[i].SomaBen-=objetos[Filhos[i].itens[AleloMutavel]].beneficio;
               do{
                   NewIndice=(int)(rand0()*(Valor_Max-1));
                   Filhos[i].itens[AleloMutavel]=NewIndice;            /*Mudou se o indice*/
                   status=1;
                   for (k = 0; (k < Filhos[i].Qtditens); k++)
                   {
                       if(k!=AleloMutavel)
                        {
                            if (Filhos[i].itens[AleloMutavel]==Filhos[i].itens[k])
                                status = 0;
                        }
                   }
               }while(status==0);
               Filhos[i].PesoInd+=objetos[NewIndice].peso;/*atualiza o peso*/
               Filhos[i].SomaBen+=objetos[NewIndice].beneficio;
               FuncaoPenalidade(Filhos,i);

         }
    }

}
