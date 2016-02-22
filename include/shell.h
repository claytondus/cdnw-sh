/*
 * shell.h
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#ifndef INCLUDE_SHELL_H_
#define INCLUDE_SHELL_H_

#define SH_CMD_CAT			0
#define SH_CMD_CD			1
#define SH_CMD_CLOSE		2
#define SH_CMD_CONNECT		3
#define SH_CMD_EXIT			4
#define SH_CMD_EXPORT		5
#define SH_CMD_HELP			6
#define SH_CMD_IMPORT		7
#define SH_CMD_LS			8
#define SH_CMD_MKDIR		9
#define SH_CMD_OPEN			10
#define SH_CMD_READ			11
#define SH_CMD_RM			12
#define SH_CMD_RMDIR		13
#define SH_CMD_SEEK			14
#define SH_CMD_TREE			15
#define SH_CMD_WRITE		16


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
#define SH_ERR_SOCKET		-11

#define SH_CMD_NUM			17
#define SH_MAX_ARGS			16
#define SH_MAX_STR			256

#define STR_PROMPT			0
#define STR_CMDS			1
#define STR_EXIT			2

extern const char *err_strings[];

#define err_str(x)			(err_strings[(x)*(-1)])
#define help_str(x)			(sh_cmds[x].help)

struct cmd_entry {
	const char *name;
	char* (*sh_cmd)(int,char*[]);
	const char *help;
};

extern const char *str_table[];

#endif /* INCLUDE_SHELL_H_ */
