#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int USR1_COUNTER = 0;

int sigaction (int sig, const struct sigaction * act, struct sigaction * oact);

void handle_USR1 (int signo, siginfo_t *sinfo, void *context) {
    USR1_COUNTER++;
    sleep(2);
    /*Forma não segura de obter output*/
    printf("SIGUSR1 signal captured, USR1_COUNTER = %d\n", USR1_COUNTER);
}

int main (int argc, char *argv[]) {
    struct sigaction act;
    struct timespec ts;

    /*Limpar a variável act*/
    memset(&act, 0, sizeof(struct sigaction));

    /*Para não bloquear sinais*/
    sigemptyset(&act.sa_mask); 

    act.sa_sigaction = handle_USR1;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    /*nanosleep - 10ms*/
    ts.tv_sec = 0;
    ts.tv_nsec = 10000000;

    pid_t p;
    p = fork();
    
    if (p < 0) {
      perror("Fork failed.\n");
      exit(-1);
    }

    if (p == 0) {
        int parent_pid = (int) getppid();
        for (int i = 1; i < 13; i++) {
            printf("Signal %d\n", i);
            fflush(stdout);
        }
        nanosleep(&ts, NULL);
    }

    while (1) {
        printf("I'm working!");
        fflush(stdout);
        sleep(1);
    }
}