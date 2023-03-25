#include <stdio.h> // printf
#include <fcntl.h> // shm_open
#include <sys/mman.h> // mmap
#include <unistd.h> // ftruncate

typedef struct {
	int prod_code;
    char description[64];
    float price;
} prod_bruh;

int reader() {
	int data_size = sizeof(prod_bruh);
	
	int fd = shm_open("/ex02", O_RDWR, S_IRUSR|S_IWUSR);
		if(fd == -1) {
        return -1;
    }
	
	prod_bruh* bruh = (prod_bruh*)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(bruh == NULL) {
		return -1;
	}
	
	printf("\nCode: %d | Description: %s | Price: %f\n", bruh->prod_code, bruh->description, bruh->price);
	
	return 0;
}

int main()
{	
	if(reader() == -1) {
		return -1;
	}
	return 0;
}
