#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PR 5 //número de produtores
#define CN 5 // número de consumidores
#define N 10  //tamanho do buffer

int qnt = 0;

sem_t vazias;
sem_t cheias;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * produtor(void * meuid);
void * consumidor (void * meuid);


void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

  sem_init(&vazias, 0, N);
  sem_init(&cheias, 0, 0);

  pthread_t tid[PR];
   
  for (i = 0; i < PR; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
 
  pthread_join(tid[0],NULL);

}

void * produtor (void* pi)
{
	int id = *((int *) pi);
 	while(1) {
		sleep(rand()%3); //produzindo dados
		sem_wait(&vazias);
		pthread_mutex_lock(&lock);
		printf("Produtor %d está a escrever dados; tamanho do buffer: %d\n", id, ++qnt);
		pthread_mutex_unlock(&lock);
		sem_post(&cheias);
  }
  pthread_exit(0);
  
}

void * consumidor (void* pi)
{
	int id = *((int *) pi);
	while(1) {
		sem_wait(&cheias);
		pthread_mutex_lock(&lock);
		printf("Consumidor %d está a ler dados; tamanho do buffer: %d\n", id, --qnt);
		pthread_mutex_unlock(&lock);
		sem_post(&vazias);
		sleep(rand()%3); //utilizando dados

	}
  pthread_exit(0);
  
}
