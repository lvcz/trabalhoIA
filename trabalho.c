#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "lista.h"
typedef struct aresta *aresta;
typedef struct grafo *grafo;
typedef struct vertice *vertice;

#define BLOCO_ATUAL 1
#define BLOCO_VIZINHO 2
struct grafo {

    lista		 vertices;
    unsigned int n_vertices;
    unsigned int n_arestas;
};

//------------------------------------------------------------------------------

struct vertice {
    lista  arestas;
    int    cor;
    int    posicao;
    int    visitado;
    int    flag;
    int    padding; // só pra evitar warning

};

//------------------------------------------------------------------------------

struct aresta {
    int	 valor;
    vertice pointer;
    int visitada;
};

//------------------------------------------------------------------------------
int n,m,k;
int *tabuleiro;
lista sequencia_trocas;
int *cores;
int *total_cores;
int *bloco_externo;
grafo g;
int print_cor = 1;


char* cor_ansi[] = { "\x1b[0m",
               "\x1b[31m", "\x1b[32m", "\x1b[33m",
               "\x1b[34m", "\x1b[35m", "\x1b[36m",
               "\x1b[37m", "\x1b[30;1m", "\x1b[31;1m",
               "\x1b[32;1m", "\x1b[33;1m", "\x1b[34;1m",
               "\x1b[35;1m", "\x1b[36;1m", "\x1b[37;1m" };
//Escopo
//------------------------------------------------------------------------------
void get_parametros(FILE *fp);
void get_tabuleiro(FILE *fp);
grafo set_grafo();
void printa_grafo(grafo g);
void resolverdor(grafo g);
void sentinela(vertice v);
void sentinela_vizinho(vertice vizinho);
void desvisita_vertices(lista l);
void printa_tabuleiro();
//------------------------------------------------------------------------------



