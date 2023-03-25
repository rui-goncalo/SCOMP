#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define ARRAY_SIZE 10
#define SM "/shmex14"

typedef struct
{
	int nums[ARRAY_SIZE];
	char done;
} s_struct;

int error(char *msg)
{
	printf("ERROR: %s\n", msg);
	shm_unlink(SM);
	return -1;
}

int main()
{
	int data_size = sizeof(s_struct);

	int fdm = shm_open(SM, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
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

	pid_t p = fork();

	if (p < 0)
	{
		return error("fork");
	}
	else if (p > 0)
	{
		for (int i = 0; i < 3; i++)
		{
			while (data->done);

			for (int j = 0; j < ARRAY_SIZE; j++)
			{
				data->nums[j] = 1 + j + i * 10;
			}

			data->done = 1;
		}

		wait(NULL);
		munmap(data, data_size);
		shm_unlink(SM);
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			while (!data->done);

			for (int j = 0; j < ARRAY_SIZE; j++)
			{
				printf("i.%d | j.%d = %d\n", i, j, data->nums[j]);
			}

			data->done = 0;
		}

		munmap(data, data_size);
		close(fdm);

		exit(0);
	}

	return 0;
}