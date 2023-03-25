#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

/*
    1. Each child process reads 200 integer numbers from a
    text file named ”Numbers.txt”, and writes those numbers
    and its PID to the file named ”Output.txt”. For example,
    the process with PID 16734 writing number 12 should output
    something like: [16734] 12

    2. At any time instant only one process should be allowed
    to read from file ”Numbers.txt”.

    3. At any time only one process should be allowed to write
    to file ”Output.txt”.

    4. One process reading from ”Numbers.txt” must not prevent
    any other process to write to ”Output.txt”.

    5. At the end, the father process should print the content
    of the file ”Output.txt”.
*/
#define CHILD_COUNT 8
#define BUFFER_SIZE 256

int main()
{
    // o último valor é 1 pq o semáforo inicia a 1
    sem_t *s = sem_open("/ex01", O_CREAT | O_EXCL, 0644, 1);

    if (s == SEM_FAILED)
    {
        perror("sem_open() ERRROR");
        exit(0);
    }

    pid_t p[CHILD_COUNT];

    for (int i = 0; i < CHILD_COUNT; i++)
    {
        p[i] = fork();

        if (p[i] < 0)
        {
            perror("ERROR FORK\n");
        }
        else if (p[i] == 0)
        {
            sem_wait(s); // mete semáforo a zero

            FILE *open = fopen("numbers.txt", "r");
            FILE *write = fopen("output.txt", "a");

            if (open && write)
            {
                char str[BUFFER_SIZE];
                fread(str, 1, BUFFER_SIZE, open);

                for (int j = i * 8; j < (i + 1) * 8; j++)
                {
                    fwrite(&str[j], 1, 1, write);
                }
                if (i == CHILD_COUNT - 1)
                {
                    //char eof = '\0';
                    //fwrite(&eof, 1, 1, write);
                }
            }
            sem_post(s);
            exit(0);
        }
    }

    for (int i = 0; i < CHILD_COUNT; i++)
    {
        wait(NULL);
    }

    FILE *read = fopen("output.txt", "r");

    if (read)
    {
        char str[BUFFER_SIZE];
        fread(str, 1, BUFFER_SIZE, read);
        printf("%s\n", str);
    }

    sem_unlink("/ex01"); 
    remove("output.txt");
    return 0;
}