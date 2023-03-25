#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>

#define CHILD_COUNT 10
#define SM "/shmex09"

typedef struct
{
    int count;
} s_struct;

int error(char *msg)
{
    perror(msg);
    shm_unlink(SM);
    sem_unlink("/semex091");
    sem_unlink("/semex092");
    return -1;
}

int main()
{
    int data_size = sizeof(s_struct);

    int fdm = shm_open(SM, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fdm == -1)
    {
        return error("ERROR: shm_open()");
    }

    if (ftruncate(fdm, data_size) == -1)
    {
        return error("ERROR: ftruncate()");
    }

    s_struct *data = (s_struct *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdm, 0);
    if (data == NULL)
    {
        return error("ERROR: mmap()");
    }

    sem_t *sem = sem_open("/semex091", O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED)
        return error("ERROR: sem_open() (1)");
    sem_t *sem_barrier = sem_open("/semex092", O_CREAT | O_EXCL, 0644, 0);
    if (sem_barrier == SEM_FAILED)
        return error("ERROR: sem_open() (2)");

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
            // Sleep between 1-3 seconds
            srand(time(0));
            sleep(((rand() * (i + 1)) % 3) + 1);

            srand(time(0));
            if ((rand() * (i + 1)) % 2 == 0)
            {
                printf("Bought chips...\n");
            }
            else
            {
                printf("Bought beer.\n");
            }

            // Barrier
            sem_wait(sem);
            data->count++;
            sem_post(sem);

            if (data->count == CHILD_COUNT)
                sem_post(sem_barrier);

            sem_wait(sem_barrier);
            sem_post(sem_barrier);
            // END

            printf("Party!\n");

            munmap(data, data_size);
            close(fdm);

            exit(0);
        }
    }

    // Wait for all child processes to end
    for (int i = 0; i < CHILD_COUNT; i++)
    {
        wait(NULL);
    }

    munmap(data, data_size);
    shm_unlink(SM);
    sem_unlink("/semex091");
    sem_unlink("/semex092");

    return 0;
}