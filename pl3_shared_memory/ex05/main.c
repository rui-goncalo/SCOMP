#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define SM "/shmex05"

typedef struct
{
	int num1;
	int num2;
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

	data->num1 = 8000;
	data->num2 = 200;

	printf("(Before) Num1: %d | Num2: %d\n", data->num1, data->num2);

	pid_t p = fork();
	if (p < 0)
	{
		return error("fork");
	}
	else if (p > 0)
	{
		for (int i = 0; i < 1000000; i++)
		{
			data->num1++;
			data->num2--;
		}

		wait(NULL);

		printf("(After) Num1: %d | Num2: %d\n", data->num1, data->num2);

		munmap(data, data_size);
		shm_unlink(SM);
	}
	else
	{
		for (int i = 0; i < 1000000; i++)
		{
			data->num1--;
			data->num2++;
		}

		munmap(data, data_size);
		close(fdm);

		exit(0);
	}

	return 0;
}