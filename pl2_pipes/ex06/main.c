#include <sys/wait.h> // wait
#include <unistd.h> // fork
#include <stdio.h> // printf
#include <stdlib.h> // exit

//ex incomplete

#define MAX_SIZE 256

int main ()
{
	char str[MAX_SIZE];
	int fd1[2];
	int fd2[2];
	pipe(fd1);
	pipe(fd2);
	
	pid_t p = fork();
	
	if(p > 0) {
		close(fd1[1]);
		close(fd2[0]);
		
		read(fd1[0], &str, MAX_SIZE);
		
		for(int i = 0; i < MAX_SIZE && str[i] != '\0'; i++) {
			str[i] -= 32; 
		}
		
		write(fd2[1], &str, MAX_SIZE);
		
		close(fd1[0]);
		close(fd2[1]);
	}
	else {
		close(fd1[0]);
		close(fd2[1]);
		
		printf("Text: ");
		scanf("%s", str);
		
		printf("Before: \"%s\"\n", str);
		write(fd1[1], &str, MAX_SIZE);
		
		read(fd2[0], &str, MAX_SIZE);
		printf("After: \"%s\"\n", str);
		
		close(fd1[1]);
		close(fd2[0]);
	}
	return 0;
}