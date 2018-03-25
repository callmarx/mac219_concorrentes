/**********************************************************/
/* MAC0219 - Computação Paralela e Distribuída - 2018/01  */
/* Eugenio Augusto Jimenes                                */
/* Nº USP 7118981                                         */
/* MiniDesafio1 - Branch prediction                       */
/**********************************************************/

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXLOOP 1000000

float timedifference_msec(struct timeval t0, struct timeval t1){
  return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

void predictable() {
  int i, x = 0;
  for (i = 0; i < MAXLOOP; i++) {
    if (i % 1 == 0)
      x++;
    else
      x--;
  }
}
 void unpredictable() {
   int i, x = 0;
   for (i = 0; i < MAXLOOP; i++) {
     if (i % 3 == 0)
       x++;
     else
       x--;
   }
 }

int main(int argc, char *argv[]){
  struct timeval t0;
  struct timeval t1;
  float elapsed;
  int func;

  func = atoi(argv[1]);

  /* #### Inicio da contagem de execução #### */
  gettimeofday(&t0, 0);
  if (func == 1)
    predictable();
  else
    unpredictable();
  /* #### Fim da contagem de execução #### */
  gettimeofday(&t1, 0);

  elapsed = timedifference_msec(t0, t1);

  if (func == 1)
    printf("Função previsivel executada em %0.2f milisegundos.\n", elapsed);
  else
    printf("Função imprevisivel executada em %0.2f milisegundos.\n", elapsed);

  return 0;
}
