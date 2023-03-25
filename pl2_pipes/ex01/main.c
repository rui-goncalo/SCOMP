#include <sys/wait.h> // wait
#include <unistd.h> // fork
#include <stdio.h> // printf
#include <stdlib.h> // exit
#include <string.h> // strcpy

int main ()
{
	int fd[2];
	pipe(fd);
	
	if(fork() == 0) {
		int pPid = 0;
		close(fd[1]);
		read(fd[0], &pPid, 100);
		printf("Parent PID: %d\n", pPid);
		close(fd[0]);
	}
	else {
		int pPid = getpid();
		close(fd[0]);
		printf("My PID: %d\n", pPid);
		write(fd[1], &pPid, sizeof(int));
		close(fd[1]);
	}
	return 0;
}