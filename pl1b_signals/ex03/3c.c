#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void printMsg()
{
    write(STDOUT_FILENO, "SIGUSR1 has been captured and was sent by the process with PID XX", 128);
}

int main()
{
    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));  // Fills all positions with 0.

    sigemptyset(&act.sa_mask);  // Excludes all the recognized signals.

    act.sa_handler = printMsg;  // Defines the function to be executed.
    sigaction(SIGUSR1, &act, NULL); // Defines the signal with the personalized behaviour.

    pid_t p = fork();

    if (p > 0)
    {
        kill(p, SIGUSR1);
    }
}