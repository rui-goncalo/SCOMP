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

typedef struct {
    int random_no[size];
} main_struct;

int main() {
    //*********1st phase *********

    int var;
    int struct_size = sizeof(main_struct);
    main_struct *pt_main_struct;

    //********* 2nd phase *********

    // Removes mem. space from the system file
    shm_unlink("/ex06"); 

    // Creates a shared memory area
    var = shm_open("/ex06", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);

    //Defines the size
    ftruncate(var, struct_size);

    //Maps the shared area in the process address space
    pt_main_struct = (main_struct *)mmap(NULL, struct_size, PROT_READ | PROT_WRITE, MAP_SHARED, var, 0);

    //********* 3rd phase *********
    srand(time(NULL));
    int aux[size];

    for (int i; i < size; i++) {
        aux[i] = rand();
    }

    clock_t begin = clock();

    pid_t p;
    p = fork();

    if (p < 0) {
        perror("FORK ERROR");
        exit(-1);
    }

    if(p == 0) {
        for (int i = 0; i < size; i++) {
            pt_main_struct->random_no[i] = aux[i];
        }
        exit(0);
    }

    wait(NULL);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Time Spent in SH.Memory: %f\n", time_spent);


    //last phase
    munmap(pt_main_struct, struct_size);
    close(var);
}