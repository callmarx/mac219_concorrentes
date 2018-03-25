/**********************************************************/
/* MAC0219 - Computação Paralela e Distribuída - 2018/01  */
/* Eugenio Augusto Jimenes                                */
/* Nº USP 7118981                                         */
/* MiniEP1 - Utilização das Memórias Caches               */
/**********************************************************/

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

float timedifference_msec(struct timeval t0, struct timeval t1){
  return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

int main(int argc, char *argv[]){
  struct timeval t0;
  struct timeval t1;
  float elapsed;
  int i, j, size;
  int **matrix;

  /* primeiro argumento é o tamanho da matriz quadrada*/
  size = atoi(argv[1]);

  /* Aloca a matriz size X size */
  matrix = (int **)malloc(size*sizeof(int*));
  for(i = 0; i < size; i++)
    matrix[i] = (int *)malloc(size*sizeof(int));

  /* #### Inicio da contagem de execução #### */
  gettimeofday(&t0, 0);
  /* Opção 1: percorre por linhas */
  if(atoi(argv[2]) == 1) {
    for(i = 0; i < size ; i++){
      for (j = 0 ; j < size; j++) {
        matrix[i][j] = 0;
      }
    }
  }
  /* Opção 2: percorre por colunas */
  else{
    for(j = 0; j < size; j++) {
      for(i = 0; i < size; i++) {
        matrix[i][j] = 0;
      }
    }
  }
  /* #### fim da contagem de execução #### */
  gettimeofday(&t1, 0);

  elapsed = timedifference_msec(t0, t1);

  printf("Matriz zerada em %f milisegundos.\n", elapsed);

  for(i= 0; i < size; i++)
    free(matrix[i]) ;
  free(matrix);

  return 0;
}
