#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define MAXNITEMS 1000000
#define MAXNTHREADS 100
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int nitems;
int buf[MAXNITEMS];

struct
{
	pthread_mutex_t mutex;
	int nput;
	int nval;
}put = {PTHREAD_MUTEX_INITIALIZER};

struct
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int nready;
}nready = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

void *produce(void*), *consume(void*);

int main(int argc, char **argv)
{
	int i, nthreads, count[MAXNTHREADS];
	pthread_t tid_produce[MAXNTHREADS], tid_consume;

	if (argc != 3) {
		fprintf(stderr, "usage:exename <items> <threads>\n");
		exit(-1);
	}

	nitems = MIN(atoi(argv[1]), MAXNITEMS);	
	nthreads = MIN(atoi(argv[2]), MAXNTHREADS);	

	//set_concurrency(nthreads);
	pthread_setconcurrency(nthreads);	

	for (i = 0; i < nthreads; i++) {
		count[i] = 0;
		pthread_create(&tid_produce[i], NULL, produce, &count[i]);
	}
	
	
	pthread_create(&tid_consume, NULL, consume, NULL);
	for (i = 0; i < nthreads; i++) {
		pthread_join(tid_produce[i], NULL);
		printf("count[%d] = %d\n", i, count[i]);
	}

	//pthread_create(&tid_consume, NULL, consume, NULL);
	pthread_join(tid_consume, NULL);
	exit(0);
}

void *produce(void *arg)
{
	for (;;) {
		pthread_mutex_lock(&put.mutex);
		if (put.nput >= nitems) {
			pthread_mutex_unlock(&put.mutex);
			return;
		}
		
		buf[put.nput] = put.nval;
		put.nput++;
		put.nval++;
		pthread_mutex_unlock(&put.mutex);
		
		int dosignal;
		pthread_mutex_lock(&nready.mutex);
		dosignal = (nready.nready == 0);
		nready.nready++;
		pthread_mutex_unlock(&nready.mutex);	
		if (dosignal) pthread_cond_signal(&nready.cond);
		*((int*)arg) += 1;
		
	}
}



void *consume(void *arg)
{
	int i;
	for (i = 0; i < nitems; i++) {
		pthread_mutex_lock(&nready.mutex);
		while (nready.nready == 0) pthread_cond_wait(&nready.cond, &nready.mutex);
		nready.nready--;
		pthread_mutex_unlock(&nready.mutex);
		if (buf[i] != i) {
			printf("buff[%d] = %d\n", i, buf[i]);
		}
	}

	return NULL;
}