//Funcoes para ler arquivo de entrada
//------------------------------------------------------------------------------
void get_parametros(FILE *fp){
	char *line;
    size_t len = 0;
	char *token;
	char *search = " ";
	int i = 0;

	getline(&line, &len, fp);
	token = strtok(line, search);

	while ( token != NULL){
		if (i == 0) n = atoi(token);
		if (i == 1) m = atoi(token);
		if (i == 2) k = atoi(token);
		++i;
		token = strtok(NULL, search);

	}

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void get_tabuleiro(FILE *fp){
    char *line;
    size_t len = 0;
    ssize_t read;
    int i = 0;
    char *token;
    char *search = " ";
    int posicao = 0;

    while ((read = getline(&line, &len, fp)) != -1){
        if (strcmp(line,"\n") || strcmp(line," \n")){
           
            token = strtok(line, search);
            

            while ( token != NULL){
                if (token)            {                    
                    int value = atoi(token);
                    if(value >0){
                        tabuleiro[posicao] = value;
                        ++posicao;
                    }
                    
                }
                token = strtok(NULL, search);
                ++i;
                

            }

        }
    }

    
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Retorna apontador para o vértice com o nome dado como argumento
// ou NULL caso o vértice for encontrado.
static vertice busca_vertice(lista l, int posicao) {

    if( !l )    return NULL;
    if( !posicao ) return NULL;

    for( no n = primeiro_no(l); n; n = proximo_no(n) ){
        vertice v = conteudo(n);
        if( v->posicao == posicao)
            return v;
    }

    return NULL;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//Funcao para printar o tabuleiro da lista
//------------------------------------------------------------------------------
void printa_tabuleiro(){    

    if(!print_cor){
        for (int i = 0; i <n ; ++i) {
            for (int j = 0; j < m; ++j) {
                printf("%d ",tabuleiro[ i*m + j]);
            }
            printf("\n");
        }
    }
    else{
        for (int i = 0; i <n ; ++i) {
            for (int j = 0; j < m; ++j) {
                printf("%s%d%s ", cor_ansi[tabuleiro[ i*m + j]], tabuleiro[ i*m + j],cor_ansi[0]);
            }
            printf("\n");
        }

    }
}
//------------------------------------------------------------------------------
//transforma o tabuleiro em grafo
//------------------------------------------------------------------------------
grafo set_grafo(){

    grafo g = malloc(sizeof(struct grafo));
    if( !g ) return NULL;

    g->vertices = constroi_lista();


    for (int i = 0; i < n ; ++i) {
        for (int j = 0; j <m ; ++j) {
            int pos = i * m + j;
            vertice vt = malloc(sizeof(struct vertice));
            vt->cor = tabuleiro[pos];
            vt->posicao = pos;
            vt->flag = 0;
            vt->arestas = constroi_lista();
            insere_lista(vt, g->vertices);

            if (j < m - 1)
            {
                aresta a   = malloc(sizeof(struct aresta));                
                a->valor = pos + 1;
                insere_lista(a, vt->arestas);
            }
            if (j > 0 && (pos - 1) != 0)
            {
                aresta a   = malloc(sizeof(struct aresta));                
                a->valor = pos - 1;
                insere_lista(a, vt->arestas);
            }
            if(i < n - 1 ){
                aresta a   = malloc(sizeof(struct aresta));
                a->valor = (i + 1) * m + j;
                insere_lista(a, vt->arestas);
            }
            if(i > 0 && ((i - 1) * m + j) != 0){
                aresta a   = malloc(sizeof(struct aresta));
                a->valor = (i - 1) * m + j;
                insere_lista(a, vt->arestas);
            }
        }
    }

    for( no n = primeiro_no(g->vertices); n; n = proximo_no(n) ){
        vertice v = conteudo(n);
        for( no m = primeiro_no( v->arestas ); m; m = proximo_no(m)){
            aresta a = conteudo(m);        
            a->pointer = busca_vertice(g->vertices, a->valor);            
        }        
    }
    return g;
}
//------------------------------------------------------------------------------
//mostra o grafo e seus vizinhos
//------------------------------------------------------------------------------
void printa_grafo(grafo g){

    for( no n = primeiro_no(g->vertices); n; n = proximo_no(n) ){
        vertice v = conteudo(n);
        printf(" %d , %d  vizinhos: \n", v->posicao, v->cor);
        for( no m = primeiro_no( v->arestas ); m; m = proximo_no(m)){
            aresta a = conteudo(m);
            //printf(" %d \n",a->valor);          
        }
       // printf("fim do vertice\n \n");
    }
}


//------------------------------------------------------------------------------
//Auto explicativo
void desvisita_vertices(lista l){
    
    for( no n= primeiro_no(l); n; n = proximo_no(n) ){
        vertice v = conteudo(n);
        v->visitado = 0;
    }
}

//------------------------------------------------------------------------------
void resolverdor(grafo g){
    if (!g) return;
    for (int i = 0; i < k; ++i)
    {
       cores[i]=0;
    }
    char * aux;
    int maior_vizinho,//maior bloco procurado
        nova_cor, //cor que sera trocada
        passos = 0, //contador de passos
        sem_vizinhos =0;
    no primeiro = primeiro_no(g->vertices);
    vertice v = conteudo(primeiro);
    v->flag = 1;
    while(1)
    { 
            
        
        sentinela(v);

        maior_vizinho = k-1;
        
        for (int i = 0; i < k; ++i)
        {
            if(cores[i] > cores[maior_vizinho]) maior_vizinho = i;
            printf("%d tem : %d nodos\n",i+1,cores[i] );
            
            if(cores[i] == 0) sem_vizinhos++; 
            
        }


        for (int i = 0; i < k && i !=maior_vizinho; ++i)
        {
            //procura por blocos de vizinhos que tenham tamanhos iguais e escolhe um (random)
            if(cores[maior_vizinho] == cores[i]){
                //TODO
            }
        }

        if (sem_vizinhos == k) break;
        
        
        nova_cor = maior_vizinho +1 ;
        printf("nova cor %d\n", nova_cor);


        for( no n = primeiro_no(g->vertices); n; n = proximo_no(n) ){
            vertice vt= conteudo(n);
            vt-> visitado = 0;
            //total_cores[vt->cor -1 ] ++;

            // se ja esta no bloco pinta com a nova cor
            if (vt -> flag == BLOCO_ATUAL){
                vt -> cor = nova_cor;
                tabuleiro[vt->posicao] = nova_cor;
            }
            //incorpora o vertice para o bloco Atual
            if( vt -> flag == BLOCO_VIZINHO && vt -> cor == nova_cor){
                vt -> flag = BLOCO_ATUAL;
            }

            //se o bloco foi visitado e nao pertence a nova cor volta a flag para 0
            if(vt-> flag == BLOCO_VIZINHO && vt -> cor != nova_cor){
                vt -> flag = 0;
            }

        }
         printa_tabuleiro();
        for (int i = 0; i < k; ++i)
        {
            
            cores[i] = 0;
            

            
            //  if(total_cores[i] == n*m){
            //     para =1;

            // }
            // else{
            //     total_cores[i]=0;
            // }
           
        }
        sem_vizinhos=0;
       

        //scanf("%s",aux);

        sleep(1);

        passos++;
    }
    

    printf("%d\n", passos );


    


}

void sentinela(vertice v){
    int cor_atual = v->cor;
    //printf("entra :%d , %d  \n", v->posicao, v->cor);
    v->visitado = 1;
    for( no n = primeiro_no(v->arestas); n; n = proximo_no(n) ){
        aresta t = conteudo(n);
        vertice vizinho = t->pointer;
        //printf("    vizinho : %d cor:%d flag: %d\n",vizinho->posicao, vizinho->cor,vizinho->flag );
        if(vizinho -> cor != cor_atual){
            //se for de cor diferente da atual solta uma sentinela procurando blocos vizinhos
            if (vizinho -> flag !=BLOCO_VIZINHO)
            {
                //procura por blocos no vizinho que tenha essa cor
                sentinela_vizinho(vizinho);
            }
            //ja foi visitado
            vizinho -> flag = BLOCO_VIZINHO;            
        }
        else{
            // se for da minha cor adiciono no meu bloco
            vizinho->flag = BLOCO_ATUAL;

            //se nao foi visitado  faz a recursao procurando vizinhos
            if(!vizinho -> visitado)
            {            
            sentinela(vizinho);
            }
        }

    }
    //printf("sai :%d , %d  \n", v->posicao, v->cor);
}


void sentinela_vizinho(vertice v){
    int cor_atual = v->cor;
    v-> flag = BLOCO_VIZINHO;
    cores[cor_atual -1] ++;   
    //printf("            sentinela_vizinho :%d , %d  \n", v->posicao, v->cor);
    for( no n = primeiro_no(v->arestas); n; n = proximo_no(n) ){
        aresta t = conteudo(n);
        vertice vizinho = t->pointer;
        if(vizinho -> cor == cor_atual && vizinho -> flag != BLOCO_VIZINHO){
            //caso meu vizinho seja da minha cor vejo se os vizinhos dele sao da minha cor tambem guardando flag BLOCO_VIZINHO 
            //indicado que ele sera pintado se eu trocar de cor        
            vizinho -> flag = BLOCO_VIZINHO;
            sentinela_vizinho(vizinho);
        }

    }


}


//------------------------------------------------------------------------------
//main
int main(int argc, char **argv){

    if(argc != 2 ){
        printf("usage: ./trabalho <arquivo.txt>\n");
        return -1;
    }
    srand(time(NULL));

    FILE *fp =  fopen( argv[1], "r" );

    get_parametros(fp);

    tabuleiro= malloc(sizeof(int) * n * m);
    cores = malloc (sizeof(int) * k);
    total_cores = malloc (sizeof(int) * k);
    
    

    if( tabuleiro == NULL) return -1;

    get_tabuleiro(fp);

    
    printa_tabuleiro();
    g = set_grafo();
    //printa_grafo(g);

    resolverdor(g);



}