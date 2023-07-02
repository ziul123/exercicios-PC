#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 3 //número de produtores
#define CN 3 // número de consumidores
#define N 10  //tamanho do buffer

int bc = 0;

pthread_cond_t p_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t cond_lock = PTHREAD_MUTEX_INITIALIZER;

void * produtor(void * meuid);
void * consumidor (void * meuid);


void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

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
  int id = *(int *) pi;
  while(1)
  {
	sleep(rand()%3); //produzindo dados
	pthread_mutex_lock(&cond_lock);
	while(bc == N)
		pthread_cond_wait(&p_cond, &cond_lock);

	bc++;
	printf("Produtor %d está a escrever dados; tamanho do buffer: %d\n", id, bc);

	if(bc == 1)
		pthread_cond_signal(&c_cond);
	pthread_mutex_unlock(&cond_lock);


  }
  pthread_exit(0);
  
}

void * consumidor (void* pi)
{
  int id = *(int *) pi;
 while(1)
  {
 
	pthread_mutex_lock(&cond_lock);
	while(bc == 0)
		pthread_cond_wait(&c_cond, &cond_lock);

	bc--;
	printf("Consumidor %d está a ler dados; tamanho do buffer: %d\n", id, bc);

	if(bc == N-1)
		pthread_cond_signal(&p_cond);
	pthread_mutex_unlock(&cond_lock);
	sleep(rand()%3); //utilizando dados

  }
  pthread_exit(0);
  
}
