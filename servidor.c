#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define TAM 5

pthread_barrier_t barrier;  

int bufferT1[TAM];
int prod_posT1=0;
int cons_posT1 = 0;
pthread_mutex_t bufferT1_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t posicoes_livresT1;
sem_t posicoes_ocupadasT1;


int bufferT2[TAM];
int prod_posT2=0;
int cons_posT2 = 0;
pthread_mutex_t bufferT2_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t posicoes_livresT2;
sem_t posicoes_ocupadasT2;


pthread_t w1,w2;

void insereT1(int value){
    	sem_wait(&posicoes_livresT1);
        pthread_mutex_lock(&bufferT1_mutex); 		
	bufferT1[prod_posT1] = value;
        prod_posT1 = (prod_posT1+1)%TAM;
	pthread_mutex_unlock(&bufferT1_mutex);
	sem_post(&posicoes_ocupadasT1);
}

int removeT1(){
        sem_wait(&posicoes_ocupadasT1);
	pthread_mutex_lock(&bufferT1_mutex);
	int item = bufferT1[cons_posT1];
        cons_posT1 = (cons_posT1+1)%TAM;
	pthread_mutex_unlock(&bufferT1_mutex);
	sem_post(&posicoes_livresT1);
        return item;
}

void insereT2(int value){
    	sem_wait(&posicoes_livresT2);
        pthread_mutex_lock(&bufferT2_mutex); 		
	bufferT2[prod_posT2] = value;
        prod_posT2 = (prod_posT2+1)%TAM;
	pthread_mutex_unlock(&bufferT2_mutex);
	sem_post(&posicoes_ocupadasT2);
}

int removeT2(){
        sem_wait(&posicoes_ocupadasT2);
	pthread_mutex_lock(&bufferT2_mutex);
	int item = bufferT2[cons_posT2];
        cons_posT2 = (cons_posT2+1)%TAM;
	pthread_mutex_unlock(&bufferT2_mutex);
	sem_post(&posicoes_livresT2);
        return item;
}





void * worker1(void *arg) {
  int myid = *(int *)(arg);
  while(1){
     int req = removeT1();
     if(req == 0){//leitura
	printf("T1 executando uma leitura\n");
	sleep(1);
     }else{//escrita
        pthread_barrier_wait(&barrier); 
	if(myid == 0){
           printf("T1 executando uma escrita\n");
           sleep(5);
        }else{
           printf("T1 aguardando uma escrita\n");
      
        }
        pthread_barrier_wait(&barrier); 
     } 
  }
}

void * worker2(void *arg) {
  int myid = *(int *)(arg);
  while(1){
     int req = removeT2();
     if(req == 0){//leitura
	printf("T2 executando uma leitura\n");
	sleep(1);
     }else{//escrita
        pthread_barrier_wait(&barrier); 
	if(myid == 0){
           printf("T2 executando uma escrita\n");
           sleep(5);
        }else{
           printf("T2 aguardando uma escrita\n");
      
        }
        pthread_barrier_wait(&barrier); 
     } 
  }
}




int main(int argc, char *argv[]) {
  

  sem_init(&posicoes_livresT1,0,TAM);    
  sem_init(&posicoes_ocupadasT1,0,0);
  sem_init(&posicoes_livresT2,0,TAM);    
  sem_init(&posicoes_ocupadasT2,0,0);

  pthread_barrier_init(&barrier, NULL, 2);

  int *id;
  id = (int *) malloc(sizeof(int));
  *id = 0;
  pthread_create(&w1, NULL, worker1, (void *) (id));

  id = (int *) malloc(sizeof(int));
  *id = 1;
  pthread_create(&w2, NULL, worker2, (void *) (id));

  srand48(time(NULL));

  int prox = 0;
  while(1){
     int rand = (int) (drand48() * 100.0);
     
     if(rand < 70){//leitura
        int req = 0;
        if(prox == 0){
           insereT1(req);
        }else{
           insereT2(req);
        }
        prox = (prox+1)%2;
     }else{//escrita
        int req = 1;
        insereT1(req);
        insereT2(req);
     
     }
     sleep(1);
  }

}


