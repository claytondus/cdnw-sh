/*
 * fs.h
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#ifndef INCLUDE_FS_H_
#define INCLUDE_FS_H_

#define BLOCK_SIZE			4096
#define BLOCKS_PER_INODE	4
#define INODE_SIZE			64
#define INODE_PADDING		7

#define BLOCK_SUPER			0
#define BLOCK_FREE_INODES	1
#define BLOCK_FREE_BLOCKS	2
#define BLOCK_INODE_TABLE	3

#define ITYPE_FILE 		0
#define ITYPE_FOLDER	1

#define MAX_FD			1024

struct block {
	uint8_t byte[BLOCK_SIZE];
};

typedef uint32_t iptr; // inode pointer

struct inode {
	uint8_t type;
	uint32_t size;

	uint32_t created;
	uint32_t modified;
	uint32_t accessed;

	iptr data0[8];		// direct data block pointers
	iptr data1;			// single indirect data block pointers
	iptr data2;			// double indirect data block pointers

	uint8_t padding[INODE_PADDING];
};

struct superblock {
	uint32_t inode_count;
	uint32_t block_count;

	uint32_t free_inode_count;
	uint32_t free_block_count;

	uint32_t first_data_block;		// must also be the start of the directory listing for the filesystem root
};

#define DIR_ENTRY_UNUSED		0

struct dir_entry {
	iptr fnode;			// iptr to entry's file/folder; 0 = unused/end-of-list
	uint16_t entry_len;	// length in bytes to the next dir_entry within the block or next block if equals block size
	uint8_t name_len;	// length in bytes of the entry's file/folder name
	char name[1];	// first character of the entry name
};

// free block or inode bitmap values
#define BM_FREE		0
#define BM_USED		1

#define VFS_BLANK	0
#define VFS_GOOD	1
#define VFS_ERR		-1

// holds values related to a virtual file system file
struct vfs {

	int8_t state;
	struct superblock *superblk;
	uint8_t *free_inodes;
	uint8_t *free_blocks;

	// ptr to memory mapped vfs file
	char* fs;

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
