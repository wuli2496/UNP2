#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

struct mq_attr attr;
struct sigevent sigev;
mqd_t mq;

static void notify_thread(union sigval);

int main(int argc, char **argv)
{

	if ((mq = mq_open(argv[1], O_RDONLY|O_NONBLOCK)) < 0) {
		fprintf(stderr, "mq_open error:%s\n", strerror(errno));
		exit(-1);
	}

	if (mq_getattr(mq, &attr) < 0) {
		fprintf(stderr, "mq_getattr error:%s\n", strerror(errno));
		exit(-1);
	}

	sigev.sigev_notify = SIGEV_THREAD;
	sigev.sigev_notify_function = notify_thread;
	sigev.sigev_value.sival_ptr = NULL;
	sigev.sigev_notify_attributes = NULL;
	
	if (mq_notify(mq, &sigev) < 0) {
		fprintf(stderr, "mq_notify error:%s\n", strerror(errno));
		exit(-1);
	}		

	for (;;) {
		pause();
	}
	exit(0);
}

void notify_thread(union sigval arg) 
{
	ssize_t n;
	void *ptr;

	ptr = calloc(attr.mq_msgsize, sizeof(char));
	mq_notify(mq, &sigev);
	
	while ((n = mq_receive(mq, ptr, attr.mq_msgsize, NULL)) >= 0) {
		printf("read %ld bytes\n", (long)n);
	}

	if (errno != EAGAIN) {
		fprintf(stderr, "mq_receive error:%s\n", strerror(errno));
	}

	free(ptr);
	pthread_exit(NULL);
}
