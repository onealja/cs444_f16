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


