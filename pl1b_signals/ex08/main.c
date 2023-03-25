#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

int sigaction (int sig, const struct sigaction * act, struct sigaction * oact);
int child_p = 5;

void handle_USR1 (int signo, siginfo_t *sinfo, void *context) {
    child_p --;
    printf("\nChild Process Terminated.\n");
}

int main() {
    struct sigaction act;

    /*Limpar a variável act*/
    memset(&act, 0, sizeof(struct sigaction));

    /*Para bloquear todos os sinais*/
    sigfillset(&act.sa_mask); 

    act.sa_sigaction = handle_USR1;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    
    pid_t p;
    int res = 0;

    for (int i = 0; i < 5; i++) {
        p = fork();
        if (p == 0) {
            int j = i;
            for (j = i*200; j < (i+5)*200; j++) {
                printf("%i\n", j);
            }
            kill(getppid(), 10); //Sinal SIGUSR1
            exit(i);
        }
    }

    while (child_p != 0) {
        pause();
    }
    
    wait(0);
}