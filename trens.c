#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define X 8		//número de baias de descarga
#define Y 2		//número de baias exclusivas de carga
#define Z X-Y	//número de baias de uso geral
#define C 8		//número de trens de carga
#define P 4		//número de trens de passageiro

#define BREAK rand()%3==0	//chance de um trem quebrar

void *trem_carga(void *arg);
void *trem_pass(void *arg);
void *manutencao(void *arg);
void receber_manutencao(int id, char *tipo);

sem_t cargas;
sem_t gerais;
sem_t manu_idle;
sem_t espera;
pthread_mutex_t em_manu = PTHREAD_MUTEX_INITIALIZER;

int main() {
	int *id;
	pthread_t trens_carga[C];
	pthread_t trens_pass[P];
	pthread_t equipe_m;
	sem_init(&cargas, 0, Y);
	sem_init(&gerais, 0, Z);
	sem_init(&manu_idle, 0, 0);
	sem_init(&espera, 0, 0);
	for (int i=0;i<C;i++) {
		id = malloc(sizeof(int));
		*id = i;
		pthread_create(&(trens_carga[i]), NULL, trem_carga, (void *) id);
	}
	for (int i=0;i<P;i++) {
		id = malloc(sizeof(int));
		*id = i;
		pthread_create(&(trens_pass[i]), NULL, trem_pass, (void *) id);
	}

	pthread_create(&equipe_m, NULL, manutencao, NULL);

	pthread_join(equipe_m, NULL);
}

void *trem_carga(void *arg) {
	int id = *((int *) arg);
	while (1) {
		sleep(rand()%4 + 3);	//trem voltando para a estação
		int flag;	//qual baia conseguiu, 0 se carga, 1 se geral
		if (sem_trywait(&cargas) == 0) {
			flag = 0;	//conseguiu baia de carga
		} else if (sem_trywait(&gerais) == 0) {
			flag = 1;	//conseguiu geral
		} else {
			sem_wait(&cargas);
			flag = 0;
		}
		printf("Trem de carga %d descarregando em baia de %s.\n", id, flag?"uso geral":"carga");
		sleep(rand()%2);

		if (BREAK)
			receber_manutencao(id, "carga");

		if (flag)
			sem_post(&gerais);
		else
			sem_post(&cargas);
		printf("Trem de carga %d saindo da estação.\n", id);
	}
}

void *trem_pass(void *arg) {
	int id = *((int *) arg);
	while (1) {
		sleep(rand()%4 + 3);	//trem voltando para a estação
		sem_wait(&gerais);
		printf("Trem %d descarregando passageiros.\n", id);
		sleep(rand()%2);
		if (BREAK)
			receber_manutencao(id, "passageiros");
		sem_post(&gerais);
		printf("Trem de passageiros %d saindo da estação.\n", id);
	}
}

void *manutencao(void *arg) {
	while (1) {
		sem_wait(&manu_idle);
		sleep(2);	//consertando o trem
		sem_post(&espera);
	}
}

void receber_manutencao(int id, char *tipo) {
	printf("Trem de %s %d quebrou, esperando manutenção.\n", tipo, id);
	pthread_mutex_lock(&em_manu);
	sem_post(&manu_idle);
	sem_wait(&espera);
	printf("Trem de %s %d recebeu manutenção.\n", tipo, id);
	pthread_mutex_unlock(&em_manu);
}
