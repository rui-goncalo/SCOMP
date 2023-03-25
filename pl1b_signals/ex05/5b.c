#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


void printMsg(int signo) {
    if (signo == SIGINT) {
        write(STDOUT_FILENO, "I won't let the process end with CTRL-C!", 40);
    }
}

int main()
{
    signal(SIGINT, &printMsg);
    for (;;) {
        printf("I Like Signal\n");
        sleep(1);
    }
}