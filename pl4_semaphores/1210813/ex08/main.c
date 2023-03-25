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
    sem_unlink("/semex081");
    sem_unlink("/semex082");
    return -1;
}

int main()
{
    sem_t *sem1 = sem_open("/semex081", O_CREAT | O_EXCL, 0644, 1);
    if (sem1 == SEM_FAILED)
        return error("ERROR: sem_open() (1)");
    sem_t *sem2 = sem_open("/semex082", O_CREAT | O_EXCL, 0644, 1);
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
            printf("S");
            fflush(stdout);
            usleep(500000);
            sem_post(sem1);
            sem_wait(sem2);
            printf("S");
            fflush(stdout);
            usleep(500000);
            sem_post(sem2);
        }
    }
    else
    {
        while (1)
        {
            sem_wait(sem2);
            printf("C");
            fflush(stdout);
            usleep(500000);
            sem_post(sem2);
            sem_wait(sem1);
            printf("C");
            fflush(stdout);
            usleep(500000);
            sem_post(sem1);
        }
    }

    return 0;
}