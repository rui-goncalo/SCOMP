#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define STR_SIZE 50
#define NR_DISC 10

typedef struct
{
    int number;
    char name[STR_SIZE];
    int courses[NR_DISC];
} aluno;

int main()
{
    //*********1st phase *********

    int var;
    int struct_size = sizeof(aluno);
    aluno *pt_main_struct;

    //********* 2nd phase *********

    // Removes mem. space from the system file
    shm_unlink("/ex12");

    // Creates a shared memory area
    var = shm_open("/ex12", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);

    // Defines the size
    ftruncate(var, struct_size);

    // Maps the shared area in the process address space
    pt_main_struct = (aluno *)mmap(NULL, struct_size, PROT_READ | PROT_WRITE, MAP_SHARED, var, 0);

    //********* 3rd phase *********

    int min;
    int max;
    int total;
    double average;

    pid_t p;
    p = fork();

    if (p < 0)
    {
        perror("FORK ERROR");
        return -1;
    }

    if (p > 0)
    {
        printf("Student number: ");
        scanf("%d \n", &pt_main_struct->number);
        printf("Student name: ");
        scanf("%s \n", pt_main_struct->name);
        printf("Student grades: %d\n", NR_DISC);
        for (int i = 0; i < NR_DISC; i++) {
            printf("%d, ", i + 1);
            scanf("%d", &pt_main_struct->courses[i]);
        }
        wait(NULL);
    } else {
        min = 999;
        max = 0;
        total = 0;

        for(int i = 0; i < NR_DISC; i++) {
            if (pt_main_struct->courses[i] < min) {
                min = pt_main_struct->courses[i];
            } 
            if(pt_main_struct->courses[i] > max) {
                max = pt_main_struct->courses[i];
            }
            total += pt_main_struct->courses[i];
        }

        average = ((double)total)/NR_DISC;

        printf("Student number: %d\n", pt_main_struct->number);
        printf("Student name: %s\n", pt_main_struct->name);
        printf("Lower grade: %d\n", min);
        printf("Highest grade: %d\n", max);
        printf("Average grades: %.4f\n", average);

        exit(0);
    }

    // last phase
    munmap(pt_main_struct, struct_size);
    close(var);
}