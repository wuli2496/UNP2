#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define MAXLINE 128
#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define SERV_FIFO "/tmp/fifo"

int main(int argc, char **argv)
{
	int readfifo, writefifo;
	size_t len;
	ssize_t n;
	char *ptr, fifoname[MAXLINE], buf[MAXLINE];
	pid_t pid;
	
	pid = getpid();
	snprintf(fifoname, sizeof(fifoname), "/tmp/fifo.%ld", (long)pid);
	if (mkfifo(fifoname, FILE_MODE) < 0 && errno != EEXIST) {
		fprintf(stderr, "can't create:%s\n", fifoname);
		return -1;
	}

	snprintf(buf, sizeof(buf), "%ld ", (long)pid);
	len = strlen(buf);
	ptr = buf + len;
	fgets(ptr, MAXLINE - len, stdin);
	len = strlen(buf);
	writefifo = open(SERV_FIFO, O_WRONLY, 0);
	write(writefifo, buf, len);
	
	readfifo = open(fifoname, O_RDONLY, 0);	
	while ((n = read(readfifo, buf, MAXLINE)) > 0) {
		write(STDOUT_FILENO, buf, n);
	}

	close(readfifo);
	unlink(fifoname);
	exit(0);

}
