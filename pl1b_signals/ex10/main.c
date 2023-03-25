#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int sigaction (int sig, const struct sigaction * act, struct sigaction * oact);


void handle_ALARM(int signo) {
    int p = getpid();
    char str_m[100] = "To slow, that is why the program will end!\n";
    write(STDOUT_FILENO, str_m, strlen(str_m));
    kill(p, SIGINT);
}

int main () {
    char str_in[100];
    int count = 0;
    struct sigaction act;

    /*Limpar a variável act*/
    memset(&act, 0, sizeof(struct sigaction));

    /*Para não bloquear sinais*/
    sigemptyset(&act.sa_mask); 

    act.sa_handler = handle_ALARM;
    sigaction(SIGALRM, &act, NULL);

    alarm(10);
    char str[100] = "Write the string: \n";
    write(STDOUT_FILENO, str, strlen(str));
    fgets(str_in, 100, stdin);

    for (int i = 0; i < strlen(str_in); i++) {
        if (str_in[i] != 10 && str_in[i] != 32) {
            count++;
        }
    }

    //Para que o alarm(20) não interrompa
    alarm(0);

    printf("The number of characters: %d\n", count);
    sleep(20);
    printf("Successful execution!\n");
}