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


// local includes here

#include "client.h"
#include "server.h"
#include "shell.h"
#include "fs.h"


// function prototypes here

//server.c
sh_err start_listening(void);
char* recv_cmd(void);
sh_err send_results(char*);
sh_err run(void);					// loop to check sockets sets (fd_set)
void clean_svr(void);

//client.c
sh_err rconnect(char*,char*);
sh_err send_cmd(char*);
sh_err rclose(void);
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
char* sh_mkfs(int, char*[]);
char* sh_pwd(int, char*[]);
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
sh_err chk_vfs(char**);



#endif /* INCLUDE_CDNWSH_H_ */
