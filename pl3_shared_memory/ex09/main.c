#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define ARRAY_SIZE 1000
#define CHILD_COUNT 10
#define SM "/shmex09"

typedef struct
{
	int nums[ARRAY_SIZE];
	int max[CHILD_COUNT];
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

	srand(time(0));
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		data->nums[i] = rand() % 1001;
	}

	pid_t p[CHILD_COUNT];

	for (int i = 0; i < CHILD_COUNT; i++)
	{
		p[i] = fork();

		if (p[i] < 0)
		{
			return error("fork");
		}
		else if (p[i] == 0)
		{
			data->max[i] = data->nums[(ARRAY_SIZE / CHILD_COUNT) * i];
			for (int j = (ARRAY_SIZE / CHILD_COUNT) * i + 1; j < (ARRAY_SIZE / CHILD_COUNT) * i + (ARRAY_SIZE / CHILD_COUNT); j++)
			{
				if (data->max[i] < data->nums[j])
				{
					data->max[i] = data->nums[j];
				}
			}

			munmap(data, data_size);
			close(fdm);

			exit(0);
		}
	}

	// Wait for all the child processes to finish
	for (int i = 0; i < CHILD_COUNT; i++)
	{
		wait(NULL);
	}

	// Get the max number of the max array
	int max = data->max[0];
	for (int i = 1; i < CHILD_COUNT; i++)
	{
		if (max < data->max[i])
		{
			max = data->max[i];
		}
	}

	printf("Max number = %d\n", max);

	munmap(data, data_size);
	shm_unlink(SM);

	return 0;
}