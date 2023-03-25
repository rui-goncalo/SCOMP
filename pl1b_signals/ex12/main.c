#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int sigaction(int sig, const struct sigaction * act, struct sigaction * oact);

void handle_SIGUSR1(int signo, siginfo_t *sinfo, void *context) {
    char *str = "SIGUSR1 Success!";
    write(STDOUT_FILENO,str,strlen(str));
}

void handle_SIGUSR2(int signo, siginfo_t *sinfo, void *context) {
    char *str = "SIGUSR2 Success!";
    write(STDOUT_FILENO,str,strlen(str));
}

int simula1();
int simula2();

int main() {
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigfillset(&act.sa_mask);

    act.sa_sigaction = handle_SIGUSR1;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    int i, p;
    for (i = 0; i < 50; i++) {
        p = fork();

        if (p == 0) {
            int s1 = simula1();
            int ppid = getppid();
            sleep(2);
            if (s1 == 0) {
                kill(ppid, SIGUSR1);
            } else if (s1 == 1) {
                kill(ppid, SIGUSR2);
            } else {
                printf("Father (:");
            }

            int s2 = simula2();
            if (s2 == 0) {
                kill(ppid, SIGUSR1);
            } else {
                kill(ppid, SIGUSR2);
            }
            exit(1);
        }
    }
}

int simula1() {
    int i, x;
    printf("Simula1()");
    for (i = 1; i <= 10; i++) {
        x = rand() % 100 + 1;
        return 0;
    }
    return 1;
}

int simula2() {
    int i, x;
    printf("Simula2()");
    for (i = 1; i <= 10; i++) {
        x = rand() % 100 + 1;
        return 0;
    }
    return 1;
}