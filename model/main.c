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
#define SHM "/shm"
#define SEM_N1 "/semproc"
#define SEM_N2 "/sembarrier"

typedef struct
{
    char votes[ARRAY_SIZE];
    short count;
} s_struct;

int error(char *msg)
{
    perror(msg);
    shm_unlink(SHM);
    sem_unlink(SEM_N1);
    sem_unlink(SEM_N2);
    return -1;
}

int main()
{
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    /// SHARED MEMORY
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

    /// SEMAPHORE
    sem_t *sem_proc = sem_open(SEM_N1, O_CREAT | O_EXCL, 0644, 1);
    if (sem_proc == SEM_FAILED)
    {
        return error("ERROR: sem_open() (1)");
    }

    sem_t *sem_barrier = sem_open(SEM_N2, O_CREAT | O_EXCL, 0644, 0);
    if (sem_barrier == SEM_FAILED)
    {
        return error("ERROR: sem_open() (2)");
    }
    /// END

    pid_t p[ARRAY_SIZE];

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        p[i] = fork();

        if (p[i] < 0)
        {
            return error("ERROR: fork()");
        }
        else if (p[i] == 0)
        {
            /// Barrier
            sem_wait(sem_barrier);
            sem_post(sem_barrier);
            /// End

            sem_wait(sem_proc);

            srand(time(0) * i);
            switch (rand() % 2)
            {
            case 0:
                data->votes[data->count++] = 'S';
                break;
            case 1:
                data->votes[data->count++] = 'N';
                break;
            }

            sem_post(sem_proc);

            munmap(data, data_size);
            close(fdm);

            exit(0);
        }
    }

    printf("Voting start 3 second delay...\n\n");
    sleep(3);
    sem_post(sem_barrier);
    printf("Waiting 5 seconds for votes...\n\n");
    sleep(5);
    printf("Voting has ended.\n\n");

    int num_s = 0, num_n = 0;

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        if (data->votes[i] == 'S')
        {
            num_s++;
        }
        else if (data->votes[i] == 'N')
        {
            num_n++;
        }
    }

    sleep(3);
    printf("Counting all the votes...\n\n");
    fflush(stdout);
    sleep(3);
    printf("And ");
    fflush(stdout);
    sleep(1);
    printf("the ");
    fflush(stdout);
    sleep(1);
    printf("results ");
    fflush(stdout);
    sleep(1);
    printf("are");
    fflush(stdout);

    for (int i = 0; i < 75; i++)
    {
        printf("e");
        fflush(stdout);
        usleep(50000);
    }

    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    printf("Okay.\n\n");
    fflush(stdout);
    sleep(2);
    printf("That's enough.\n\n");
    fflush(stdout);
    sleep(2);
    printf("Here they are...\n\n");
    fflush(stdout);
    sleep(2);

    printf("Results:\nS = %d votes\nN = %d votes\nBlank = %d votes\n\n", num_s, num_n, num_s + num_n - ARRAY_SIZE);

    munmap(data, data_size);
    shm_unlink(SHM);
    sem_unlink(SEM_N1);
    sem_unlink(SEM_N2);

    return 0;
}