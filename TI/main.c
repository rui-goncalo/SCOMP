#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>

#define CHILD_COUNT 3  // Number of child processes to create
#define ARRAY_SIZE 100 // Size of the array of measurements
#define CYCLE_COUNT 60 // Number of times to repeat the cycle (If A1 sleeps for a second in each cycle, 60 * 1 = 60 seconds, therefore, 1 minute of execution)
#define SHM "/shmti"   // Name for the shared memory
#define SEM "/semti"   // Name for the semaphore

typedef struct
{
    int measurements[ARRAY_SIZE];
    int sync_flag;
} ss_struct;

typedef struct
{
    ss_struct processes[CHILD_COUNT];
} s_struct;

// Error function. Gets called when there's a problem forking processes or when creating either the shared memory or the semaphore.
int error(char *msg)
{
    perror(msg);
    shm_unlink(SHM);
    sem_unlink(SEM);
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
    sem_t *sem = sem_open(SEM, O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED)
    {
        return error("ERROR: sem_open()");
    }
    /// End

    pid_t a1[CHILD_COUNT];

    srand(time(NULL));

    for (int i = 0; i < CHILD_COUNT; i++)
    {
        a1[i] = fork();

        if (a1[i] < 0)
        {
            return error("ERROR: fork()");
        }
        else if (a1[i] == 0)
        {
            for (int j = 0; j < CYCLE_COUNT; j++)
            {
                // Waits for A3 processes to finish
                while (data->processes[i].sync_flag != 0)
                    ;

                sem_wait(sem);

                // Generates an array of 100 random numbers between 1-100
                for (int k = 0; k < ARRAY_SIZE; k++)
                {
                    data->processes[i].measurements[k] = ((rand() + i * k) % 100) + 1;
                    printf("%d. %d ### %d\n", i, data->processes[i].measurements[k], j);
                }

                // Updates the flag to let A2 processes operate
                data->processes[i].sync_flag = 1;

                sem_post(sem);
                sleep(1);
            }

            // Disconnects the shared memory
            munmap(data, data_size);
            // Closes file descriptor for the shared memory
            close(fdm);

            exit(0);
        }
    }

    pid_t a2[CHILD_COUNT];

    for (int i = 0; i < CHILD_COUNT; i++)
    {
        a2[i] = fork();

        if (a2[i] < 0)
        {
            return error("ERROR: fork()");
        }
        else if (a2[i] == 0)
        {
            for (int j = 0; j < CYCLE_COUNT; j++)
            {
                // Waits for A1 processes to finish
                while (data->processes[i].sync_flag != 1)
                    ;

                sem_wait(sem);

                // Adds 10 to each number in the array of 100 numbers
                for (int k = 0; k < ARRAY_SIZE; k++)
                {
                    data->processes[i].measurements[k] += 10;
                }

                // Updates the flag to let A3 processes operate
                data->processes[i].sync_flag = 2;

                sem_post(sem);
                sleep((rand() % 3) + 1);
            }

            // Disconnects the shared memory
            munmap(data, data_size);
            // Closes file descriptor for the shared memory
            close(fdm);

            exit(0);
        }
    }

    pid_t a3[CHILD_COUNT];

    int fd[2];
    pipe(fd);

    for (int i = 0; i < CHILD_COUNT; i++)
    {
        a3[i] = fork();

        if (a3[i] < 0)
        {
            return error("ERROR: fork()");
        }
        else if (a3[i] == 0)
        {
            for (int j = 0; j < CYCLE_COUNT; j++)
            {
                // Closes the reading "side" of the pipe
                close(fd[0]);

                // Waits for A2 processes to finish
                while (data->processes[i].sync_flag != 2)
                    ;

                sem_wait(sem);

                // Adds 100 to each number in the array of 100 numbers
                for (int k = 0; k < ARRAY_SIZE; k++)
                {
                    data->processes[i].measurements[k] += 100;
                }

                // Sends the final vector to the "printer" process through a pipe
                write(fd[1], &data->processes[i].measurements, sizeof(data->processes[i].measurements));

                // Updates the flag to let A1 processes operate
                data->processes[i].sync_flag = 0;

                sem_post(sem);
                sleep(rand() % 3);
            }

            // Disconnects the shared memory
            munmap(data, data_size);
            // Closes file descriptor for the shared memory
            close(fdm);
            // Closes the writing "side" of the pipe
            close(fd[1]);

            exit(0);
        }
    }

    // Closes the writing "side" of the pipe
    close(fd[1]);

    int measurements[ARRAY_SIZE];
    for (int i = 0; i < CHILD_COUNT * CYCLE_COUNT; i++)
    {
        // Reads the incoming array from the pipe
        read(fd[0], &measurements, sizeof(measurements));

        // Prints the entire array in the following format: "Measurements[CHILD NUMBER][MEASUREMENT NUMBER]"
        for (int j = 0; j < ARRAY_SIZE; j++)
        {
            printf("Measurements[%d][%d] = %d\n", i, j, measurements[j]);
        }
    }

    // Closes the reading "side" of the pipe
    close(fd[0]);

    // Wait for all child processes to end
    for (int i = 0; i < CHILD_COUNT * 3; i++)
    {
        wait(NULL);
    }

    // Disconnects the shared memory
    munmap(data, data_size);
    // Removes the shared memory from the file system
    shm_unlink(SHM);
    // Removes the semaphore
    sem_unlink(SEM);

    return 0;
}