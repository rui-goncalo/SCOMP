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
    
    //********* 3rd phase *********
    srand(time(NULL));
    int aux[size];

    for (int i; i < size; i++) {
        aux[i] = rand();
    }

    clock_t begin = clock();
    int var[2];

    if(pipe(var) == -1) {
        perror("PIPE ERROR\n");
        return 1;
    }

    pid_t p;
    p = fork();

    if (p < 0) {
        perror("FORK ERROR");
        exit(-1);
    }

    if(p == 0) {
        close(var[1]);
        for (int i = 0; i < size; i++) {
            read(var[0], &aux[i], sizeof(aux[i]));
        }
        close(var[0]);
        exit(0);
    }

    close(var[0]);
    int aux2[size];
    for(int i = 0; i < size; i++) {
        write(var[1], &aux2[i], sizeof(aux2[i]));
    }

    close(var[1]);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Time Spent in Pipes: %f\n", time_spent);

}