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
	iptr inode_id;
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




//********Private*****************
dir_ptr* cnopendir(const char* name);
void cnclosedir(dir_ptr* dir);
dir_entry* cnreaddir(dir_ptr* dir);



//********FS state and cache********

vfs fs;
block superblk_cache;
block block_bm_cache;
block inode_bm_cache;

fd_entry fd_tbl[MAX_FD];

char cwd_str[4096];
dir_ptr* cwd;


//************flush_metadata************
void flush_metadata(void)
{
	blk_write(BLOCKID_SUPER, &superblk_cache);
	blk_write(BLOCKID_BLOCK_BITMAP, &block_bm_cache);
	blk_write(BLOCKID_INODE_BITMAP, &inode_bm_cache);
}

//*************reserve_inode************
iptr reserve_inode(void)
{
	superblock* super = (superblock*)&superblk_cache;
	if(super->free_inode_count == 0)
	{
		return 0;
	}
	iptr inode_ptr = find_free_bit(&inode_bm_cache);
	set_bitmap(&inode_bm_cache, inode_ptr);
	super->free_inode_count--;
	flush_metadata();
	return inode_ptr;
}

//**************release_inode***********
void release_inode(iptr inode_ptr)
{
	superblock* super = (superblock*)&superblk_cache;
	clear_bitmap(&inode_bm_cache, inode_ptr);
	super->free_inode_count++;
	flush_metadata();
}

//*************reserve_block************
iptr reserve_block(void)
{
	superblock* super = (superblock*)&superblk_cache;
	if(super->free_block_count == 0)
	{
		return 0;
	}
	iptr blockid = find_free_bit(&block_bm_cache);
	set_bitmap(&block_bm_cache, blockid);
	super->free_block_count--;
	flush_metadata();
	return blockid;
}

//**************release_block***********
void release_block(iptr blockid)
{
	superblock* super = (superblock*)&superblk_cache;
	clear_bitmap(&block_bm_cache, blockid);
	super->free_block_count++;
	flush_metadata();
}

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
	strcpy(cwd_str,"/");
	cwd = cnopendir("/");
	return 0;
}

//*****************umount****************
int8_t cnumount(void)
{
	cnclosedir(cwd);
	fs.superblk->state = VALID_FS;
	blk_write(BLOCKID_SUPER, &superblk_cache);
	blk_write(BLOCKID_BLOCK_BITMAP, &block_bm_cache);
	blk_write(BLOCKID_INODE_BITMAP, &inode_bm_cache);
	return 0;
}



//*****************mkfs****************
void superblock_init(void)
{
	superblock *sb = calloc(1,sizeof(block));
	memset(sb, 0, sizeof(block));
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
	block *block_btm = calloc(1,sizeof(block));

	//Mark all reserved blocks as used
	set_bitmap(block_btm, BLOCKID_SUPER);
	set_bitmap(block_btm, BLOCKID_BLOCK_BITMAP);
	set_bitmap(block_btm, BLOCKID_INODE_BITMAP);

	//Mark all inode table blocks as used
	for (uint16_t i = BLOCKID_INODE_TABLE; i < INODE_TABLE_BLOCKS + BLOCKID_INODE_TABLE; i++) {
		set_bitmap(block_btm, i);
	}

	//Mark root directory block as used
	set_bitmap(block_btm, INODE_TABLE_BLOCKS + BLOCKID_INODE_TABLE);

	blk_write(BLOCKID_BLOCK_BITMAP, block_btm);
	free(block_btm);
}

void inode_bitmap_init(void)
{
	block *inode_btm = calloc(1,sizeof(block));

	//Mark first inode as used
	set_bitmap(inode_btm, 0);

	blk_write(BLOCKID_INODE_BITMAP, inode_btm);
	free(inode_btm);
}

