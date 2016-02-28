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
	uint8_t byte_found;
	uint16_t ptr;
	for(ptr = 0; ptr < BLOCK_SIZE; ptr++)
	{
		if((blk->byte[ptr] & 0xFF) != 0xFF)
		{
			byte_found = blk->byte[ptr];
			break;
		}
	}
	if(ptr == BLOCK_SIZE) return 0;  //No byte found with cleared bits

	//Find zero bit in this byte
	uint8_t mask = 0x01;
	uint8_t offset;
	for(offset = 0; offset < 8; offset++)
	{
		if((byte_found & mask) == 0)
		{
			break;
		}
		mask <<= 1;
	}
	return ptr*8 + offset;
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
	//debug("Setting bit %d",index);
	uint32_t byte_index = index / 8;
	//debug("Byte index is %d",byte_index);
	//shift a 1 into the byte and OR it with the byte
	uint8_t offset = index % 8;
	//debug("Bit offset is %"PRIu8, offset);
	uint8_t mask = 1 << offset;
	//debug("Mask is %"PRIx8, mask);
	blk->byte[byte_index] |= mask;
};

void clear_bitmap(block* blk, uint32_t index)
{
	//Calculate which byte contains the bit
	//debug("Clearing bit %d",index);
	uint32_t byte_index = index / 8;
	//debug("Byte index is %d",byte_index);
	//shift a 1 into the byte, invert it, and AND it with the byte
	uint8_t offset = index % 8;
	//debug("Bit offset is %"PRIu8,offset);
	uint8_t mask = ~(1 << offset);
	//debug("Mask is %"PRIx8, mask);
	blk->byte[byte_index] &= mask;
}
