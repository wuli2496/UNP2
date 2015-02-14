#include <mqueue.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

volatile sig_atomic_t mqflag;

static void sig_usr1(int);

int main(int argc, char **argv)
{
	mqd_t mqd;
	void *ptr;
	ssize_t n;
	sigset_t zeromask, newmask, oldmask;
	struct mq_attr attr;
	struct sigevent sigev;
	
	if (argc != 2) {
		fprintf(stderr, "usage: exename <name>\n");	
		exit(-1);
	}	

	if ((mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK)) < 0) {
		fprintf(stderr, "mq_open error:%s\n", strerror(errno));
		exit(-1);
	}

	if (mq_getattr(mqd, &attr) < 0) {
		fprintf(stderr, "mq_getattr error:%s\n", strerror(errno));
		exit(-1);
	}

	ptr = malloc(attr.mq_msgsize);
	
	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	sigemptyset(&oldmask);
	sigaddset(&newmask, SIGUSR1);
	
	signal(SIGUSR1, sig_usr1);
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGUSR1;
	if (mq_notify(mqd, &sigev) < 0) {
		fprintf(stderr, "mq_notify error:%s\n", strerror(errno));	
		exit(-1);
	}

	for (;;) {
		sigprocmask(SIG_BLOCK, &newmask, &oldmask);
		while (mqflag == 0) sigsuspend(&zeromask);
		mqflag = 0;
		
		if (mq_notify(mqd, &sigev) < 0) {
			fprintf(stderr, "mq_notify error:%s\n", strerror(errno));
			exit(-1);
		}

		while ((n = mq_receive(mqd, ptr, attr.mq_msgsize, NULL)) >= 0) {
			printf("read %ld bytes\n", (long)n);
		}
		
		if (errno != EAGAIN) {
			fprintf(stderr, "mq_receive error:%s\n", strerror(errno));
			exit(-1);
		}
		sigprocmask(SIG_UNBLOCK, &newmask, NULL);
	}
	exit(0);
}

void sig_usr1(int signo)
{
	mqflag = 1;
}
