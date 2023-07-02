#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXSIZE 10000  /* maximum matrix size */

pthread_barrier_t barrier;  
int size;


int matrix1[MAXSIZE][MAXSIZE];
int vetorR[MAXSIZE];

void *Worker(void *);

int main(int argc, char *argv[]) {
  int i, j;
 
  
  size = atoi(argv[1]);
  
   if(size > MAXSIZE){
	printf("Tamanho muito grande!\n");
        return 0;	
   }

  pthread_t workerid[size];
  

  pthread_barrier_init(&barrier, NULL, size);


  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++)
      matrix1[i][j] = 1;
  

	for (i = 0; i < size; i++)
		vetorR[i] = 0;


  int * id;
  for (i = 0; i < size; i++){
     id = (int *) malloc(sizeof(int));
     *id = i;
     pthread_create(&workerid[i], NULL, Worker, (void *) (id));

  }


  for (i = 0; i < size; i++){
       if(pthread_join(workerid[i], NULL)){
          printf("\n ERROR joining thread");
          exit(1);
       }
  }
  printf("Bye!\n");
}


void *Worker(void *arg) {
  int myid = *(int *)(arg);

  int self = pthread_self();
  int i;
 
  printf("worker %d (pthread id %d) has started\n", myid, self);

	for(i=0;i<size;i++)
		vetorR[myid] += matrix1[myid][i];

	printf("waiting at barrier\n");
    pthread_barrier_wait(&barrier);

    if (myid == 0) {
		int soma = 0;
		for(i=0;i<size;i++)
			soma += vetorR[i];

		printf("Soma dos elementos da matriz: %d\n", soma);
     }
	pthread_exit(NULL);

}

