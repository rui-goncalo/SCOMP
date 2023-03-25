#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

typedef struct {
	int prod_code;
    char description[64];
    float price;
} prod_bruh;

int writer() {
	int data_size = sizeof(prod_bruh);
	shm_unlink("/ex02");
	
	int fd = shm_open("/ex02", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1) {
        return -1;
    }
	
	prod_bruh* bruh = (prod_bruh*)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(bruh == NULL) {
		return -1;
	}
	
	ftruncate(fd, data_size);
	
	printf("Code: ");
	scanf("%d", &bruh->prod_code);
	printf("Description: ");
	scanf("%s", bruh->description);
    printf("Price: ");
	scanf("%.f", bruh->description);
    
	return 0;
}

int main()
{	
	if(writer() == -1) {
		return -1;
	}
	return 0;
}