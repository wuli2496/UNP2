#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	mqd_t mq;
	struct mq_attr attr;
	unsigned int prio;
	size_t len;
	void *ptr;
	ssize_t n;

	if ((mq = mq_open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "mq_open error:%s\n", strerror(errno));
		exit(-1);
	}

	if (mq_getattr(mq, &attr) < 0) {
		fprintf(stderr, "mq_getattr error:%s\n",strerror(errno));
		exit(-1);
	}

	ptr = calloc(attr.mq_msgsize, sizeof(char));
	while ((n = mq_receive(mq, ptr, attr.mq_msgsize, &prio)) >= 0) {
		printf("received %d bytes, msgsize=%ld\n", n, attr.mq_msgsize);
	}	
		
	exit(0);
}
