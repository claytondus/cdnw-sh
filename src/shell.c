/*
 * shell.c
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#include <cdnwsh.h>

const char *str_table[] = {
		"\ncdnw-shell> \0",
		"ERROR: command not found\0"
};

struct cmd_entry sh_cmds[] = {
		{"exit\0",sh_exit,"Usage: exit\nIf used at a remote server prompt, "
				"EXIT will disconnect the client connection.\nIf used at a local prompt, "
				"EXIT will close the shell and stop the client or server\0"},
		{"open\0",sh_open,"Usage: open <filename> <R/W>\n"
				"OPEN initializes access to a file for reading or writing.\n"
				"In the second parameter, 1 = read and 2 = write\0"},
		{"close\0",sh_close,"Usage: close <fd>\n"
				"fd = file descriptor\0"},
		{"read\0",sh_read,"Usage: read <fd> <num_of_bytes>\0"},
		{"write\0",sh_write,"Usage: write <fd> <string>\0"},
		{"seek\0",sh_seek,"Usage: seek <fd> <byte_offset>\n"
				"A negative byte offset will move back instead of forward in the file\0"},
		{"mkdir\0",sh_mkdir,"Usage: mkdir <dir_name>\0"},
		{"rmdir\0",sh_rmdir,"Usage: rmdir <dir name>\0"},
		{"rm\0",sh_rm,"Usage: rm <filename>\0"},
		{"cd\0",sh_cd,"Usage: cd <dir_name>\0"},
		{"ls\0",sh_ls,"Usage: ls\0"},
		{"tree\0",sh_tree,"Usage: tree\0"},
		{"cat\0",sh_cat,"Usage: cat <filename>\0"},
		{"import\0",sh_import,"Usage: import <external_filename> <internal_filename>\0"},
		{"export\0",sh_export,"Usage: export <internal_filename> <external_filename>\0"},
		{"connect\0",sh_connect,"Usage: connect <server_ip> <server_port>\0"},
		{"help\0",sh_help,"Usage: help [<cmd>], <cmd> --help\0"}
};

// incomplete function
// need to make case insensitive
// need quote handling
char* run_cmd(char *cmdstr) {
	char *cmd_argc[SH_MAX_ARGS];
	char *cmd_tok=NULL;
	int cmd_argv = 0;
	char *result=NULL;
	int last_stop = 0;

	int cmdstr_len = strlen(cmdstr);
	for(int i = 0; i<cmdstr_len; i++) {
		char c = cmdstr[i];
		if (c == '\"' || c == '\'') {
			// quote handling
		} else if (c == ' ' || c == '\n' || c == '\r'){
			if(cmd_tok) {
				cmd_argc[cmd_argv] = malloc(sizeof(char)*(i-last_stop));
				strncpy(cmd_argc[cmd_argv],cmdstr+last_stop+1,(i-last_stop-1));
				(cmd_argc[cmd_argv])[i-last_stop] = '\0';

				cmd_argv++;
			} else {
				cmd_tok = malloc(sizeof(char)*(i+1));
				strncpy(cmd_tok,cmdstr,i);
				cmd_tok[i+1] = '\0';
			}
			last_stop = i;
		}
		if(c=='\n'||c=='\r') {
			break;
		}

	}

	if(cmd_tok) {
		int i;
		for(i=0; i<SH_CMD_NUM; i++) {
			if(!strcmp(cmd_tok,sh_cmds[i].name)) {
				result = sh_cmds[i].sh_cmd(cmd_argv, cmd_argc);
				break;
			}
		}
		if(i==SH_CMD_NUM) {
			result = malloc(sizeof(char)*(strlen(str_table[STR_ERR_BADCMD])+strlen(str_table[STR_PROMPT])+1));
			strcpy(result,str_table[STR_ERR_BADCMD]);
		}
		strcat(result,str_table[STR_PROMPT]);
	} else {
		result = malloc(sizeof(char)*(strlen(str_table[STR_PROMPT])+1));
		strcpy(result,str_table[STR_PROMPT]);
	}

	return result;
}

char* sh_exit(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_open(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_close(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_read(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_write(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_seek(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_mkdir(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_rmdir(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_rm(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_cat(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_cd(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_ls(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_tree(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_import(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_export(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_connect(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}

char* sh_help(int cmd_argv, char* cmd_argc) {
	char* result = NULL;


	return result;
}



