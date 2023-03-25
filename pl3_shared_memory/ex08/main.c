#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define size 1000000

typedef struct
{
    int value;
} main_struct;

int main()
{
    //*********1st phase *********

    int var;
    int struct_size = sizeof(main_struct);
    main_struct *pt_main_struct;

    //********* 2nd phase *********

    // Removes mem. space from the system file
    shm_unlink("/ex08");

    // Creates a shared memory area
    var = shm_open("/ex08", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);

    // Defines the size
    ftruncate(var, struct_size);

    // Maps the shared area in the process address space
    pt_main_struct = (main_struct *)mmap(NULL, struct_size, PROT_READ | PROT_WRITE, MAP_SHARED, var, 0);

    //********* 3rd phase *********

    pid_t p;
    p = fork();

    if (p < 0)
    {
        perror("FORK ERROR");
        exit(-1);
    }
    pt_main_struct->value = 100;

    if (p == 0)
    {
        for (int i = 0; i < size; i++)
        {
            pt_main_struct->value++;
            pt_main_struct->value--;
        }
        exit(0);
    }

    for (int i = 0; i < size; i++)
    {
        pt_main_struct->value++;
        pt_main_struct->value--;
    }

    wait(NULL);

    printf("Value: %d\n", pt_main_struct->value);

    // last phase
    munmap(pt_main_struct, struct_size);
    close(var);
}