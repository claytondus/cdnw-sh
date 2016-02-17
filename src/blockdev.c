#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <sys/types.h>
#include <sys/mman.h>
#define __USE_ATFILE
#include <fcntl.h>
#include <stdio.h>

#define FS_SIZE 104857600

int test_blockdev(void)
{
	const char str1[] = "Test this";
	char str2[10];
	int fd = -1;
	char *fs;

	if((fd = open("/tmp/fs.bin", O_RDWR|O_CREAT, 0660)) == -1)
		err(1, "Open fs.bin");
	if (lseek(fd, FS_SIZE-1, SEEK_SET) == -1)
		err(1, "Error stretching fs.bin");
	if (write(fd, "", 1) != 1)
		err(1, "Error writing last byte of file");

	fs = (char*)mmap(NULL, FS_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (fs == MAP_FAILED)
		errx(1, "mmap failed");

	strcpy(fs, str1);
	printf("%s\n",fs);
	strcpy(str2, fs);
	printf("Got %s from mmap\n",str2);

	munmap(fs, FS_SIZE);
	close(fd);
	return EXIT_SUCCESS;	

}
