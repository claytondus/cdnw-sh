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
	uint8_t state;
	int16_t fd;
	inode inode;
	uint32_t cursor;
} fd_entry;


int8_t cnmkfs(void);
int8_t cnmount(void);
int8_t cnumount(void);
int16_t cnopen(char *, uint8_t);
char* cnread(int16_t, uint32_t);
int8_t cnwrite(int16_t, char*);
int8_t cnseek(int16_t, int32_t);
int8_t cnclose(int16_t);
int8_t cnmkdir(const char*);
int8_t cnrmdir(char*);
int8_t cncd(const char*);
int8_t cnpwd(char*);
char* cnls(void);
char* cncat(char*);
char* cntree(void);
int8_t cnimport(char*,char*);
int8_t cnexport(char*,char*);



#endif /* INCLUDE_FS_H_ */
