#include <mqueue.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

int main(int argc, char **argv)
{
	mqd_t mq;
	struct mq_attr attr;
	void *ptr;
	size_t len;
	ssize_t n;
	struct sigevent sigev;
	sigset_t newmask;
	int signo;

	if ((mq = mq_open(argv[1], O_RDONLY|O_NONBLOCK)) < 0) {
		fprintf(stderr, "mq_open error:%s\n", strerror(errno));
		exit(-1);
	}

	if (mq_getattr(mq, &attr) < 0) {
		fprintf(stderr, "mq_getattr error:%s\n", strerror(errno));
		exit(-1);
	}

	ptr = calloc(attr.mq_msgsize, sizeof(char));
	
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGUSR1);
	sigprocmask(SIG_BLOCK, &newmask, NULL);
	
	sigev.sigev_notify = SIGEV_SIGNAL;	
	sigev.sigev_signo = SIGUSR1;
	if (mq_notify(mq, &sigev) < 0) {
		fprintf(stderr, "mq_notify error:%s\n", strerror(errno));
		exit(-1);
	}	

	for (;;) {
		sigwait(&newmask, &signo);
		if (signo == SIGUSR1) {
			mq_notify(mq, &sigev);
			while ((n = mq_receive(mq, ptr, attr.mq_msgsize, NULL)) >= 0) {
				printf("read %ld bytes\n", (long)n);
			}
			if (errno != EAGAIN) {
				fprintf(stderr, "mq_receive error\n");
				exit(-1);
			}
		}
	}
	exit(0);
}
