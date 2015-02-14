#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>

void client(int readfd, int writefd)
{
	size_t len;
	ssize_t n;
	char buf[128];
	
	fgets(buf, 128, stdin);
	len = strlen(buf);
	if (buf[len - 1] == '\n') len--;

	write(writefd, buf, len);
	
	while ((n = read(readfd, buf, 128)) > 0) {
		write(STDOUT_FILENO, buf, n);
	}	
}

void server(int readfd, int writefd)
{
	int fd;
	ssize_t n;
	char buf[128];

	if ((n = read(readfd, buf, 128)) < 0) {
		fprintf(stderr, "read error:%s\n", strerror(errno));
		return;
	}

	buf[n] = 0;

	if ((fd = open(buf, O_RDONLY)) < 0) {
		snprintf(buf + n, sizeof(buf) - n, ": can't openr:%s\n", strerror(errno));
		n = strlen(buf);
		write(writefd, buf, n);
	} else {
		while ((n = read(fd, buf, 128)) > 0) {
			write(writefd, buf, n);
		}

		close(fd);
	}

	
}
int main()
{
	int pipe1[2], pipe2[2];
	pid_t childpid;
	
	pipe(pipe1);
	pipe(pipe2);

	if ((childpid = fork()) == 0) {
		close(pipe1[1]);
		close(pipe2[0]);

		server(pipe1[0], pipe2[1]);
		exit(0);
	}

	close(pipe1[0]);
	close(pipe2[1]);
	
	client(pipe2[0], pipe1[1]);

	waitpid(childpid, NULL, 0);
	exit(0);
	
	return 0;
}
