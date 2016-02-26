#include "fs.h"
#include "blockdev.h"
#include "bitmap.h"

#define SUPERBLOCK_PADDING (BLOCK_SIZE-1048)


typedef struct {
	uint8_t boot_record[1024]; //0
	uint32_t inode_count;      //1023
	uint32_t block_count;      //1027

	uint32_t free_inode_count; //1031
	uint32_t free_block_count; //1035

	uint32_t first_data_block; //1039, must also be the start of the directory listing for the filesystem root
	uint16_t magic; 		   //1043
	uint16_t state;			   //1045
	uint8_t padding[SUPERBLOCK_PADDING];
} superblock;


void mkfs(void)
{
	superblock_init();
	block_bitmap_init();
	inode_bitmap_init();
	write_root_dir();
}

void superblock_init(void)
{
	superblock *sb = malloc(sizeof(superblock));
	sb->inode_count = INODE_COUNT;
	sb->block_count = BD_SIZE_BLOCKS-1;
	sb->free_inode_count = INODE_COUNT;
	sb->free_block_count = BD_SIZE_BLOCKS-1;
	sb->magic = FS_MAGIC;
	sb->state = FS_VALID;
	blk_write(BLOCKID_SUPER, sb);
	free(sb);
}

void block_bitmap_init(void)
{
	block *block_btm = malloc(sizeof(block));
	memset(block_btm, 0, sizeof(block));

	//Mark all reserved blocks as used
	set_bitmap(block_btm, BLOCKID_SUPER);
	set_bitmap(block_btm, BLOCKID_BLOCK_BITMAP);
	set_bitmap(block_btm, BLOCKID_INODE_BITMAP);

	//Mark all inode table blocks as used
	for (uint16_t i = BLOCKID_INODE_TABLE; i < (INODE_COUNT >> 6) + BLOCKID_INODE_TABLE; i++) {
		set_bitmap(block_btm, i);
	}
	blk_write(BLOCKID_BLOCK_BITMAP, block_btm);
	free(block_btm);
}

void inode_bitmap_init(void)
{
	block *inode_btm = malloc(sizeof(block));
	memset(inode_btm, 0, sizeof(block));

	//Mark first inode as used
	set_bitmap(inode_btm, 0);

	blk_write(BLOCKID_BLOCK_BITMAP, inode_btm);
	free(inode_btm);
}

void write_root_dir(void)
{
	inode root_i;
	time_t now = time(NULL);
	root_i->created = now;
	root_i->accessed = now;
	root_i->modified = now;
	root_i->type = ITYPE_DIR;

}

