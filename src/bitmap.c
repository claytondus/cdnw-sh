/*
 * bitmap.c
 *
 *  Created on: Feb 26, 2016
 *      Author: Clayton Davis
 */

#include "bitmap.h"

uint32_t find_free_bit(block* blk)
{
	//TODO: Implement find free bit in bitmap
	blk->byte[0] = 0;
	return 0;
}

bool read_bitmap(block* blk, uint32_t index)
{
	//Calculate which byte contains the bit
	uint32_t byte_index = index >> 3;
	//AND the byte with a bitmask and compare to 0
	return blk->byte[byte_index] & (1 << (index % 8));
}

void set_bitmap(block* blk, uint32_t index)
{
	//Calculate which byte contains the bit
	uint32_t byte_index = index >> 3;
	//shift a 1 into the byte and OR it with the byte
	blk->byte[byte_index] |= 1 << (index % 8);
};

void clear_bitmap(block* blk, uint32_t index)
{
	//Calculate which byte contains the bit
	uint32_t byte_index = index >> 3;
	//shift a 1 into the byte, invert it, and AND it with the byte
	blk->byte[byte_index] &= !(1 << (index % 8));
}
