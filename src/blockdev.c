#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "blockdev.h"

#define BD_SIZE_BYTES ((BLOCK_SIZE)*(BD_SIZE_BLOCKS))

#define EFSOPEN -1
#define ESTRETCH -2
#define ELASTBYTE -3
#define EMMAP -4

int fd = -1;
uint8_t *bd;
bool attached;

int8_t blockdev_attach(void)
{
	if(attached == false) {
		if((fd = open("/tmp/fs.bin", O_RDWR|O_CREAT, 0660)) == -1)
			err(1, "Open fs.bin");
		if (lseek(fd, BD_SIZE_BYTES-1, SEEK_SET) == -1)
			err(1, "Error stretching fs.bin");
		if (write(fd, "", 1) != 1)
			err(1, "Error writing last byte of file");

		bd = (uint8_t*)mmap(NULL, BD_SIZE_BYTES, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
		if (bd == MAP_FAILED)
			errx(1, "mmap failed");

		attached = true;
	}
	else
	{
		err(1,"blockdev already attached");
	}
	return EXIT_SUCCESS;
}

int8_t blockdev_detach(void)
{
	if (attached == true) {
		munmap(bd, BD_SIZE_BYTES);
		close(fd);
		attached = false;
	}
	else
	{
		err(1,"blockdev not attached");
	}
	return EXIT_SUCCESS;
}

int8_t blockdev_destroy(void)
{
	if(attached == true) {
		err(1,"cannot destroy attached blockdev");
	}
	unlink("/tmp/fs.bin");
	return EXIT_SUCCESS;
}

int8_t blk_read(const uint32_t lba, block* b_ptr) {
	if(b_ptr == NULL || lba > BD_SIZE_BLOCKS) {
		return -1;
	}
	//debug("Reading block %u",lba);
	memcpy(b_ptr->byte, ((block*)bd)+lba, BLOCK_SIZE);
	return 0;
}

int8_t blk_write(const uint32_t lba, const block* b_ptr) {
	if(b_ptr == NULL || lba > BD_SIZE_BLOCKS) {
		return -1;
	}
	//debug("Writing block %u",lba);
	memcpy(((block*)bd)+lba, b_ptr->byte, BLOCK_SIZE);
	msync(((block*)bd)+lba, BLOCK_SIZE, MS_SYNC);
	return 0;
}
