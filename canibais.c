#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

pthread_cond_t canibais_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cozinheiro_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int porcoes = 0;

void *canibal(void*meuid);
void *cozinheiro(int m);


void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if(argc != 3){
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }
  
  n = atoi (argv[1]); //número de canibais
  m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
  printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

  if(n > MAXCANIBAIS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }
  
  for (i = 0; i < n; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cozinheiro(m);
}

void * canibal (void* pi){
  
  while(1) {
	pthread_mutex_lock(&mutex);
	while(porcoes == 0)
		pthread_cond_wait(&canibais_cond, &mutex);

	if(--porcoes == 0)
		pthread_cond_signal(&cozinheiro_cond);

	pthread_mutex_unlock(&mutex);
     
    printf("%d: vou comer a porcao que peguei\n", *(int *)(pi));
    sleep(10);
  }
  
}

void *cozinheiro (int m){
 
  while(1){
     pthread_mutex_lock(&mutex);
	 while(porcoes > 0)
		 pthread_cond_wait(&cozinheiro_cond, &mutex);
    printf("cozinheiro: vou cozinhar\n");
    sleep(20);
	porcoes = m;
	pthread_cond_broadcast(&canibais_cond);
	pthread_mutex_unlock(&mutex);
   }

}
