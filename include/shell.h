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
#define SH_CMD_MKFS			10
#define SH_CMD_OPEN			11
#define SH_CMD_PWD			12
#define SH_CMD_READ			13
#define SH_CMD_RM			14
#define SH_CMD_RMDIR		15
#define SH_CMD_SEEK			16
#define SH_CMD_TREE			17
#define SH_CMD_WRITE		18


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
#define SH_ERR_RECV			-12
#define SH_ERR_STARTSOCK	-13
#define SH_ERR_BINDSOCK		-14
#define SH_ERR_RCONNECT		-15
#define SH_ERR_RCLOSE  		-16
#define SH_ERR_LISTEN		-17
#define SH_ERR_CRECV		-18


#define SH_CMD_NUM			19
#define SH_MAX_ARGS			16
#define SH_MAX_STR			256

#define STR_PROMPT			0
#define STR_CMDS			1
#define STR_EXIT			2
#define STR_DEFAULT_PORT	3
#define STR_REMOTE_PROMPT   4
#define STR_SUCCESS_MKFS	5
#define STR_SUCCESS_OPEN	6
#define STR_SUCCESS_READ	7
#define STR_SUCCESS_WRITE	8
#define STR_SUCCESS_SEEK	9
#define STR_SUCCESS_CLOSE	10
#define STR_SUCCESS_MKDIR	11
#define STR_SUCCESS_RMDIR	12
#define STR_SUCCESS_CD		13
#define STR_SUCCESS_LS		14
#define STR_SUCCESS_CAT		15
#define STR_SUCCESS_TREE	16
#define STR_SUCCESS_IMPORT	17
#define STR_SUCCESS_EXPORT	18
#define STR_SUCCESS_CONNECT	19
#define STR_SUCCESS_REXIT	20

#define STR_TYPE_STR		1
#define STR_TYPE_HELP		0
#define STR_TYPE_ERR		-1

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
