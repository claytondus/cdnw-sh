#ifndef INCLUDE_BITMAP_H_
#define INCLUDE_BITMAP_H_

#include <stdint.h>
#include <stdbool.h>
#include "block.h"

uint32_t find_free_bit(block* blk);
bool read_bitmap(block* blk, uint32_t index);
void set_bitmap(block* blk, uint32_t index);
void clear_bitmap(block* blk, uint32_t index);


#endif /* INCLUDE_BITMAP_H_ */
