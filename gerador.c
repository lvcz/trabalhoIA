#include <stdlib.h>
#include <stdio.h>
int n,m,k,semente;



void gera_mapa(int semente) {
  int i, j;

  if(semente < 0)
    srand(time(NULL));  
  else
    srand(semente);  
  
  for(i = 0; i < n; i++) {
    for(j = 0; j < m; j++){
      printf("%d ", 1 + rand() % k);
    }
     printf("\n");
  }
 
}


int main(int argc, char **argv){
	
	if(argc < 4 || argc > 5) {
    printf("uso: %s <numero_de_linhas> <numero_de_colunas> <numero_de_cores> [<semente_aleatoria>]\n", argv[0]);
    exit(1);
  }

  n = atoi(argv[1]);
  m = atoi(argv[2]);
  k = atoi(argv[3]);
  printf("%d %d %d\n",n,m,k);

  

  if(argc == 5)
    semente = atoi(argv[4]);
  else
    semente = -1;
  gera_mapa(semente);

}