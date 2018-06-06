/* ################################################################################################ */
/* MAC0219 - Computação Paralela e Distribuída - 2018/01                                            */
/* Eugenio Augusto Jimenes                                                                          */
/* Nº USP 7118981                                                                                   */
/* EP1 - Pthreads e OpenMP                                                                          */
/* ################################################################################################ */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#include <omp.h>
/* para facilitar a leitrua e não repitir código é utilizado o "superint" */
typedef unsigned long long superint;

/*****************************************************************************/
/* Funções e estruturas utlizadas para multiplicação com Pthreads            */
/*****************************************************************************/

/* fixado uso em 4 threads */
#define NUM_THREADS 4

/* estrutura de dados para as matrizes */
typedef struct matrixthread {
 int threadId;
 double **matrixA;
 double **matrixB;
 double **matrixC;
 superint rowsA;
 superint colsA_rowsB;
 superint colsB;
} MatrixThread;

/* função de execução para as threads */
void *subMultiplication(void *arg) {
  MatrixThread *current;
  current = (MatrixThread *) arg;
  int threadId = current->threadId;
  double **matrixA = current->matrixA;
  double **matrixB = current->matrixB;
  double **matrixC = current->matrixC;
  superint rowsA = current->rowsA;
  superint colsA_rowsB = current->colsA_rowsB;
  superint colsB = current->colsB;

  superint i, j, k, partition, row_start, row_end;
  double sum;
  superint size = colsA_rowsB;

  partition = rowsA / NUM_THREADS;
  row_start = threadId * partition;
  row_end = (threadId+1) * partition;

  if(threadId == NUM_THREADS - 1 && row_end < rowsA) {
    row_end = rowsA;
  }

  for (i = row_start; i < row_end; i++) {
    for (j = 0; j < colsB; j++) {
      sum = 0;
      for (k = 0; k < size; k++) {
        sum += matrixA[i][k] * matrixB[k][j];
      }
      matrixC[i][j] = sum;
    }
  }
  return NULL;
}

/* Função principal para a execução com Pthreads */
void pthreadMultiplication(double **matrixA, double **matrixB, double **matrixC, superint rowsA, superint colsA_rowsB, superint colsB) {
  int i;
  MatrixThread thread_array[NUM_THREADS];
  pthread_t *threads;

  threads = (pthread_t *) malloc(NUM_THREADS * sizeof(pthread_t));
  for (i = 0; i < NUM_THREADS; i++) {
    thread_array[i].threadId = i;
    thread_array[i].matrixA = matrixA;
    thread_array[i].matrixB = matrixB;
    thread_array[i].matrixC = matrixC;
    thread_array[i].rowsA = rowsA;
    thread_array[i].colsA_rowsB = colsA_rowsB;
    thread_array[i].colsB = colsB;
    pthread_create( &threads[i], NULL, subMultiplication, (void *)&thread_array[i]);
  }
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join( threads[i], NULL );
  }
  return;
}

/*****************************************************************************/
/* Função de loop simples para execução sequencial da multiplicação          */
/*****************************************************************************/
void foolMultiplication(double **matrixA, double **matrixB, double **matrixC, superint rowsA, superint colsA_rowsB, superint colsB) {
  superint i, j, k;
  double sum = 0;

  for(i = 0; i < rowsA; i++) {
    for(j = 0; j < colsB; j++) {
      for(k = 0; k < colsA_rowsB; k++) {
        sum += matrixA[i][k] * matrixB[k][j];
      }
      matrixC[i][j] = sum;
      sum = 0;
    }
  }
  return;
}

/*****************************************************************************/
/* Função de loop simples para execução com OpenMP da multiplicação          */
/*****************************************************************************/
void openmpMultiplication(double **matrixA, double **matrixB, double **matrixC, superint rowsA, superint colsA_rowsB, superint colsB) {
  #pragma omp parallel
  {
    superint i, j, k;
    double sum = 0;

    #pragma omp for
    for(i=0; i < rowsA; i++){
      for(j=0; j< colsB ; j++){
        for(k=0; k< colsA_rowsB; k++){
          sum += matrixA[i][k] * matrixB[k][j];
        }
        matrixC[i][j] = sum;
        sum = 0;
      }
    }
  }
  return;
}

/*****************************************************************************/
/* Funções auxiliares (leitura, escrita e alocação)                          */
/*****************************************************************************/

/* Função que imprime a matriz em um arquivo nos moldes do EP */
void print_matrix(double **matrix, int n_rows, int n_cols, char *path_name) {
  FILE *file;
  file = fopen(path_name, "wr");

  if (file == NULL) {
      printf ("Erro de abertura/escrita de arquivo!\n'");
      exit(EXIT_FAILURE);
  }
  fprintf(file, "%d %d\n", n_rows, n_cols);
  for(int i=0; i<n_rows; i++) {
    for(int j=0; j<n_cols; j++) {
        fprintf(file, "%d %d %f\n", i+1, j+1, matrix[i][j]);
      }
  }
  fclose(file);
}

