#include "fs.h"
#include "bitmap.h"
#include "inode.h"

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


void superblock_init(void)
{
	superblock *sb = malloc(sizeof(block));
	sb->inode_count = INODE_COUNT;
	sb->block_count = BD_SIZE_BLOCKS;
	sb->free_inode_count = INODE_COUNT-1;
	sb->free_block_count = BD_SIZE_BLOCKS-4-INODE_TABLE_BLOCKS;  // 4 = super + bitmaps + rootdir
	sb->magic = FS_MAGIC;
	sb->state = FS_VALID;
	blk_write(BLOCKID_SUPER, (block*)sb);
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
	for (uint16_t i = BLOCKID_INODE_TABLE; i < INODE_TABLE_BLOCKS + BLOCKID_INODE_TABLE; i++) {
		set_bitmap(block_btm, i);
	}

	//Mark root directory block as used
	set_bitmap(block_btm, INODE_TABLE_BLOCKS + BLOCKID_INODE_TABLE + 1);

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
	//Prepare inode
	inode root_i;
	uint32_t now = time(NULL);
	root_i.created = now;
	root_i.accessed = now;
	root_i.modified = now;
	root_i.type = ITYPE_DIR;
	root_i.size = 0;
	root_i.data0[0] = BLOCKID_ROOT_DIR;

	block* root_dir_block = malloc(sizeof(block));

	// . (self entry)
	dir_entry* root_dir_entry = (dir_entry*)root_dir_block;
	root_dir_entry->fnode = 0;
	root_dir_entry->file_type = ITYPE_DIR;
	root_dir_entry->name_len = 1;
	root_dir_entry->entry_len = 12;
	memcpy(root_dir_entry->name, ".", 1);
	root_i.size += 12;

	// .. (parent entry, also itself)
	root_dir_entry = (dir_entry*)(((char*)root_dir_block) + 12);
	root_dir_entry->fnode = 0;
	root_dir_entry->file_type = ITYPE_DIR;
	root_dir_entry->name_len = 2;
	root_dir_entry->entry_len = 12;
	memcpy(root_dir_entry->name, "..", 2);
	root_i.size += 12;

	inode_write(0, &root_i);
	blk_write(BLOCKID_ROOT_DIR, root_dir_block);

	free(root_dir_block);
}

uint8_t mkfs(void)
{
	superblock_init();
	block_bitmap_init();
	inode_bitmap_init();
	write_root_dir();
	return 0;
}
