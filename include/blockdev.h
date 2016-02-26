/*
 * fs.h
 *
 *  Created on: Feb 13, 2016
 *      Author: Clayton Davis
 */

#ifndef INCLUDE_BLOCKDEV_H_
#define INCLUDE_BLOCKDEV_H_

#define BD_SIZE_BLOCKS 25600

#include <stdint.h>
#include "block.h"

int8_t blockdev_attach(void);
int8_t blockdev_detach(void);
int8_t blk_read(const uint32_t, block*);
int8_t blk_write(const uint32_t, const block*);

#endif /* INCLUDE_BLOCKDEV_H_ */
