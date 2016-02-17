#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>

int test_fs(void) 
{
	const char str1[] = "Test this";
	int fd = -1;
	char *fs;
        
	if((fd = open("fs.bin", O_RDWR, 0)) == -1)
		err(1, "Open fs.bin");
	fs = (char*)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (fs == MAP_FAILED)
		errx(1, "mmap failed");

	strcpy(fs, str1);
        printf("%s\n",fs);
        munmap(fs, 4096);
        close(fd);
	return EXIT_SUCCESS;	

}
