#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_turno = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_mAB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_mBA = PTHREAD_MUTEX_INITIALIZER;
int mAB = 0; //contadores de macacos
int mBA = 0;

void * macacoAB(void * a) {
    int i = *((int *) a);    
    while(1){
		pthread_mutex_lock(&lock_turno);
		pthread_mutex_lock(&lock_mAB);
		if(++mAB == 1) {
			pthread_mutex_lock(&lock_corda);
		}
		pthread_mutex_unlock(&lock_mAB);
		pthread_mutex_unlock(&lock_turno);
		printf("Macaco %d passado de A para B \n",i);
		sleep(1);
		pthread_mutex_lock(&lock_mAB);
		if(--mAB == 0) {
			pthread_mutex_unlock(&lock_corda);
		}
		pthread_mutex_unlock(&lock_mAB);
    }
    pthread_exit(0);
}

void * macacoBA(void * a) {
    int i = *((int *) a);    
    while(1){
		pthread_mutex_lock(&lock_turno);
		pthread_mutex_lock(&lock_mBA);
		if(++mBA == 1) {
			pthread_mutex_lock(&lock_corda);
		}
		pthread_mutex_unlock(&lock_mBA);
		pthread_mutex_unlock(&lock_turno);
		printf("Macaco %d passado de B para A \n",i);
		sleep(1);
		pthread_mutex_lock(&lock_mBA);
		if(--mBA == 0) {
			pthread_mutex_unlock(&lock_corda);
		}
		pthread_mutex_unlock(&lock_mBA);
    }
    pthread_exit(0);
}



void * gorila(void * a){
    while(1){
		pthread_mutex_lock(&lock_turno);
		pthread_mutex_lock(&lock_corda);
		printf("Gorila passado de A para B \n");
		pthread_mutex_unlock(&lock_turno);
		sleep(5);
		pthread_mutex_unlock(&lock_corda);
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
	pthread_create(&g, NULL, &gorila, NULL);

  
    pthread_join(macacos[0], NULL);
    return 0;
}
