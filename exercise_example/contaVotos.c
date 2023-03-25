#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>

#define N_PROCESSOS 5
#define TOTALVOTANTES 5000

typedef struct {
    char mocao[100];
    char votes[5000];
    int end;
    int total_votes_yes;
    int total_votes_no;
    int total_votes_abstentation;
} elections;

int countVotes(elections *str_elections, int range, int current_pid) {
    
}

int main() {
    //1st phase
    int var;
    int size = sizeof(elections);
    elections *pt_main_struct;

    //2nd phase

    // Removes mem. space from the system file
    shm_unlink("/contaVotos"); 

    // Creates a shared memory area
    var = shm_open("/contaVotos", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR); 

    //Maps the shared area in the process address space
    pt_main_struct = (elections *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, var, 0);

    //3rd phase - custom code

    pt_main_struct->total_votes_yes = 0;
    pt_main_struct->total_votes_no = 0;
    pt_main_struct->total_votes_abstentation = 0;

    char *txt = "O Fábio Coentrão está despedido: ";
    strcpy(pt_main_struct->mocao, txt);
    printf("Moção: %s\n", pt_main_struct->mocao);

    sem_t *sem_begin;
    sem_t *sem_waiting;
    sem_t *sem_incrementing;
    sem_t *sem_counter;

    if ((sem_begin = sem_open("sem_begin", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
        perror("Error in sem_open()");
        exit(EXIT_FAILURE);
    }

    if((sem_waiting = sem_open("sem_waiting", O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("Error in sem_open()");
        exit(EXIT_FAILURE);
    }

    if((sem_incrementing = sem_open("sem_incrementing", O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("Error in sem_open()");
        exit(EXIT_FAILURE);
    }

    if((sem_counter = sem_open("sem_counter", O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("Error in sem_open()");
        exit(EXIT_FAILURE);
    }

    memset(pt_main_struct->votes, 0, sizeof(pt_main_struct->votes));
    sem_post(sem_begin);

    sleep(20);

    pt_main_struct->end = 1;

    int id = spa

	//last phase
    munmap(pt_main_struct, size);
    close(var);
}