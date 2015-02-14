#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 128
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void my_lock(int), my_unlock(int);

int main(int argc, char **argv)
{
	int fd;
	long i, seqno;
	pid_t pid;
	ssize_t n;
	char line[MAXLINE + 1];
	
	pid = getpid();
	fd = open(argv[1], O_RDWR, FILE_MODE);	
	for (i = 0; i < 20; i++) {
		my_lock(fd);
		lseek(fd, 0L, SEEK_SET);
		n = read(fd, line, MAXLINE);
		line[n] = '\0';
		n = sscanf(line, "%ld\n", &seqno);
		printf("%s:pid=%ld, seq#=%ld\n", argv[0], (long)pid, seqno);
		seqno++;
		snprintf(line, sizeof(line), "%ld\n", seqno);
		lseek(fd, 0L, SEEK_SET);
		write(fd, line, strlen(line));
		my_unlock(fd);
	}	
	exit(0);
}

void my_lock(int fd)
{
	return;
}

void my_unlock(int fd)
{
	return;
}
