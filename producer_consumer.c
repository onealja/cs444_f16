#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "mt19937ar.c"

struct item {
   	int val;
	int work;
};

struct item buffer[32];
int index = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condConsume = PTHREAD_COND_INITIALIZER;
pthread_cond_t condProduce = PTHREAD_COND_INITIALIZER;
pthread_t produce;
pthread_t consume;

void interruptHandler(int signal)
{
   	if(signal == SIGINT)
	   	{
		   	pthread_detach(produce);
			pthread_detach(consume);
			exit(EXIT_SUCCESS);
		}
}

void *producer(){
   	//place to put item created
	struct item create;
	int i = 0;

	//random sleep
	while(1){
	   	int rnd = genrand_int32() % 5 + 3;
		printf("PRODUCER: next production in %d\n", rnd);
		sleep(rnd);
		//lock buffer
		pthread_mutex_lock(&mutex);
		//check buffer wait for conditional object from consumer
		while(index == 32){
		   	pthread_cond_wait(&condProduce, &mutex);
			printf("PRODUCER: space filled, waiting for consumtion...\n");
		}
		//values of item to be created
		create.val = genrand_int32() % 10;
		create.work = genrand_int32() % 8 + 2;
		//input into buffer
		buffer[index] = create;
		printf("PRODUCER: item created in spot %d with value %d and work %d\n", index, create.val, create.work);
		index++;
		//send conditional object to consumer
		pthread_cond_signal(&condConsume);
		pthread_mutex_unlock(&mutex);
	}
}

void *consumer(){
   	while(1){
	   	//lock buffer
	   	pthread_mutex_lock(&mutex);
		//make sure buffer is not empty
		while(index == 0){
		   	pthread_cond_wait(&condConsume, &mutex);
		   	printf("CONSUMER %d: waiting on producer\n", pthread_self());
		}
		//unlock
		pthread_mutex_unlock(&mutex);

		sleep(buffer[index-1].work);

		pthread_mutex_lock(&mutex);
		printf("CONSUMER %d: worked %d units to consume item at position %d with value %d\n", pthread_self(), buffer[index-1].work, index-1, buffer[index-1].val);
		if(index > 0)
		   	index -= 1;
		pthread_cond_signal(&condProduce);
		pthread_mutex_unlock(&mutex);
	}
}



int main(int argc, char *argv[])
{
   	long cons;
	int j;
   	//twister initialization
	init_genrand(time(NULL));
	signal(SIGINT, interruptHandler);
	memset(buffer, 0, sizeof(buffer));

	pthread_create(&produce, NULL, producer, NULL);
	
	if(argc < 2){
	   	printf("how many consumers?\n");
		scanf("%d", &cons);
	}
	else
	   	cons = atoi(argv[1]);

	for(j=1; j<=cons; j++)
	   	pthread_create(&consume, NULL, consumer, NULL);

	pthread_join(produce, NULL);
	
	for(j=1; j<=cons; j++);
		pthread_join(consume, NULL);
	
	return 0;
}





