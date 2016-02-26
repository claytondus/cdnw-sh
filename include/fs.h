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
#include "block.h"

#define FS_MAGIC 0xCD5E
#define FS_VALID 0x0001
#define FS_ERROR 0x0002

#define BLOCKS_PER_INODE	4
#define INODE_SIZE			64
#define INODE_PADDING		7
#define INODE_COUNT			(BD_SIZE_BLOCKS / BLOCKS_PER_INODE)

#define BLOCKID_SUPER			0
#define BLOCKID_BLOCK_BITMAP	1
#define BLOCKID_INODE_BITMAP	2
#define BLOCKID_INODE_TABLE		3


#define ITYPE_FILE 		0
#define ITYPE_DIR		1

#define MAX_FD			1024



typedef uint32_t iptr; // inode pointer

typedef struct {
	uint8_t type;
	uint32_t size;

	uint32_t created;
	uint32_t modified;
	uint32_t accessed;

	iptr data0[8];		// direct data block pointers
	iptr data1;			// single indirect data block pointers
	iptr data2;			// double indirect data block pointers

	uint8_t padding[INODE_PADDING];
} inode;  //64 bytes, 64 inodes/block


#define DIR_ENTRY_UNUSED		0

typedef struct {
	iptr fnode;			// iptr to entry's file/folder; 0 = unused/end-of-list
	uint16_t entry_len;	// length in bytes to the next dir_entry within the block or next block if equals block size
	uint8_t name_len;	// length in bytes of the entry's file/folder name
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

struct fd_entry {
	uint8_t state;
	int16_t fd;
	iptr fnode;
	uint32_t cursor;
};

#endif /* INCLUDE_FS_H_ */
