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
	int rnd = genrand_int32() % 5 + 3;
	while(1){
	   	sleep(rnd);
		printf("PRODUCER: next production in %d\n", rnd);
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


int main(){
   	//twister initialization
	init_genrand(time(NULL));
	signal(SIGINT, interruptHandler);
	memset(buff, 0, sizeof(buffer));

	pthread_create(&produce, NULL, producer, NULL);

	pthread_join(produce, NULL);

	return 0;
}