void write_root_dir(void)
{
	//Prepare inode
	inode root_i;
	uint32_t now = time(NULL);
	root_i.created = now;
	root_i.modified = now;
	root_i.type = ITYPE_DIR;
	root_i.size = 0;
	root_i.blocks = 1;
	root_i.data0[0] = BLOCKID_ROOT_DIR;

	block* root_dir_block = calloc(1,sizeof(block));

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
//Return the dir_entry at the index within dir_ptr, and increment by entry_len.
dir_entry* cnreaddir(dir_ptr* dir)
{
	if(dir->index >= dir->inode_st.size)      //Reached the end of the directory file
	{
		return NULL;
	}
	uint8_t* entry_8 = (uint8_t*)dir->data;
	entry_8 += dir->index;
	dir_entry* entry = (dir_entry*)entry_8;
	dir->index += entry->entry_len;
	return entry;
}

//******** rewinddir *****************
void cnrewinddir(dir_ptr* dir)
{
	dir->index = 0;
}

//******** inflatedir *****************
//Populates a dir_ptr from an iptr
void inflatedir(dir_ptr* dir, iptr inode_id)
{
	inode_read(inode_id,&dir->inode_st);
	dir->inode_id = inode_id;
	//Read the directory file for this inode
	dir->data = calloc(dir->inode_st.blocks, sizeof(block));  	//Memory for all directory file blocks
	llread(&dir->inode_st, dir->data);	//Read the directory file
	dir->index = 0;
}


//******** opendir ******************
dir_ptr* cnopendir(const char* name)
{
	char* name_copy = strdup(name);
	char* name_tok;
	char entry_name[256];
	iptr loaded_inode = INODE_COUNT + 1;		//Invalid sentinel
	dir_entry* entry;
	dir_ptr *dir = calloc(1,sizeof(dir_ptr));	//Directory file in memory (e.g. DIR object from filedef.h)

	if(memcmp(name,"/",1) == 0)					//Is this path absolute or relative
	{
		//Start at the root dir
		inflatedir(dir, INODE_ROOTDIR);
	}
	else
	{
		//Start at cwd
		inflatedir(dir, cwd->inode_id);
	}

	if(strcmp(name,"/") == 0)  //Root is a special case, we are done now
	{
		goto done;
	}

	name_tok = strtok(name_copy, "/");   //Guaranteed not to return NULL
	do
	{
		//Find the token in this dir
		while((entry = cnreaddir(dir)))
		{
			//Copy entry name to a null-term string to compare it
			memcpy(entry_name, entry->name, entry->name_len);
			entry_name[entry->name_len] = 0;
			if(strcmp(entry_name, name_tok) == 0)  //If this is the directory we want
			{
				loaded_inode = entry->inode;
				dir_ptr* new_dir = calloc(1, sizeof(dir_ptr));
				inflatedir(new_dir, entry->inode);
				cnclosedir(dir);  //Forget the directory we already read
				dir = new_dir;
				break;
			}
		}
		check(dir->inode_id == loaded_inode, "can not find directory %s", name_copy);

		name_tok = strtok(NULL, "/");		//Read the next token

	} while(name_tok != NULL);

done:
	free(name_copy);
	return dir;

error:
	free(dir);
	return NULL;
}



//******** closedir *****************
void cnclosedir(dir_ptr* dir)
{
	if(dir == NULL) return;
	if(dir->data == NULL) return;
	free(dir->data);
	free(dir);
}

//******** cd ************************
int8_t cncd(const char* name)
{
	dir_ptr* new_cwd = cnopendir(name);
	check(new_cwd != NULL, "directory %s does not exist", name);
	strcpy(cwd_str,name);
	cnclosedir(cwd);
	cwd = new_cwd;
	return 0;
error:
	return -1;
}

//******** pwd **********************
int8_t cnpwd(char* buf)
{
	strcpy(buf,cwd_str);
	return 0;
}

//******** mkdir ********************
int8_t cnmkdir(const char* name)
{
	char* name_copy = strdup(name);
	char name_tok[256];
	char entry_name[256];
	char* next_name_tok;
	dir_entry* entry;
	dir_ptr *dir = calloc(1,sizeof(dir_ptr));		//Directory file in memory (e.g. DIR object from filedef.h)
	bool last_dir = false;

	inode_read(cwd->inode_id, &dir->inode_st);		//Start at cwd
	dir->inode_id = cwd->inode_id;

	next_name_tok = strtok(name_copy, "/");
	do
	{
		//name_tok is the dir we are searching for or going to create
		strcpy(name_tok, next_name_tok);

		//Read the directory file for this inode
		dir->data = calloc(dir->inode_st.blocks,sizeof(block));  	//Memory for all directory file blocks
		llread(&dir->inode_st, dir->data);	//Read the directory file
		dir->index = 0;

		next_name_tok = strtok(NULL, "/");		//Read the next token
		if(next_name_tok == NULL)   //This is the last directory in the path
		{
			last_dir = true;
		}

		//Find the token in this dir
		while((entry = cnreaddir(dir)))
		{
			memcpy(entry_name, entry->name, entry->name_len);
			entry_name[entry->name_len] = 0;
			if(strcmp(entry_name, name_tok) == 0)  //If this directory already exists
			{
				if(last_dir)
				{
					return -1;   //Directory already exists
				}
				else   //Read the directory inode
				{
					dir->inode_id = entry->inode;
					inode_read(entry->inode,&dir->inode_st);   //Read the next directory's inode
					free(dir->data);  //Forget the directory we just read
					break;
				}
			}
		}

		if(last_dir)  //Create the directory at the end of the list
		{
			//Create parent directory entry
			entry = (dir_entry*)(((uint8_t*)dir->data)+dir->index);
			entry->file_type = ITYPE_DIR;
			entry->inode = reserve_inode();
			memcpy(entry->name,name_tok,strlen(name_tok));
			entry->name_len = strlen(name_tok);
			entry->entry_len = entry->name_len + 8;
			entry->entry_len += (4 - entry->entry_len % 4);  //padding out to 32 bits
			dir->inode_st.size += entry->entry_len;
			//TODO: handle mkdir block overflow

			//Write parent dir and inode
			inode_write(dir->inode_id, &dir->inode_st);
			blk_write(dir->inode_st.data0[0], dir->data);

			//Write new directory inode
			inode new_dir_i;
			uint32_t now = time(NULL);
			new_dir_i.created = now;
			new_dir_i.modified = now;
			new_dir_i.type = ITYPE_DIR;
			new_dir_i.size = 0;
			new_dir_i.blocks = 1;
			new_dir_i.data0[0] = reserve_block();

			//Write new directory file
			block* new_dir_block = calloc(1,sizeof(block));

			// . (self entry)
			dir_entry* new_dir_self_entry = (dir_entry*)new_dir_block;
			new_dir_self_entry->inode = entry->inode;
			new_dir_self_entry->file_type = ITYPE_DIR;
			new_dir_self_entry->name_len = 1;
			new_dir_self_entry->entry_len = 12;
			memcpy(new_dir_self_entry->name, ".", 1);
			new_dir_i.size += 12;

			// .. (parent entry)
			dir_entry* new_dir_parent_entry = (dir_entry*)(((char*)new_dir_block) + 12);
			new_dir_parent_entry->inode = dir->inode_id;
			new_dir_parent_entry->file_type = ITYPE_DIR;
			new_dir_parent_entry->name_len = 2;
			new_dir_parent_entry->entry_len = 12;
			memcpy(new_dir_parent_entry->name, "..", 2);
			new_dir_i.size += 12;

			//Write new dir and inode
			inode_write(entry->inode, &new_dir_i);
			blk_write(new_dir_i.data0[0], new_dir_block);

			free(new_dir_block);
			break;
		}

	} while(1);
	free(dir);
	free(name_copy);
	return 0;
}

//******** ls ***********************
int8_t cnls(const char* name, char* buf)
{
	char name_copy[256];
	if(strlen(name) == 0)
	{
		strcpy(name_copy, ".");
	}
	else
	{
		strcpy(name_copy, name);
	}
	dir_entry* entry;
	dir_ptr* dir = cnopendir(name_copy);
	check(dir !=NULL, "can not ls directory %s", name);
	while((entry = cnreaddir(dir)))
	{
		strncat(buf, entry->name, entry->name_len);
		strcat(buf, "\n");
	}
	return 0;
error:
	return -1;
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




