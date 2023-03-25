#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

sig_atomic_t can_executetaskC = 0;

void executetaskC() {
    can_executetaskC = 1;
}

int main (){
	srand (time(NULL));
	
	struct sigaction sa;
	sa.sa_handler = &executetaskC;
	sigaction(SIGUSR1,&sa,NULL);

    pid_t pid;
    if((pid = fork()) < 0){
        perror("fork error");
        exit(1);
    }

    if (pid == 0){
        int time = (rand() % (5 - 1 + 1)) + 1;
        sleep(time);
        printf("Done Executing task B - %d seconds\n", time);
        for (;;){
            if(can_executetaskC){
                    break;
            }
        }
        printf("Executing task C\n");
        exit(0);
        
    }
    else {
        sleep(3);
        printf("Done Executing task A\n");
        kill(pid, SIGUSR1);
        int returnStatus;
        waitpid(pid, &returnStatus, 0);
    }

}
