#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

mqd_t mqd;
void *buff;
struct mq_attr attr;
struct sigevent sigev;

static void sig_usr1(int);

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "usage: exename <name>\n");
		exit(-1);
	}	

	mqd = mq_open(argv[1], O_RDONLY);	
	if (mqd == -1) {
		fprintf(stderr, "mq_open error:%s\n", strerror(errno));
		exit(-1);
	}

	if (mq_getattr(mqd, &attr) < 0) {
		fprintf(stderr, "mq_getattr error:%s\n", strerror(errno));
		exit(-1);
	}	

	buff = malloc(attr.mq_msgsize);
	signal(SIGUSR1, sig_usr1);
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGUSR1;
	
	if (mq_notify(mqd, &sigev) < 0) {
		fprintf(stderr, "mq_notify error:%s\n", strerror(errno));
		exit(-1);                                                                                                                                              
	}

	signal(SIGUSR1, sig_usr1);
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGUSR1;

	/*	
	if (mq_notify(mqd, &sigev) < 0) {
		fprintf(stderr,"mq_notify error:%s\n", strerror(errno));
		exit(-1);
	}
	*/
	
	for (;;) {
		pause();
	}	

	
	exit(0);
}


void sig_usr1(int signo)
{
	ssize_t n;

	mq_notify(mqd, &sigev);
	n = mq_receive(mqd, buff, attr.mq_msgsize, NULL);
	printf("SIGUSR1 received, read %ld bytes\n", (long)n);
}
