/*
 * fs.h
 *
 *  Created on: Feb 13, 2016
 *      Author: Clayton Davis
 */

#ifndef INCLUDE_BLOCKDEV_H_
#define INCLUDE_BLOCKDEV_H_

#include "fs.h"

struct block* blk_read(uint32_t);
int8_t blk_write(uint32_t,struct block*);
int8_t chk_vfs(struct vfs*);

int test_blockdev(void);

#endif /* INCLUDE_BLOCKDEV_H_ */
