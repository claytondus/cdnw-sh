#include "fs.h"
#include "bitmap.h"
#include "inode.h"

#define SUPERBLOCK_PADDING (BLOCK_SIZE-1048)

// free block or inode bitmap values
#define BM_FREE		0
#define BM_USED		1

#define VALID_FS 	1
#define ERROR_FS 	2

#define VFS_BLANK	0
#define VFS_GOOD	1
#define VFS_ERR		-1

typedef struct {
	iptr inode;			// iptr to entry's file/folder; 0 = unused/end-of-list
	uint16_t entry_len;	// length in bytes to the next dir_entry within the block or next block if equals block size
	uint8_t name_len;	// length in bytes of the entry's file/folder name
	uint8_t file_type;   // ITYPE_FILE / ITYPE_DIR
	char name[1];	// first character of the entry name
} dir_entry;

typedef struct {
	inode inode_st;
	uint32_t index;
	block* data;
} dir_ptr;

typedef struct {
	iptr inode;
} stat_st;

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


// holds values related to a virtual file system file
typedef struct {

	uint8_t state;
	superblock *superblk;
	uint8_t *free_inodes;
	uint8_t *free_blocks;

} vfs;

//********FS state and cache********

vfs fs;
block superblk_cache;
block block_bm_cache;
block inode_bm_cache;

fd_entry fd_tbl[MAX_FD];

iptr cwd_iptr;
char cwd_str[4096];

//*****************mount****************
int8_t cnmount(void)
{
	blk_read(BLOCKID_SUPER, &superblk_cache);
	blk_read(BLOCKID_BLOCK_BITMAP, &block_bm_cache);
	blk_read(BLOCKID_INODE_BITMAP, &inode_bm_cache);

	fs.superblk = (superblock*)&superblk_cache;
	if(fs.superblk->magic == FS_MAGIC && fs.superblk->state == VALID_FS) {
		fs.state = VFS_GOOD;
		fs.superblk->state = ERROR_FS;
	} else
	{
		fs.state = VFS_BLANK;
	}
	memset(fd_tbl, 0, sizeof(fd_entry)*1024);
	return 0;
}

//*****************umount****************
int8_t cnumount(void)
{
	fs.superblk->state = VALID_FS;
	blk_write(BLOCKID_SUPER, &superblk_cache);
	blk_write(BLOCKID_BLOCK_BITMAP, &block_bm_cache);
	blk_write(BLOCKID_INODE_BITMAP, &inode_bm_cache);
	return 0;
}



//*****************mkfs****************
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
	root_i.blocks = 1;
	root_i.data0[0] = BLOCKID_ROOT_DIR;

	block* root_dir_block = malloc(sizeof(block));

	// . (self entry)
	dir_entry* root_dir_entry = (dir_entry*)root_dir_block;
	root_dir_entry->inode = 0;
	root_dir_entry->file_type = ITYPE_DIR;
	root_dir_entry->name_len = 1;
	root_dir_entry->entry_len = 12;
	memcpy(root_dir_entry->name, ".", 1);
	root_i.size += 12;

	// .. (parent entry, also itself)
	root_dir_entry = (dir_entry*)(((char*)root_dir_block) + 12);
	root_dir_entry->inode = 0;
	root_dir_entry->file_type = ITYPE_DIR;
	root_dir_entry->name_len = 2;
	root_dir_entry->entry_len = 12;
	memcpy(root_dir_entry->name, "..", 2);
	root_i.size += 12;

	inode_write(0, &root_i);
	blk_write(BLOCKID_ROOT_DIR, root_dir_block);

	free(root_dir_block);
}

int8_t cnmkfs(void)
{
	superblock_init();
	block_bitmap_init();
	inode_bitmap_init();
	write_root_dir();
	return 0;
}
//******** end mkfs *****************

//******** llread *******************
//Reads a complete file from its inode data
int8_t llread(inode* inode_ptr, block* buf)
{
	if(inode_ptr->blocks <= 8)
	{
		for(uint8_t i = 0; i < inode_ptr->blocks; i++)
		{
			blk_read(inode_ptr->data0[i], buf);
			buf++;
		}
	}
	else
	{
		//TODO: read indirect blocks
	}
	return 0;
}

//******** llwrite ******************
//Writes a file completely and updates inode data
int8_t llwrite(inode* inode_ptr, block* buf)
{
	if(inode_ptr->blocks <= 8)
	{
		for(uint8_t i = 0; i < inode_ptr->blocks; i++)
		{
			blk_write(inode_ptr->data0[i], buf);
			buf++;
		}
	}
	else
	{
		//TODO: read indirect blocks
	}
	return 0;
}

//******** readdir ******************
dir_entry* cnreaddir(dir_ptr* dir)
{
	dir_ptr* dir_indexed = (dir_ptr*)(((uint8_t*)dir)+dir->index);
	dir_entry* entry = (dir_entry*)dir_indexed->data;
	dir->index += entry->entry_len;
	if(dir->index >= dir->inode_st.size)      //Reached the end of the directory file
	{
		return NULL;
	}
	return entry;
}

//******** opendir ******************
dir_ptr* cnopendir(char* name)
{
	dir_ptr *dir = malloc(sizeof(dir_ptr));		//Directory file in memory (e.g. DIR object from filedef.h)

	if(memcmp(name,"/",1) == 0)					//Is this path absolute or relative
	{
		inode_read(INODE_ROOTDIR, &dir->inode_st);   //Start at the root dir
	}
	else
	{
		inode_read(cwd_iptr, &dir->inode_st);		//Start at cwd
	}

	char* name_tok = strtok(name, "/");
	dir_entry* entry;
	do
	{
		//Read the directory file for this inode
		dir->data = malloc(sizeof(block)*(dir->inode_st.blocks));  	//Memory for all directory file blocks
		llread(&dir->inode_st, dir->data);	//Read the directory file
		dir->index = 0;

		name_tok = strtok(NULL, "/");		//Read the next token
		if(name_tok == NULL)   //This is the last directory in the path
		{
			return dir;		   //Path was traversed, return the dir_ptr
		}

		//Find the token in this dir
		while((entry = cnreaddir(dir)))
		{
			if(memcmp(entry->name, name_tok, entry->name_len) == 0)  //If this is the directory we want
			{
				inode_read(entry->inode,&dir->inode_st);   //Read the next directory's inode
				free(dir->data);  //Forget the directory we just read
				break;
			}
		}
	} while(1);
}



//******** closedir *****************
void cnclosedir(dir_ptr* dir)
{
	free(dir->data);
	free(dir);
}

//******** stat *********************
//int8_t stat(char* path, stat_st *buf)
//{
//	inode cwd_in;
//	inode_read(cwd, &cwd_in);
//}
//******** end stat *****************


//******** cnopen *********************
//int8_t cnopen(char* filename, uint8_t mode){

//}
//******** end open *****************




