#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>

int error(char *msg)
{
    perror(msg);
    sem_unlink("/semex051");
    sem_unlink("/semex052");
    return -1;
}

int main()
{
    sem_t *sem1 = sem_open("/semex051", O_CREAT | O_EXCL, 0644, 0);
    if (sem1 == SEM_FAILED)
        return error("ERROR: sem_open() (1)");
    sem_t *sem2 = sem_open("/semex052", O_CREAT | O_EXCL, 0644, 1);
    if (sem2 == SEM_FAILED)
        return error("ERROR: sem_open() (2)");

    pid_t p = fork();
    if (p < 0)
    {
        return error("ERROR: fork()");
    }
    else if (p > 0)
    {
        while (1)
        {
            sem_wait(sem1);

            printf("I'm the father.\n");
            usleep(500000);

            sem_post(sem2);
        }
    }
    else
    {
        while (1)
        {
            sem_wait(sem2);

            printf("I'm the child.\n");
            usleep(500000);

            sem_post(sem1);
        }
    }

    return 0;
}