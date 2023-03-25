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

int error(char *msg)
{
    perror(msg);
    sem_unlink("/ex021");
    sem_unlink("/ex022");
    sem_unlink("/ex023");
    sem_unlink("/ex024");
    sem_unlink("/ex025");
    sem_unlink("/ex026");
    sem_unlink("/ex027");
    sem_unlink("/ex028");
    return -1;
}

int main()
{
    sem_t *sem1 = sem_open("/ex021", O_CREAT | O_EXCL, 0644, 0);
    if (sem1 == SEM_FAILED)
        return error("ERROR: sem_open()");
    sem_t *sem2 = sem_open("/ex022", O_CREAT | O_EXCL, 0644, 0);
    if (sem2 == SEM_FAILED)
        return error("ERROR: sem_open()");
    sem_t *sem3 = sem_open("/ex023", O_CREAT | O_EXCL, 0644, 0);
    if (sem3 == SEM_FAILED)
        return error("ERROR: sem_open()");
    sem_t *sem4 = sem_open("/ex024", O_CREAT | O_EXCL, 0644, 0);
    if (sem4 == SEM_FAILED)
        return error("ERROR: sem_open()");
    sem_t *sem5 = sem_open("/ex025", O_CREAT | O_EXCL, 0644, 0);
    if (sem5 == SEM_FAILED)
        return error("ERROR: sem_open()");
    sem_t *sem6 = sem_open("/ex026", O_CREAT | O_EXCL, 0644, 0);
    if (sem6 == SEM_FAILED)
        return error("ERROR: sem_open()");
    sem_t *sem7 = sem_open("/ex027", O_CREAT | O_EXCL, 0644, 0);
    if (sem7 == SEM_FAILED)
        return error("ERROR: sem_open()");
    sem_t *sem8 = sem_open("/ex028", O_CREAT | O_EXCL, 0644, 0);
    if (sem8 == SEM_FAILED)
        return error("ERROR: sem_open()");

    pid_t p[CHILD_COUNT];

    for (int i = CHILD_COUNT - 1; i >= 0; i--)
    {
        p[i] = fork();

        if (p[i] < 0)
        {
            return error("fork");
        }
        else if (p[i] == 0)
        {
            switch (i)
            {
            case 0:
                break;
            case 1:
                sem_wait(sem1);
                break;
            case 2:
                sem_wait(sem2);
                break;
            case 3:
                sem_wait(sem3);
                break;
            case 4:
                sem_wait(sem4);
                break;
            case 5:
                sem_wait(sem5);
                break;
            case 6:
                sem_wait(sem6);
                break;
            case 7:
                sem_wait(sem7);
                break;
            }

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

            switch (i)
            {
            case 0:
                sem_post(sem1);
                break;
            case 1:
                sem_post(sem2);
                break;
            case 2:
                sem_post(sem3);
                break;
            case 3:
                sem_post(sem4);
                break;
            case 4:
                sem_post(sem5);
                break;
            case 5:
                sem_post(sem6);
                break;
            case 6:
                sem_post(sem7);
                break;
            case 7:
                sem_post(sem8);
                break;
            }

            printf("Child %d ended.\n", i);
            exit(0);
        }
    }

    // Wait for the last child process to end
    sem_wait(sem8);

    // Print the contents of output.txt
    FILE *fp = fopen("output.txt", "r");
    if (fp)
    {
        char str[BUFFER_SIZE];
        fread(str, 1, BUFFER_SIZE, fp);

        printf("\"%s\"\n", str);
    }

    remove("output.txt");
    sem_unlink("/ex021");
    sem_unlink("/ex022");
    sem_unlink("/ex023");
    sem_unlink("/ex024");
    sem_unlink("/ex025");
    sem_unlink("/ex026");
    sem_unlink("/ex027");
    sem_unlink("/ex028");

    return 0;
}