/* Função que devolve uma matriz alocada e zerada. */
double **matrix_malloc(superint n_rows, superint n_cols) {
  superint i, j;
  double **matrix = malloc(n_rows*sizeof(double*));
  for(i = 0; i<n_rows; i++) {
    matrix[i] = malloc(n_cols*sizeof(double*));
  }
  for(i = 0; i < n_rows; i++) {
    for(j = 0; j < n_cols; j++) {
        matrix[i][j] = 0;
      }
  }
  return matrix;
}

/* Função de leitura e atrubição de valores à matriz e seu tamanho. */
double **read_matrix(char *path_name, superint *n_rows, superint *n_cols) {
  int r;
  superint i, j;
  double v, **matrix;
  FILE *file;

  file = fopen(path_name, "r");
  if (file == NULL) {
      printf ("Erro de abertura/leitura de arquivo!\n'");
      exit(EXIT_FAILURE);
  }
  /* Lẽ a primeira linha do arquivo para obter o tamanho da matriz */
  r = fscanf(file, "%lld %lld", n_rows, n_cols);

  matrix = matrix_malloc(*n_rows, *n_cols);
  /* Lẽ as linhas subsequentes e preenche a matriz ate o final do arquivo */
  r = fscanf(file, "%lld %lld %lf", &i, &j, &v);
  matrix[i-1][j-1] = v;
  while (r != EOF) {
      matrix[i-1][j-1] = v;
      r = fscanf(file, "%lld %lld %lf", &i, &j, &v);
  }
  fclose(file);
  return matrix;
}

/* Função para calculo do tempo de execução */
/* Retirado de: https://stackoverflow.com/a/27448980*/
float timedifference_msec(struct timeval t0, struct timeval t1){
  return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}


/*****************************************************************************/
/* Função Main. Recebe como paramentros:                                     */
/* 1º: letras 'o', 'p' e 's', correspondendo a execução OpenMP, Pthreads e   */
/* sequencial.                                                               */
/* 2º: caminho + nome do arquivo com a Matriz A.                             */
/* 3º: caminho + nome do arquivo com a Matriz B.                             */
/* 4º: caminho + nome do arquivo destino para a Matriz C.                    */
/* Escreve o resultado da multiplicação no arquivo da Matriz C e imprime na  */
/* tela o tempo de execução da multiplicação para implementação escolhida.   */
/*****************************************************************************/
int main(int argc, char *argv[]) {
  struct timeval t0;
  struct timeval t1;
  float elapsed;
  char implementation, path_nameA[200], path_nameB[200], path_nameC[200];
  superint rowsA, colsA, rowsB, colsB;
  double **matrixA, **matrixB, **matrixC;

  if(argc < 5) {
    printf("Quatidade de argumentos invalido!\n Execute: main <implementação> <caminho_matr_A> <caminho_matr_B> <caminho_matr_C>\n");
    exit(1);
  }
  implementation = argv[1][0];
  strcpy(path_nameA, argv[2]);
  strcpy(path_nameB, argv[3]);
  strcpy(path_nameC, argv[4]);
  printf("Arquivo da Matriz A: %s\n", path_nameA);
  printf("Arquivo da Matriz B: %s\n", path_nameB);
  printf("Arquivo destino da Matriz C: %s\n", path_nameC);

  matrixA = read_matrix(path_nameA, &rowsA, &colsA);
  matrixB = read_matrix(path_nameB, &rowsB, &colsB);
  matrixC = matrix_malloc(rowsA, colsB);

  if(colsA != rowsB) {
    printf("A Multiplicação não é possivel!\n A quantidade de colunas da Matriz A deve ser igual a quantidade de linhas da Matrix B.\n");
    free(matrixA);
    free(matrixB);
    free(matrixC);
    exit(1);
  }

  switch (implementation) {
    case 's':
      gettimeofday(&t0, 0); /* Inicio da contagem de execução */
      foolMultiplication(matrixA, matrixB, matrixC, rowsA, colsA, colsB);
      gettimeofday(&t1, 0); /* fim da contagem de execução */
      elapsed = timedifference_msec(t0, t1);
      printf("Multiplicação sequencial (loops simples): Executado em %f milesegundos\n", elapsed);
    break;
    case 'o':
      gettimeofday(&t0, 0); /* Inicio da contagem de execução */
      openmpMultiplication(matrixA, matrixB, matrixC, rowsA,colsA , colsB);
      gettimeofday(&t1, 0); /* fim da contagem de execução */
      elapsed = timedifference_msec(t0, t1);
      printf("Multiplicação com OpenMP: Executado em %f milesegundos\n", elapsed);
    break;
    case 'p':
      gettimeofday(&t0, 0); /* Inicio da contagem de execução */
      pthreadMultiplication(matrixA, matrixB, matrixC, rowsA, colsA, colsB);
      gettimeofday(&t1, 0); /* fim da contagem de execução */
      elapsed = timedifference_msec(t0, t1);
      printf("Multiplicação com Pthreads: Executado em %f milesegundos\n", elapsed);
    break;

   default:
     printf("Opção de implementação invalida!\n");
  }
  print_matrix(matrixC, rowsA, colsB, path_nameC);
  free(matrixA);
  free(matrixB);
  free(matrixC);
  return 0;
}
