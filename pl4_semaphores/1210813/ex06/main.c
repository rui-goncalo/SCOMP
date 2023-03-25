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
    sem_unlink("/semex061");
    sem_unlink("/semex062");
    return -1;
}

int main()
{
    sem_t *sem1 = sem_open("/semex061", O_CREAT | O_EXCL, 0644, 0);
    if (sem1 == SEM_FAILED)
        return error("ERROR: sem_open() (1)");
    sem_t *sem2 = sem_open("/semex062", O_CREAT | O_EXCL, 0644, 1);
    if (sem2 == SEM_FAILED)
        return error("ERROR: sem_open() (2)");

    int count = 0;

    pid_t p = fork();
    if (p < 0)
    {
        return error("ERROR: fork()");
    }
    else if (p > 0)
    {
        while (count < 15)
        {
            sem_wait(sem1);

            printf("I'm the father. (%d)\n", count++);
            usleep(200000);

            sem_post(sem2);
        }
    }
    else
    {
        while (count < 15)
        {
            sem_wait(sem2);

            printf("I'm the child. (%d)\n", count++);
            usleep(200000);

            sem_post(sem1);
        }
    }

    return 0;
}