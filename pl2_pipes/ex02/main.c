#include <sys/wait.h> // wait
#include <unistd.h> // fork
#include <stdio.h> // printf
#include <stdlib.h> // exit
#include <string.h> // strcpy

typedef struct {
	int x;
	char str[48];
} s_payload;

int main ()
{
	int fd[2];
	pipe(fd);
	
	if(fork() != 0) {
		s_payload payload;
		payload.x = 88;
		strcpy(payload.str, "Ok.");
		
		close(fd[0]);
		write(fd[1], &payload, sizeof(s_payload));
		close(fd[1]);
	}
	else {
		s_payload payload;
		
		close(fd[1]);
		read(fd[0], &payload, sizeof(s_payload));
		close(fd[0]);
		
		printf("Num = %d | Str = \"%s\"\n", payload.x, payload.str);
	}
	return 0;
}