/*
 * inode.c
 *
 *  Created on: Feb 26, 2016
 *      Author: Clayton Davis
 */

#include "inode.h"
#include "blockdev.h"

uint32_t find_inode_table_blockid(iptr index)
{
	return index / INODES_IN_BLOCK;
}


uint8_t inode_write(iptr index, inode* inode_st)
{
	block* inode_blk = malloc(sizeof(block));
	uint32_t lba = BLOCKID_INODE_TABLE + find_inode_table_blockid(index);
	blk_read(lba, inode_blk);

	inode* inode_table = (inode*)inode_blk;
	memcpy(inode_table + (index % INODES_IN_BLOCK), inode_st, sizeof(inode));
	blk_write(lba, inode_blk);

	free(inode_blk);
	return 0;
}

uint8_t inode_read(iptr index, inode* inode_st)
{
	block* inode_blk = malloc(sizeof(block));
	uint32_t lba = BLOCKID_INODE_TABLE + find_inode_table_blockid(index);
	blk_read(lba, inode_blk);

	inode* inode_table = (inode*)inode_blk;
	memcpy(inode_st, inode_table + (index % INODES_IN_BLOCK), sizeof(inode));

	free(inode_blk);
	return 0;
}


