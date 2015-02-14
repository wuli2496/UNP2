#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

int main(int argc, char **argv)
{
	int flag = O_RDWR | O_CREAT;
	mqd_t mq;
	
	if ((mq = mq_open(argv[1], flag, FILE_MODE, NULL)) < 0) {
		fprintf(stderr, "mq_open error:%s\n", strerror(errno));
		exit(-1);
	}	

	mq_close(mq);
	exit(0);
}
