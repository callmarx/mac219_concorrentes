#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>

#define MALE 0
#define FEMALE 1

typedef struct {
  int id;
  int position;
//  int gender;
  pthread_t thread;
} frog;

frog *frogs;
int *stones;
int number_of_frogs_m;
int number_of_frogs_f;
int number_of_stones;
int deadlock;
int deadlock_count;
int impossible_going;
int done;
pthread_mutex_t mutex;
pthread_cond_t condition;


void printStones() {
  int i, j;
  printf("Pedras do lago:\n");
  for (i = 0; i < number_of_stones; i++) {
    if (stones[i] == -1)
      /* se estiver vazio */
      printf("__ ");
    else if (stones[i] < number_of_frogs_f)
      /* se for rã */
      printf("F%d ", stones[i]);
    else
      /* se for sapo */
      printf("M%d ", stones[i]);
  }
  printf("\n");
}

void checkFinished() {
  int i;
  for (i = 0; i < number_of_frogs_m; i++)
    if (stones[i] == -1 || stones[i] < number_of_frogs_f)
      /* Se a pedra vazia esta na posição que não seja em stones[number_of_frogs_m]
      * ou se há rãs onde devieria ter sapos então não terminou */
      return;
  for (i = number_of_frogs_m + 1; i < number_of_stones; i++)
    if (stones[i] == -1 || stones[i] > number_of_frogs_f)
      /* Se a pedra vazia esta na posição que não seja em stones[number_of_frogs_m]
      * ou se há sapos onde devieria ter rãs então não terminou */
      return;
  done = 1;
}

void checkImpossibleGoing() {
  int i, left, left_two, right, right_two;
  for (i = 0; i < number_of_stones; i++) {
    if (stones[i] == -1) {
      left = i - 1;
      left_two = i - 2;
      right = i + 1;
      right_two = i + 2;

      if (left >= 0 && stones[left] < number_of_frogs_f)
      /* Se a esquerda da padra vazia há uma rã, ela ainda pode pular*/
        return;
      if (left_two >= 0 && stones[left_two] < number_of_frogs_f)
      /* Se dois a esquerda da padra vazia há uma rã, ela ainda pode pular*/
        return;
      if (right < number_of_stones && stones[right] > number_of_frogs_f)
      /* Se a direita da padra vazia há um sapo, ele ainda pode pular*/
        return;
      if (right_two < number_of_stones && stones[right_two] > number_of_frogs_f)
      /* Se dois a direita da padra vazia há um sapo, ele ainda pode pular*/
        return;
      /* Se nenhum dos casos acima ocorre, então não há mais quem pule :( */
    }
  }
  impossible_going = 1;
}

// int keepGoing() {
//
// }

void *frogJump(void *frog_m) {
  int position, left, left_two, right, right_two;
  frog *f = (frog *) frog_m;

  // pthread_mutex_lock(&mutex);
  // pthread_cond_wait(&condition, &mutex);
  // pthread_mutex_unlock(&mutex);

  while (deadlock_count < deadlock) {
    pthread_mutex_lock(&mutex);

    position = f->position;
    left = position - 1;
    left_two = position - 2;
    right = position + 1;
    right_two = position + 2;
    if (left >= 0 && stones[left] == -1 && f->id > number_of_frogs_f) {
      stones[left] = f->id;
      stones[position] = -1;
      f->position = left;
      deadlock_count = 0;
      printStones();
    }
    else if (left_two >= 0 && stones[left_two] == -1 && f->id > number_of_frogs_f) {
      stones[left_two] = f->id;
      stones[position] = -1;
      f->position = left_two;
      deadlock_count = 0;
      printStones();
    }
    else if (right < number_of_stones && stones[right] == -1 && f->id <= number_of_frogs_f) {
      stones[right] = f->id;
      stones[position] = -1;
      f->position = right;
      deadlock_count = 0;
      printStones();
    }
    else if (right_two < number_of_stones && stones[right_two] == -1 && f->id <= number_of_frogs_f) {
      stones[right_two] = f->id;
      stones[position] = -1;
      f->position = right_two;
      deadlock_count = 0;
      printStones();
    }
    else {
      deadlock_count++;
      //printf("deadlock_count = %d e deadlock = %d\n", deadlock_count, deadlock);
    }
    checkFinished();
    checkImpossibleGoing();
    if(impossible_going || done) {
      pthread_mutex_unlock(&mutex);
      pthread_cond_signal(&condition);
      break;
    }
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condition);
  }
  pthread_exit(NULL);
}

int main(int argc, char ** argv) {
  int i;

  if (argc < 3) {
    printf("ERRO! Quantidade de argumentos invalidada. Passe o numero de rãs e o numero de sapos para o problema.\n");
    exit(EXIT_FAILURE);
  }
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&condition, NULL);
  number_of_frogs_f = atoi(argv[1]);
  number_of_frogs_m = atoi(argv[2]);
  number_of_stones = number_of_frogs_f + number_of_frogs_m + 1;
  deadlock = 1000 * (number_of_frogs_f + number_of_frogs_m);
  deadlock_count = 0;
  impossible_going = 0;
  done = 0;
  stones = (int *) malloc(number_of_stones * sizeof(int));
  frogs = (frog *) malloc((number_of_frogs_f + number_of_frogs_m) * sizeof(frog));
  for (i = 0; i < number_of_frogs_f; i++) {
    stones[i] = i;
    frogs[i].id = i;
    frogs[i].position = i;
    //pthread_create(&frogs[i].thread, NULL, frogJump, &frogs[i]);
  }
  stones[number_of_frogs_f] = -1;
  for (i = number_of_frogs_f + 1; i < number_of_stones; i++) {
    stones[i] = i;
    frogs[i-1].id = i-1;
    frogs[i-1].position = i;
    //pthread_create(&frogs[i-1].thread, NULL, frogJump, &frogs[i-1]);
  }
  printf("Posição inicial das rãs e sapos\n");
  printStones();
  printf("Programa inicializado!\n");
  for (i = 0; i < number_of_frogs_f + number_of_frogs_m; i++) {
    pthread_create(&frogs[i].thread, NULL, frogJump, &frogs[i]);
    pthread_cond_wait (&condition, &mutex);
  }
  if (deadlock_count >= deadlock)
    printf("deadlock atingido! Foram %s tentativas de pulo sem sucesso!\n", deadlock_count);
  if (impossible_going) {
    printf("Chegado num posicionamento sem solução!!! :(\n");
    printStones();
  }
  if (done) {
    printf("Solução Encontrada!!!\n");
    printStones();
  }
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&condition);
  free(stones);
  free(frogs);
  return 0;
}
