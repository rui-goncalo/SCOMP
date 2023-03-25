#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main()
{
    pid_t p = fork();

    if (p > 0)
    {
        sleep(1);
        kill(p, SIGSTOP);
        write(STDOUT_FILENO, "\nSTOP\n", 6);
        kill(p, SIGCONT);
        write(STDOUT_FILENO, "\nCONT\n", 6);
        sleep(1);
        kill(p, SIGKILL);
        write(STDOUT_FILENO, "\nFIM\n", 5);
    }
    else
    {
        for (;;)
        {
            // printf("I Love SCOMP!\n");
            write(STDOUT_FILENO, "I Love SCOMP!\n", 15);
        }
    }
}