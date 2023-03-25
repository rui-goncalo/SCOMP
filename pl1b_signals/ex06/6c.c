#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int USR1_COUNTER = 0; 

int sigaction (int sig, const struct sigaction * act, struct sigaction * oact);

void handle_signal(int signo, siginfo_t *sinfo, void *context)
{
    if (signo == SIGUSR1)
    {
        USR1_COUNTER++;
        printf("SIGUSR1 signal captured, USR1_COUNTER = %d\n", USR1_COUNTER);
    }
}

int main(int argc) {
    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));

    sigfillset(&act.sa_mask); //bloqueia todos os sinais

    act.sa_sigaction = handle_signal;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    
    while (1) {
        printf("I'm working!");
        fflush(stdout);
        sleep(1);
    }
}