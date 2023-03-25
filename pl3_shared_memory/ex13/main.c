#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define ARRAY_SIZE 10
#define BUFFER_SIZE 32
#define SM "/shmex13"

typedef struct
{
	char path[16];
	char word[16];
	int count;
	char done;
} ss_struct;

typedef struct
{
	ss_struct array[ARRAY_SIZE];
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

	// Fill array with information
	strcpy(data->array[0].path, "text0.txt");
	strcpy(data->array[0].word, "banana");

	strcpy(data->array[1].path, "text1.txt");
	strcpy(data->array[1].word, "tomato");

	strcpy(data->array[2].path, "text2.txt");
	strcpy(data->array[2].word, "egg");

	strcpy(data->array[3].path, "text3.txt");
	strcpy(data->array[3].word, "salt");

	strcpy(data->array[4].path, "text4.txt");
	strcpy(data->array[4].word, "oil");

	strcpy(data->array[5].path, "text5.txt");
	strcpy(data->array[5].word, "banana");

	strcpy(data->array[6].path, "text6.txt");
	strcpy(data->array[6].word, "tomato");

	strcpy(data->array[7].path, "text7.txt");
	strcpy(data->array[7].word, "egg");

	strcpy(data->array[8].path, "text8.txt");
	strcpy(data->array[8].word, "salt");

	strcpy(data->array[9].path, "text9.txt");
	strcpy(data->array[9].word, "oil");

	pid_t p[ARRAY_SIZE];

	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		p[i] = fork();

		if (p[i] < 0)
		{
			return error("fork");
		}
		else if (p[i] == 0)
		{
			char str[BUFFER_SIZE];

			FILE *fp = fopen(data->array[i].path, "r");
			if (fp)
			{
				fread(str, 1, BUFFER_SIZE, fp);

				for (int j = 0; j < BUFFER_SIZE; j++)
				{
					for (int k = 0; k < strlen(data->array[i].word); k++)
					{
						if (str[j + k] != data->array[i].word[k])
						{
							break;
						}
						else
						{
							if (k == strlen(data->array[i].word) - 1)
							{
								data->array[i].count++;
							}
						}
					}
				}

				data->array[i].done = 1;
			}

			munmap(data, data_size);
			close(fdm);

			exit(0);
		}
	}

	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		while (!data->array[i].done);
		printf("%d. %d\n", i, data->array[i].count);
	}

	munmap(data, data_size);
	shm_unlink(SM);

	return 0;
}