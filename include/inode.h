/*
 * inode.h
 *
 *  Created on: Feb 26, 2016
 *      Author: parallels
 */

#ifndef INCLUDE_INODE_H_
#define INCLUDE_INODE_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "fsparams.h"

#define INODE_ROOTDIR 0

typedef uint32_t iptr; // inode pointer


typedef struct {
	uint8_t type;
	uint32_t size;
	uint32_t blocks;

	uint32_t created;
	uint32_t modified;
	uint32_t accessed;

	iptr data0[8];		// direct data block pointers
	iptr data1;			// single indirect data block pointers
	iptr data2;			// double indirect data block pointers

	uint8_t padding[INODE_PADDING];
} inode;  //64 bytes, 64 inodes/block



uint8_t inode_write(iptr, inode*);
uint8_t inode_read(iptr, inode*);

#endif /* INCLUDE_INODE_H_ */
