#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int pid;


void function7 () {

sigset_t oldset;
     sigprocmask(SIG_UNBLOCK, NULL, &oldset);
     for(int i = 1; i <= 31; i++){
            if(sigismember(&oldset, i)) {
                printf("Signal Included: %d \n", i);
            }
            else{
                printf("Signal Not Included: %d \n", i);
            }
     }

}


void signal_handler (int signum){

    if (signum == SIGUSR1){
    function7();
    }

}

int main(){

signal(SIGUSR1, signal_handler);
 pid = getpid();
    
    kill(pid, SIGUSR1);

}