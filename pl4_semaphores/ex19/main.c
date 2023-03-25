#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>

#define CHILD_COUNT 5
#define MEASUREMENT_COUNT 6
#define SHM "/shmex19"
#define SEM_1 "/semex191"
#define SEM_2 "/semex192"

typedef struct
{
    int measurements[MEASUREMENT_COUNT];
    int measurements_count;
    char update_flag;
} ss_struct;

typedef struct
{
    ss_struct measurements[CHILD_COUNT];
    int alarm_count;
    char update_flag;
} s_struct;

int error(char *msg)
{
    perror(msg);
    shm_unlink(SHM);
    sem_unlink(SEM_1);
    sem_unlink(SEM_2);
    return -1;
}

int main()
{
    /// Shared Memory
    int data_size = sizeof(s_struct);

    int fdm = shm_open(SHM, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
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
    sem_t *sem = sem_open(SEM_1, O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED)
    {
        return error("ERROR: sem_open()");
    }

    sem_t *sem_barrier = sem_open(SEM_2, O_CREAT | O_EXCL, 0644, 0);
    if (sem == SEM_FAILED)
    {
        return error("ERROR: sem_open()");
    }
    /// End

    pid_t p[CHILD_COUNT];

    srand(time(NULL));

    for (int i = 0; i < CHILD_COUNT; i++)
    {
        p[i] = fork();

        if (p[i] < 0)
        {
            return error("ERROR: fork()");
        }
        else if (p[i] == 0)
        {
            sem_wait(sem_barrier);
            sem_post(sem_barrier);
            
            for (int j = 0; j < MEASUREMENT_COUNT; j++)
            {
                sem_wait(sem);

                data->measurements[i].measurements[data->measurements[i].measurements_count] = (rand() + i) % 101;
                if (data->measurements[i].measurements[data->measurements[i].measurements_count] > 50)
                {
                    data->alarm_count++;
                }

                data->measurements[i].measurements_count++;
                data->measurements[i].update_flag = 1;

                sem_post(sem);

                sleep(rand() % 4);
            }

            munmap(data, data_size);
            close(fdm);

            exit(0);
        }
    }

    sem_post(sem_barrier);

    int total_measurements;

    do
    {
        total_measurements = 0;

        for (int i = 0; i < CHILD_COUNT; i++)
        {
            total_measurements += data->measurements[i].measurements_count;

            if (data->measurements[i].update_flag)
            {
                printf("Sensor %d reading %d: %d\n", i, data->measurements[i].measurements_count, data->measurements[i].measurements[data->measurements[i].measurements_count-1]);
                data->measurements[i].update_flag = 0;
            }
        }

    } while (total_measurements != CHILD_COUNT * MEASUREMENT_COUNT);

    printf("\nTotal alarm count: %d\n", data->alarm_count);

    // Wait for all child processes to end
    for (int i = 0; i < CHILD_COUNT; i++)
    {
        wait(NULL);
    }

    munmap(data, data_size);
    shm_unlink(SHM);
    sem_unlink(SEM_1);
    sem_unlink(SEM_2);

    return 0;
}