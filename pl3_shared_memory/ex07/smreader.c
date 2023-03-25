#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define ARRAY_SIZE 10
#define SM "/shmex07"

typedef struct
{
	int nums[ARRAY_SIZE];
} s_struct;

int error(char *msg)
{
	printf("ERROR: %s\n", msg);
	shm_unlink(SM);
	return -1;
}

int writer()
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
		data->nums[i] = (rand() % 20) + 1;
		printf("%d. %d\n", i, data->nums[i]);
	}

	munmap(data, data_size);
	close(fdm);

	return 0;
}

int reader()
{
	int data_size = sizeof(s_struct);

	int fdm = shm_open(SM, O_RDWR, S_IRUSR | S_IWUSR);
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

	int sum = 0;
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		sum += data->nums[i];
	}

	printf("Total = %d | Average = %.1f\n", sum, (float)sum / ARRAY_SIZE);

	munmap(data, data_size);
	shm_unlink(SM);

	return 0;
}

int main()
{
	return reader();
}