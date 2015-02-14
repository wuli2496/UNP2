#include <mqueue.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void sig_usr1(int);

int fd[2];

int main(int argc, char **argv)
{
	mqd_t mq;
	struct mq_attr attr;
	void *ptr;
	int nfds;
	ssize_t n;
	fd_set rset;
	struct sigevent sigev;
	char c;

	if ((mq = mq_open(argv[1], O_RDONLY|O_NONBLOCK)) < 0) {
		fprintf(stderr, "mq_open error:%s\n", strerror(errno));
		exit(-1);
	}

	if (mq_getattr(mq, &attr) < 0) {
		fprintf(stderr, "mq_getattr error:%s\n", strerror(errno));	
		exit(-1);
	}

	ptr = calloc(attr.mq_msgsize, sizeof(char));
	
	signal(SIGUSR1, sig_usr1);
	sigev.sigev_notify = SIGEV_SIGNAL;	
	sigev.sigev_signo= SIGUSR1;
	if (mq_notify(mq, &sigev) < 0) {
		fprintf(stderr, "mq_notify error:%s\n", strerror(errno));
		exit(-1);
	}

	if (pipe(fd) < 0) {
		fprintf(stderr, "pipe error:%s\n", strerror(errno));	
		exit(-1);
	}
	
	FD_ZERO(&rset);
	for (;;) {
		FD_SET(fd[0], &rset);
		nfds = select(fd[0] + 1, &rset, NULL, NULL, NULL);
		if (nfds <= 0) continue;
		if (FD_ISSET(fd[0], &rset)) {
			read(fd[0], &c, 1);
			mq_notify(mq, &sigev);
			while ((n = mq_receive(mq, ptr, attr.mq_msgsize, NULL)) >= 0) {
				printf("read %ld bytes\n", (long)n);
			}			

			if (errno != EAGAIN) fprintf(stderr, "mq_receive error:%s\n", strerror(errno));
		}
	}
	exit(0);
}

void sig_usr1(int signo)
{
	write(fd[1], "", 1);
}
