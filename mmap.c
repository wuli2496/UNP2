#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
 
#define SEM_NAME "/abc"
int main(int argc, char **argv)
{
	int fd, i, nloop, zero = 0;
	int *ptr;
	sem_t *mutex;
	
	if (argc != 3) {
		fprintf(stderr, "usage:exe <pathname> <loops>\n");
		exit(-1);
	}

	nloop = atoi(argv[2]);
	fd = open(argv[1], O_RDWR | O_CREAT, 0666);
	write(fd,&zero, sizeof(zero));
	ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 1);
	sem_unlink(SEM_NAME);
	setbuf(stdout, NULL);
	
	if (fork() == 0) {
		for (i = 0; i < nloop; i++) {
			sem_wait(mutex);
			printf("child:%d\n", (*ptr)++);
			sem_post(mutex);
		}	
		exit(0);
	}	 

	for (i = 0; i < nloop; i++) {
		sem_wait(mutex);
		printf("parent:%d\n", (*ptr)++);
		sem_post(mutex);
	}
	exit(0);
}	
