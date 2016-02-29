/*
 * fs.h
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#ifndef INCLUDE_FS_H_
#define INCLUDE_FS_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include "fsparams.h"
#include "inode.h"
#include "block.h"
#include "blockdev.h"

#define ITYPE_FILE 		0
#define ITYPE_DIR		1

#define MAX_FD			1024

#define FD_FREE 	0
#define FD_READ		1
#define FD_WRITE	2

typedef struct {
	block* data;
	uint8_t state;
	inode inode;
	uint32_t cursor;
} fd_entry;

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
	iptr inode_id;
} stat_st;


int8_t cnmkfs(void);
int8_t cnmount(void);
int8_t cnumount(void);
int8_t cncreat(dir_ptr*, const char*);
int8_t cnstat(dir_ptr* dir, const char* name, stat_st *buf);
int16_t cnopen(dir_ptr*, const char *, uint8_t);
size_t cnread(uint8_t*, size_t, int16_t);
size_t cnwrite(uint8_t*, size_t, int16_t);
int8_t cnseek(int16_t, uint32_t);
int8_t cnclose(int16_t);
dir_ptr* cnopendir(const char* name);
void cnclosedir(dir_ptr* dir);
dir_entry* cnreaddir(dir_ptr* dir);
int8_t cnmkdir(const char*);
int8_t cnrmdir(char*);
int8_t cncd(const char*);
int8_t cnpwd(char*);
int8_t cnls(const char *, char*);
char* cncat(char*);
char* cntree(void);
int8_t cnimport(char*,char*);
int8_t cnexport(char*,char*);



#endif /* INCLUDE_FS_H_ */
