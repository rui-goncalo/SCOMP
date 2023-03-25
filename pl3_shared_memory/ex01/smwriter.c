#include <stdio.h>	  // printf
#include <fcntl.h>	  // shm_open
#include <sys/mman.h> // mmap
#include <unistd.h>	  // ftruncate

typedef struct
{
	int number;
	char name[128];
} s_student;

int writer()
{
	shm_unlink("/shmex01");
	int data_size = sizeof(s_student);

	int fd = shm_open("/shmex01", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		return -1;
	}

	if (ftruncate(fd, data_size) == -1)
	{
		return -1;
	}

	s_student *student = (s_student *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (student == NULL)
	{
		return -1;
	}

	printf("Choose a number: ");
	scanf("%d", &student->number);
	printf("Chose a name: ");
	scanf("%s", student->name);

	munmap(student, data_size);
	close(fd);

	return 0;
}

int reader()
{
	int data_size = sizeof(s_student);

	int fd = shm_open("/shmex01", O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		return -1;
	}

	if (ftruncate(fd, data_size) == -1)
	{
		return -1;
	}

	s_student *student = (s_student *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (student == NULL)
	{
		return -1;
	}

	printf("\nNumber: %d | Name: \"%s\"\n", student->number, student->name);

	munmap(student, data_size);
	shm_unlink("/shmex01");

	return 0;
}

int main()
{
	return writer();
}