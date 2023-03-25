#include <sys/wait.h> // wait
#include <unistd.h>   // fork
#include <stdio.h>    // printf
#include <stdlib.h>   // exit
#include <time.h>     // time

int main()
{
    int vec1[1000];
    int vec2[1000];
    int res[1000];
    int tab[5][2];

    pid_t p[5];
    time_t t;
    srand ((unsigned) time (&t));

    int count = 0;
    int count2 = 0;
    int status = 0;
    int i= 0;

    // fill entire table
    for (i = 0; i < 1000; i++)
    {
        vec1[i] = rand() % 100;
        vec2[i] = rand() % 100;
    }

    for (i = 0; i < 5; i++)
    {

        pipe(tab[i]);
        p[i] = fork();

        if (p[i] < 0)
        {
            printf("Descriptor ERROR!");
            exit(1);
        }

        if (p[i] == 0)
        { // child process

            int start = 200 * i;
            int end = (i + 1) * 200;

            for (int y = start; y < end; y++)
            {
                count = vec1[y] + vec2[y];
                write(tab[i][1], &count, sizeof(int));
            }
            close(tab[i][1]);
            exit(1);
        }
        else
        {
            wait(&status); 
            close(tab[i][1]);
            int start = 200 * i;
            int end = (i + 1) * 200;

            for (int y = start; y < end; y++)
            {
                read(tab[i][0], &count2, sizeof(int));
                res[y] = count2;
                printf("Array's size: %d\n", res[y]);
            }
            close(tab[i]);
        }
    }

    return 0;
}