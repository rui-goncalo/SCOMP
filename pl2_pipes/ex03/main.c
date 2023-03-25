#include <sys/wait.h> // wait
#include <unistd.h> // fork
#include <stdio.h> // printf
#include <stdlib.h> // exit
#include <string.h> // strcpy

int main ()
{
	int status;
	int fd[2];
	pipe(fd);
	
	pid_t p = fork();
	
	if(p > 0) {	
		char str[100];
			
		close(fd[0]);
		strcpy(str, "Hello world");
		write(fd[1], &str, 20);
		strcpy(str, "Goodbye");
		write(fd[1], &str, 20);
		close(fd[1]);
		
		p = wait(&status);
		
		wait(&status);
		if (WIFEXITED(status)) {
			printf("Child PID: %d | Child exit value: %d\n", p, WEXITSTATUS(status));
		}
	}
	else {
		char str[100];
		
		close(fd[1]);
		read(fd[0], &str, 20);
		printf("\"%s\"\n\n", str);
		read(fd[0], &str, 20);
		printf("\"%s\"\n\n", str);
		close(fd[0]);
		
		exit(88);
	}
	return 0;
}