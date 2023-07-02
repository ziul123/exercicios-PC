#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

pthread_mutex_t lock_turno = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_condicao = PTHREAD_MUTEX_INITIALIZER;
int mAB = 0; //contadores de macacos
int mBA = 0;
int gAB = 0;
int gBA = 0;
int gAB_q = 0; //flag de gorila querendo passar
int gBA_q = 0;
pthread_cond_t AB_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t BA_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t gAB_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t gBA_cond = PTHREAD_COND_INITIALIZER;

void * macacoAB(void * a) {
    int i = *((int *) a);    
    while(1){
		pthread_mutex_lock(&lock_turno);
		pthread_mutex_lock(&lock_condicao);
		while(mBA > 0 || gAB || gBA || gAB_q || gBA_q) {
			pthread_cond_wait(&AB_cond, &lock_condicao);
		}
		mAB++;
		pthread_mutex_unlock(&lock_condicao);
		pthread_mutex_unlock(&lock_turno);
		printf("Macaco %d passado de A para B \n",i);
		sleep(1);
		pthread_mutex_lock(&lock_condicao);
		if(--mAB == 0) {
			pthread_cond_broadcast(&BA_cond);
			pthread_cond_signal(&gAB_cond);
			pthread_cond_signal(&gBA_cond);
		}
		pthread_mutex_unlock(&lock_condicao);
    }
    pthread_exit(0);
}

void * macacoBA(void * a) {
    int i = *((int *) a);    
    while(1){
		pthread_mutex_lock(&lock_turno);
		pthread_mutex_lock(&lock_condicao);
		while(mAB > 0 || gAB || gBA || gAB_q || gBA_q) {
			pthread_cond_wait(&BA_cond, &lock_condicao);
		}
		mBA++;
		pthread_mutex_unlock(&lock_condicao);
		pthread_mutex_unlock(&lock_turno);
		printf("Macaco %d passado de B para A \n",i);
		sleep(1);
		pthread_mutex_lock(&lock_condicao);
		if(--mBA == 0) {
			pthread_cond_broadcast(&AB_cond);
			pthread_cond_signal(&gAB_cond);
			pthread_cond_signal(&gBA_cond);
		}
		pthread_mutex_unlock(&lock_condicao);
    }
    pthread_exit(0);
}



void * gorilaAB(void * a){
    while(1){
		pthread_mutex_lock(&lock_turno);
		pthread_mutex_lock(&lock_condicao);
		gAB_q = 1;
		while(mAB > 0 || mBA > 0 || gBA) {
			pthread_cond_wait(&gAB_cond, &lock_condicao);
		}
		pthread_mutex_unlock(&lock_condicao);
		printf("Gorila passando de A para B\n");
		sleep(5);
		pthread_mutex_unlock(&lock_turno);
		pthread_mutex_lock(&lock_condicao);
		pthread_cond_broadcast(&AB_cond);
		pthread_cond_broadcast(&BA_cond);
		pthread_cond_signal(&gBA_cond);
		pthread_mutex_unlock(&lock_condicao);
		gAB_q = 0;
		sleep(1);
     }
    pthread_exit(0);
}

void * gorilaBA(void * a){
    while(1){
		pthread_mutex_lock(&lock_turno);
		pthread_mutex_lock(&lock_condicao);
		gBA_q = 1;
		while(mAB > 0 || mBA > 0 || gAB) {
			pthread_cond_wait(&gBA_cond, &lock_condicao);
		}
		pthread_mutex_unlock(&lock_condicao);
		printf("Gorila passando de B para A\n");
		sleep(5);
		pthread_mutex_unlock(&lock_turno);
		pthread_mutex_lock(&lock_condicao);
		pthread_cond_broadcast(&AB_cond);
		pthread_cond_broadcast(&BA_cond);
		pthread_cond_signal(&gAB_cond);
		pthread_mutex_unlock(&lock_condicao);
		gBA_q = 0;
		sleep(1);
     }
    pthread_exit(0);
}

int main(int argc, char * argv[])
{
    pthread_t macacos[MA+MB];
    int *id;
    int i = 0;

    for(i = 0; i < MA+MB; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(i%2 == 0){
          if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }else{
          if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }
    }
    pthread_t g;
	pthread_create(&g, NULL, &gorilaAB, NULL);
	pthread_create(&g, NULL, &gorilaBA, NULL);

  
    pthread_join(macacos[0], NULL);
    return 0;
}
