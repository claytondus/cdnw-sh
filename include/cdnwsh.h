/*
 * cdnwsh.h
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#ifndef INCLUDE_CDNWSH_H_
#define INCLUDE_CDNWSH_H_

// library includes here

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <glib/gstdio.h>
#include <glib.h>

// local includes here

#include "client.h"
#include "server.h"
#include "shell.h"
#include "fs.h"


// function prototypes here
void mkfs(void);

// fs.c
struct block* blk_read(uint32_t);
sh_err blk_write(uint32_t,struct block*);
sh_err chk_vfs(struct vfs*);

int16_t cnopen(char *, uint8_t);
char* cnread(int16_t, uint32_t);
sh_err cnwrite(int16_t, char*);
sh_err cnseek(int16_t, int32_t);
sh_err cnclose(int16_t);
sh_err cnmkdir(char*);
sh_err cnrmdir(char*);
sh_err cnrm(char*);
sh_err cncd(char*);
char* cnls(void);
char* cncat(char*);
char* cntree(void);
sh_err cnimport(char*,char*);
sh_err cnexport(char*,char*);


//server.c
in_port_t start_listening(void);
char* recv_cmd(void);
sh_err send_results(char*);
sh_err run(void);					// loop to check sockets sets (fd_set)

//client.c
int cnconnect(struct sockaddr_in);
sh_err send_cmd(char*);
char* recv_results(void);

// shell.c
char* prompt(void);
char* run_cmd(char*);

char* sh_exit(int, char*[]);
char* sh_open(int, char*[]);
char* sh_read(int, char*[]);
char* sh_write(int, char*[]);
char* sh_seek(int, char*[]);
char* sh_close(int, char*[]);
char* sh_mkdir(int, char*[]);
char* sh_rmdir(int, char*[]);
char* sh_cd(int, char*[]);
char* sh_ls(int, char*[]);
char* sh_cat(int, char*[]);
char* sh_tree(int, char*[]);
char* sh_import(int, char*[]);
char* sh_export(int, char*[]);
char* sh_help(int, char*[]);
char* sh_connect(int, char*[]);
char* sh_rm(int, char*[]);




#endif /* INCLUDE_CDNWSH_H_ */
