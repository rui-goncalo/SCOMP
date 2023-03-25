#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>

#define ARRAY_SIZE 50
#define CHILD_COUNT 5
#define SHM_N "/shmex03"
#define SEM_N "/semex03"

typedef struct
{
    char str[ARRAY_SIZE][80];
    int count;
} s_struct;

int error(char *msg)
{
    perror(msg);
    shm_unlink(SHM_N);
    sem_unlink(SEM_N);
    return -1;
}

int main()
{
    /// Shared Memory
    int data_size = sizeof(s_struct);

    int fdm = shm_open(SHM_N, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
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
    /// END

    /// Semaphore
    sem_t *sem = sem_open(SEM_N, O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED)
    {
        return error("ERROR: sem_open()");
    }
    /// END

    printf("Processing...\n");

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
            /// Build string
            char num[12];
            snprintf(num, 12, "%d", getpid());
            char str[48] = "I'm the Father - with PID ";
            strcat(str, num);
            /// END

            while (data->count < ARRAY_SIZE)
            {
                sem_wait(sem);

                // Prevents the waiting processes from writing outside the array
                if (data->count < ARRAY_SIZE)
                {
                    strcpy(data->str[data->count++], str);

                    // Sleep for 1/20 of a second
                    usleep(50000);
                }

                sem_post(sem);
            }

            // Close shared memory
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

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        printf("\"%s\"\n", data->str[i]);
    }

    // Remove both shared memory and semaphore
    munmap(data, data_size);
    shm_unlink(SHM_N);
    sem_unlink(SEM_N);

    return 0;
}