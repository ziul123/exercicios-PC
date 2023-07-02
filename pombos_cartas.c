#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

sem_t mochila;
sem_t espera;
int mc = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;

	sem_init(&mochila, 0, 0);
	sem_init(&espera, 0, CARTAS);
    pthread_t usuario[N];
    int *id;
    for(i = 0; i < N; i++){
         id = (int *) malloc(sizeof(int));
          *id = i;
	pthread_create(&(usuario[i]),NULL,f_usuario,  (void *) (id));
    }
     pthread_t pombo;
     id = (int *) malloc(sizeof(int));
     *id = 0;
     pthread_create(&(pombo),NULL,f_pombo, (void*) (id));

     pthread_join(pombo,NULL);
   
}


void * f_pombo(void *arg){
  
    while(1){
		sem_wait(&mochila);
		printf("Pombo está a voar de A para B\n");
		sleep(4);
		pthread_mutex_lock(&lock);
		mc = 0;
		pthread_mutex_unlock(&lock);
		for(int i=0;i<CARTAS;i++)
			sem_post(&espera);

        //Inicialmente está em A, aguardar/dorme a mochila ficar cheia (20 cartas)
        //Leva as cartas para B e volta para A
        //Acordar os usuários   
    }
}

void * f_usuario(void *arg){
    
    while(1){
		int id = *((int *) arg);
		printf("Usuário %d está a escrever uma carta\n", id);
		sleep(rand()%3);
		sem_wait(&espera);
		pthread_mutex_lock(&lock);
		printf("Usuário %d está a colocar uma carta na mochila; %d cartas\n", id, ++mc);
		if(mc == CARTAS)
			sem_post(&mochila);
		pthread_mutex_unlock(&lock);

	//Escreve uma carta
        //Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme	
        //Posta sua carta na mochila do pombo
        //Caso a mochila fique cheia, acorda o ṕombo
    }
}
