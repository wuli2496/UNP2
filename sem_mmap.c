#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define SEM_NAME "/wuli"

int main(int argc, char **argv)
{

    int i, fd, nloop, zero = 0;
    int *ptr;
    sem_t *mutex;
    pid_t pid;
    int status;
    
    if (argc != 3) {
        fprintf(stderr, "usage: exe  <pathname> <nloop>\n");
        exit(1);
    }
    
    nloop = atoi(argv[2]);
    fd = open(argv[1], O_RDWR | O_CREAT, 0666);
    write(fd, &zero, sizeof(zero));
    ptr = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (ptr == MAP_FAILED) {
        exit(1);
    }
    
    mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 1);
    if (mutex == SEM_FAILED) {
        exit(1);
    }
    
    if (sem_unlink(SEM_NAME) == -1) {
        exit(1);
    }
    
    setbuf(stdout, NULL);

#if 1   
    if ((pid = fork()) == 0) {
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
    
    wait(&status);
#endif
    
    
    exit(0);
}
