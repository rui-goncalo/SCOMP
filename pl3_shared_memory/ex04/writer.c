#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


typedef struct {
    char array[100];
} main_struct;

int main() {
    //1st phase

    int var;
    int size = sizeof(main_struct);
    main_struct *pt_main_struct;

    //2nd phase

    // Removes mem. space from the system file
    shm_unlink("/ex04"); 

    // Creates a shared memory area
    var = shm_open("/ex04", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR); 

    //Maps the shared area in the process address space
    pt_main_struct = (main_struct *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, var, 0);

    //3rd phase
    srand(time(NULL));
    char aux[100];

    for (int i; i < 100; i++) {
        aux[i] = (rand() % 26) + 65;
    }

    strcpy(pt_main_struct->array, aux);

    //last phase
    munmap(pt_main_struct, size);
    close(var);
}