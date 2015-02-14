#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAXLINE 128
#define SERV_FIFO "/tmp/fifo"
#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

size_t readline(int fd, void *ptr, size_t maxsize)
{
    char *p = ptr;
    size_t rc, n;
    char c;

    for (n = 1; n < maxsize; n++) {
again:
        if ((rc = read(fd, &c, 1)) == 1) {
            *p++ = c;
            if (c == '\n') break;
            } else if (rc == 0) {
                if (n == 1) return 0;
                else break;
        } else {
            if (errno == EINTR) goto again;
            return -1;
        }
    }

    *p = 0;
    return n;
}

int main(int argc, char **argv)
{
	int readfd, writefd, dummyfd, fd;
	char *ptr, buf[MAXLINE], fifoname[MAXLINE];
	pid_t pid;
	ssize_t n;

	if ((mkfifo(SERV_FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
		fprintf(stderr, "can't create %s\n", SERV_FIFO);
		return -1;
	}	

	readfd = open(SERV_FIFO, O_RDONLY, 0);	
	dummyfd = open(SERV_FIFO, O_WRONLY, 0);
	
	while ((n = readline(readfd, buf, MAXLINE)) > 0) {
		if (buf[n - 1] == '\n') n--;
		buf[n] = 0;
		printf("buf=%s\n", buf);		
		if ((ptr = strchr(buf, ' ')) == NULL) {
			fprintf(stderr, "error strchr:%s\n", strerror(errno));
			continue;
		}

		*ptr++ = 0;
		pid = atoi(buf);
		snprintf(fifoname, sizeof(fifoname), "/tmp/fifo.%ld", (long)pid);
		if ((writefd = open(fifoname, O_WRONLY, 0)) < 0) {
			fprintf(stderr, "open fifoname error:%s\n", strerror(errno));
			continue;
		}

		if ((fd = open(ptr, O_RDONLY)) < 0) {
			snprintf(buf + n, sizeof(buf) - n, ":can't open,%s\n", strerror(errno));
			n = strlen(ptr);
			write(writefd, ptr, n);
			close(writefd);
		} else {
			while ((n = read(fd, buf, MAXLINE)) > 0) {
				write(writefd, buf, n);
			}

			close(fd);
			close(writefd);
		}		
	}	
	exit(0);
}
