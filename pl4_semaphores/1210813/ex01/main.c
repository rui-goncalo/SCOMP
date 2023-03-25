#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>

#define BUFFER_SIZE 128
#define CHILD_COUNT 8
#define SEM_N "/semex01"

int error(char *msg)
{
    perror(msg);
    sem_unlink(SEM_N);
    return -1;
}

int main()
{
    sem_t *sem = sem_open(SEM_N, O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED)
    {
        return error("ERROR: sem_open()");
    }

    pid_t p[CHILD_COUNT];

    for (int i = 0; i < CHILD_COUNT; i++)
    {
        p[i] = fork();

        if (p[i] < 0)
        {
            return error("fork");
        }
        else if (p[i] == 0)
        {
            sem_wait(sem);

            FILE *fpr = fopen("numbers.txt", "r");
            FILE *fpw = fopen("output.txt", "a");

            if (fpr && fpw)
            {
                char str[BUFFER_SIZE];
                fread(str, 1, BUFFER_SIZE, fpr);

                for (int j = i * 8; j < (i + 1) * 8; j++)
                {
                    fwrite(&str[j], 1, 1, fpw);
                }

                // The last child adds the EOF character at the end
                if (i == CHILD_COUNT - 1)
                {
                    char eof = '\0';
                    fwrite(&eof, 1, 1, fpw);
                }
            }

            sem_post(sem);
            exit(0);
        }
    }

    // Wait for all child processes to end
    for (int i = 0; i < CHILD_COUNT; i++)
    {
        wait(NULL);
    }

    // Print the contents of output.txt
    FILE *fp = fopen("output.txt", "r");
    if (fp)
    {
        char str[BUFFER_SIZE];
        fread(str, 1, BUFFER_SIZE, fp);

        printf("\"%s\"\n", str);
    }

    remove("output.txt");
    sem_unlink(SEM_N);

    return 0;
}