#include <stdio.h> // printf
#include <unistd.h> // fork

#define BUFFER_SIZE 128

int main ()
{
	char str[BUFFER_SIZE];
	int fd[2];
	pipe(fd);
	
	if(fork() > 0) {
		close(fd[0]);
		
		FILE* fp = fopen("file.txt", "r");
		if(fp) {
			fread(str, 1, BUFFER_SIZE, fp);
			sleep(2);
			write(fd[1], &str, BUFFER_SIZE);
		}
		
		close(fd[1]);
	}
	else {
		close(fd[1]);
		
		read(fd[0], &str, BUFFER_SIZE);
		printf("\"%s\"\n\n", str);
		
		close(fd[0]);
	}
	return 0;
}
