/*
 * shell.c
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#include <cdnwsh.h>

const char *str_table[] = {
		"\ncdnw-shell> \0",
		"Available commands: cat cd close connect exit export help import ls mkdir mkfs open read rm rmdir seek tree write\0",
		"Exiting...\0",
		"5560\0",
		"\nremote-cdnw> \0"
};

const char *err_strings[] = {
		"SUCCESS\0",
		"ERROR: unknown problem\0",
		"ERROR: file or folder not found\0",
		"ERROR: end of file\0",
		"ERROR: \0",
		"ERROR: insufficient diskspace to complete this operation\0",
		"ERROR: insufficient inodes to complete this operation\0",
		"ERROR: invalid virtual file system, unable to mount\0",
		"ERROR: invalid operation, no virtual file system has been mounted\0",
		"ERROR: invalid parameters, unable to run this command\0",
		"ERROR: command not found\0",
		"ERROR: socket - unable to run select()\0",
		"ERROR: unable to receive command, network error on recv()\0",
		"ERROR: unable to allocate network socket\0",
		"ERROR: unable to bind to socket\0",
		"ERROR: problem connecting to server \0",
		"ERROR: problem disconnecting from server, assuming connection closed \0"
};

struct cmd_entry sh_cmds[] = {
		{"cat\0",sh_cat,"Usage: cat <filename>\0"},
		{"cd\0",sh_cd,"Usage: cd <dir_name>\0"},
		{"close\0",sh_close,"Usage: close <fd>\n"
				"fd = file descriptor\0"},
		{"connect\0",sh_connect,"Usage: connect <server_ip/name> <server_port>\0"},
		{"exit\0",sh_exit,"Usage: exit\nIf used at a remote server prompt, "
				"EXIT will disconnect the client connection.\nIf used at a local prompt, "
				"EXIT will close the shell and stop the client or server\0"},
		{"export\0",sh_export,"Usage: export <internal_filename> <external_filename>\0"},
		{"help\0",sh_help,"Usage: 'help [<cmd>]' or '<cmd> --help'\0"},
		{"import\0",sh_import,"Usage: import <external_filename> <internal_filename>\0"},
		{"ls\0",sh_ls,"Usage: ls\0"},
		{"mkdir\0",sh_mkdir,"Usage: mkdir <dir_name>\0"},
		{"mkfs\0",sh_mkfs,"Usage: mkfs\n Check and mount the virtual file system\0"},
		{"open\0",sh_open,"Usage: open <filename> <R/W>\n"
				"OPEN initializes access to a file for reading or writing.\n"
				"In the second parameter, 1 = read and 2 = write\0"},
		{"read\0",sh_read,"Usage: read <fd> <num_of_bytes>\0"},
		{"rm\0",sh_rm,"Usage: rm <filename>\0"},
		{"rmdir\0",sh_rmdir,"Usage: rmdir <dir name>\0"},
		{"seek\0",sh_seek,"Usage: seek <fd> <byte_offset>\n"
				"A negative byte offset will move back instead of forward in the file\0"},
		{"tree\0",sh_tree,"Usage: tree\0"},
		{"write\0",sh_write,"Usage: write <fd> <string>\0"}
};

// incomplete function
// need to make case insensitive
// need quote handling
char* run_cmd(char *cmdstr) {
	char *cmd_argv[SH_MAX_ARGS];
	char *cmd_tok=NULL;
	int cmd_argc = 0;
	char *result=NULL;
	int last_stop = 0;

	int cmdstr_len = strlen(cmdstr);
	for(int i = 0; i<cmdstr_len; i++) {
		char c = cmdstr[i];
		if (c == '\"' || c == '\'') {
			// quote handling
		} else if (c == ' ' || c == '\n' || c == '\r'){
			if(cmd_tok) {
				cmd_argv[cmd_argc] = malloc(sizeof(char)*(i-last_stop));
				strncpy(cmd_argv[cmd_argc],cmdstr+last_stop+1,(i-last_stop-1));
				(cmd_argv[cmd_argc])[i-last_stop] = '\0';

				cmd_argc++;
			} else {
				cmd_tok = malloc(sizeof(char)*(i+1));
				strncpy(cmd_tok,cmdstr,i);
				cmd_tok[i] = '\0';
			}
			last_stop = i;
		}
		if(c=='\n'||c=='\r') {
			break;
		}

	}

	if(cmd_tok && cmd_tok[0] && strncmp(cmd_tok,"\n",1) && strncmp(cmd_tok,"\r",1) && strncmp(cmd_tok," ",1)) {
		int i;
		for(i=0; i<SH_CMD_NUM; i++) {
			if(!strcmp(cmd_tok,sh_cmds[i].name)) {
				result = sh_cmds[i].sh_cmd(cmd_argc, cmd_argv);
				break;
			}
		}
		if(!result) {
			result = malloc(sizeof(char)*(strlen(err_str(SH_ERR_BADCMD))+1));
			strcpy(result,err_str(SH_ERR_BADCMD));
		}
	} else {
		result = NULL;
	}

	return result;
}

char* sh_exit(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;
	(void)(cmd_argc);
	(void)(cmd_argv);
	if(shell_server.client == CLIENT_STATUS_OPEN) {
		cmd_err = rclose();
		if(cmd_err<0) {
			char* str_errno = strerror(errno);
			result = malloc(sizeof(char)*(strlen(err_str(SH_ERR_RCLOSE))+strlen(str_errno)+2));
			strcpy(result, err_str(SH_ERR_RCLOSE));
			strcat(result, "\n");
			strcat(result, str_errno);
		} else {

			result = malloc(sizeof(char)*(strlen("Connection to server closed\0")+2));
			strcpy(result, "Connection to server closed\0");
		}
	} else {
		shell_server.status = SVR_STATUS_EXIT;
		result = malloc(sizeof(char)*strlen(str_table[STR_EXIT]));
		strcpy(result, str_table[STR_EXIT]);
	}

	return result;
}


char* sh_mkfs(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);
	cnmkfs();
	return result;
}

char* sh_open(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);
	return result;
}

char* sh_close(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);
	return result;
}

char* sh_read(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);
	return result;
}

char* sh_write(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);

	return result;
}

char* sh_seek(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);

	return result;
}

char* sh_mkdir(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);

	return result;
}

char* sh_rmdir(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);

	return result;
}

char* sh_rm(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);

	return result;
}

char* sh_cat(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);

	return result;
}

char* sh_cd(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);

	return result;
}

char* sh_ls(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);

	return result;
}

char* sh_tree(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);

	return result;
}

char* sh_import(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);

	return result;
}

char* sh_export(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	(void)(cmd_argc);
	(void)(cmd_argv);

	return result;
}

char* sh_connect(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(cmd_argc != 2) {
		result = malloc(sizeof(char)*(strlen(sh_cmds[SH_CMD_CONNECT].help)+2));
		strcpy(result, sh_cmds[SH_CMD_CONNECT].help);
	} else {
		cmd_err = rconnect(cmd_argv[0],cmd_argv[1]);
		if(cmd_err>=0) {
			result = malloc(sizeof(char)*(strlen("Connected to server: ")+strlen(cmd_argv[0])+2));
			strcpy(result, "Connected to server: ");
			strcat(result, cmd_argv[0]);
		} else {
			char* str_errno = strerror(errno);
			result = malloc(sizeof(char)*(strlen(err_str(SH_ERR_RCONNECT))+strlen(cmd_argv[0])+strlen(str_errno)+2));
			strcpy(result, err_str(SH_ERR_RCONNECT));
			strcat(result, cmd_argv[0]);
			strcat(result, "\n");
			strcat(result, str_errno);
		}
	}

	return result;
}

char* sh_help(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;

	if(cmd_argc) {
		int i;
		for(i=0; i<SH_CMD_NUM; i++) {
			if(!strcmp(cmd_argv[0],sh_cmds[i].name)) {
				result = malloc(sizeof(char)*(strlen(sh_cmds[i].help)));
				strcpy(result, sh_cmds[i].help);
				break;
			}
		}
		if(!result) {
			result = malloc(sizeof(char)*(strlen(err_str(SH_ERR_BADCMD))+2));
			strcpy(result,err_str(SH_ERR_BADCMD));
		}
	} else {
		result = malloc(sizeof(char)*(strlen(sh_cmds[SH_CMD_HELP].help)+strlen(str_table[STR_CMDS])+2));
		strcpy(result, sh_cmds[SH_CMD_HELP].help);
		strcat(result, "\n");
		strcat(result, str_table[STR_CMDS]);
	}

	return result;
}

