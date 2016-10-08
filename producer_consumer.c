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

pthread_mutex_t mutex;
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

void *producer()
{
   	//place to put item created
	struct item create;
	int i = 0;

	//random sleep
	int rnd = genrand_int32() % 5 + 3;
	while(1){
	   	sleep(rnd);
		printf("next production in %d", rnd);
		//lock buffer
		pthread_mutex_lock(&mutex);
		//check buffer
		while(index == 32){
		   	pthread_cond_wait(&condProduce, &mutex);
			printf("space filled, waiting for consumtion...");
		}
		//values of item to be created
		create.val = genrand_int32() % 10;
		create.work = genrand_int32() %



