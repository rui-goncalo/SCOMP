#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>

#define CHILD_COUNT 3

int error(char *msg)
{
    perror(msg);
    sem_unlink("/semex071");
    sem_unlink("/semex072");
    sem_unlink("/semex073");
    return -1;
}

int main()
{
    sem_t *sem1 = sem_open("/semex071", O_CREAT | O_EXCL, 0644, 0);
    if (sem1 == SEM_FAILED)
        return error("ERROR: sem_open() (1)");
    sem_t *sem2 = sem_open("/semex072", O_CREAT | O_EXCL, 0644, 0);
    if (sem2 == SEM_FAILED)
        return error("ERROR: sem_open() (2)");
    sem_t *sem3 = sem_open("/semex073", O_CREAT | O_EXCL, 0644, 0);
    if (sem3 == SEM_FAILED)
        return error("ERROR: sem_open() (3)");

    pid_t p[CHILD_COUNT];
    for (int i = 0; i < CHILD_COUNT; i++)
    {
        p[i] = fork();

        if (p[i] < 0)
        {
            return error("ERROR: fork()");
        }
        else if (p[i] == 0)
        {
            switch (i)
            {
            case 0:
                printf("Sistemas ");
                fflush(stdout);
                sem_post(sem1);
                sem_wait(sem3);
                printf("a ");
                fflush(stdout);
                sem_post(sem1);
                break;
            case 1:
                sem_wait(sem1);
                printf("de ");
                fflush(stdout);
                sem_post(sem2);
                sem_wait(sem1);
                printf("melhor ");
                fflush(stdout);
                sem_post(sem2);
                break;
            case 2:
                sem_wait(sem2);
                printf("Computadores, ");
                fflush(stdout);
                sem_post(sem3);
                sem_wait(sem2);
                printf("Disciplina!\n");
                fflush(stdout);
                sem_post(sem3);
                break;
            }

            exit(0);
        }
    }

    // Wait for all child processes to end
    for (int i = 0; i < CHILD_COUNT; i++)
    {
        wait(NULL);
    }

    sem_unlink("/semex071");
    sem_unlink("/semex072");
    sem_unlink("/semex073");

    return 0;
}