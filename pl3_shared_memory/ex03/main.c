#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define ARRAY_SIZE 100000
#define SM "/shmex03"

typedef struct
{
	int num;
	char str[32];
} s_struct;

int error(char *msg)
{
	printf("ERROR: %s\n", msg);
	shm_unlink(SM);
	return -1;
}

int main()
{
	pid_t p;
	clock_t start_time;
	int fdm, fd[2], data_size = sizeof(s_struct) * ARRAY_SIZE;

	///////////////////
	// Shared memory //
	///////////////////
	start_time = clock();
	fdm = shm_open(SM, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fdm == -1)
	{
		return error("shm_open");
	}

	if (ftruncate(fdm, data_size) == -1)
	{
		return error("ftruncate");
	}

	s_struct *data = (s_struct *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdm, 0);
	if (data == NULL)
	{
		return error("mmap");
	}

	p = fork();
	if (p < 0)
	{
		return error("fork");
	}
	else if (p > 0)
	{
		for (int i = 0; i < ARRAY_SIZE; i++)
		{
			data[i].num = i;
			strcpy(data[i].str, "ISEP - SCOMP 2020");
		}

		wait(NULL);
		munmap(data, data_size);
		shm_unlink(SM);
	}
	else
	{
		while (!data[ARRAY_SIZE - 1].num);

		printf("Time to transfer data with shared memory in milliseconds: %ldms\n", clock() - start_time);

		munmap(data, data_size);
		close(fdm);

		exit(0);
	}

	///////////
	// Pipes //
	///////////
	start_time = clock();
	pipe(fd);

	p = fork();
	if (p < 0)
	{
		return error("fork");
	}
	else if (p > 0)
	{
		close(fd[0]);
		write(fd[1], data, data_size);
		close(fd[1]);

		wait(NULL);
	}
	else
	{
		close(fd[1]);
		write(fd[0], data, data_size);
		printf("Time to transfer data with pipes in milliseconds: %ldms\n", clock() - start_time);
		close(fd[0]);

		exit(0);
	}

	return 0;
}