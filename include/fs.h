/*
 * fs.h
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#ifndef INCLUDE_FS_H_
#define INCLUDE_FS_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "fsparams.h"
#include "inode.h"
#include "block.h"
#include "blockdev.h"

#define ITYPE_FILE 		0
#define ITYPE_DIR		1

#define MAX_FD			1024





#define DIR_ENTRY_UNUSED		0

typedef struct {
	iptr fnode;			// iptr to entry's file/folder; 0 = unused/end-of-list
	uint16_t entry_len;	// length in bytes to the next dir_entry within the block or next block if equals block size
	uint8_t name_len;	// length in bytes of the entry's file/folder name
	uint8_t file_type;   // ITYPE_FILE / ITYPE_DIR
	char name[1];	// first character of the entry name
} dir_entry;

// free block or inode bitmap values
#define BM_FREE		0
#define BM_USED		1

#define VFS_BLANK	0
#define VFS_GOOD	1
#define VFS_ERR		-1

// holds values related to a virtual file system file
struct vfs {

	uint8_t state;
	struct superblock *superblk;
	uint8_t *free_inodes;
	uint8_t *free_blocks;

};

#define FD_FREE 	0
#define FD_READ		1
#define FD_WRITE	2

typedef struct {
	uint8_t state;
	int16_t fd;
	iptr fnode;
	uint32_t cursor;
} fd_entry;


uint8_t mkfs(void);

#endif /* INCLUDE_FS_H_ */
