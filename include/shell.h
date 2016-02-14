/*
 * shell.h
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#ifndef INCLUDE_SHELL_H_
#define INCLUDE_SHELL_H_

typedef int8_t sh_err;

#define SH_ERR_SUCCESS		0
#define SH_ERR_UNK			-1
#define SH_ERR_NOTFOUND		-2
#define SH_ERR_EOF			-3
#define SH_ERR_FILESIZE		-4
#define SH_ERR_DISKFULL		-5
#define SH_ERR_INODESFULL	-6
#define SH_ERR_BADVFS		-7
#define SH_ERR_NOVFS		-8
#define SH_ERR_BADARGS		-9
#define SH_ERR_BADCMD		-10

#define SH_CMD_NUM			17
#define SH_MAX_ARGS			16

#define STR_PROMPT			0
#define STR_ERR_BADCMD		1



struct cmd_entry {
	char *name;
	char* (*sh_cmd)(int,char*);
	char *help;
};

#endif /* INCLUDE_SHELL_H_ */
