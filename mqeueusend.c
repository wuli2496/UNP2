#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	mqd_t mq;
	void *ptr;
	size_t len;
	unsigned int prio;
	
	if (argc != 4) {
		fprintf(stderr, "exename <name> <bytes> <priority>\n");
		exit(-1);
	}

	if ((mq = mq_open(argv[1], O_WRONLY)) < 0) {
		fprintf(stderr, "mq_open error:%s\n", strerror(errno));
		exit(-1);
	}

	len = atoi(argv[2]);
	prio = atoi(argv[3]);

	ptr = calloc(len, sizeof(char));
	if (mq_send(mq, ptr, len, prio) < 0) {
		fprintf(stderr, "mq_send error:%s\n", strerror(errno));
		exit(-1);
	}
	
	exit(0);	
}
