
#include <cdnwsh.h>
#include <math.h>

const char *str_table[] = {
		"\ncdnw-shell> \0",
		"Available commands: cat cd close connect exit export help import ls mkdir mkfs open read rm rmdir seek tree write\0",
		"Exiting...\0",
		"5560\0",
		"\nremote-cdnw> \0",
		"Virtual file system initialized and mounted.\0",
		"SUCCESS, fd = \0",
		"\0",
		"Write successful.\0",
		"Seek successful.\0",
		"File closed.\0",
		"Directory created.\0",
		"Directory removed.\0",
		"Directory changed.\0",
		"\0",
		"\0",
		"\0",
		"File imported.\0",
		"File exported.\0",
		"Connected to server: \0",
		"Connection to server closed\0"
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
		"ERROR: socket - unable to run poll()\0",
		"ERROR: unable to receive command, network error on recv()\0",
		"ERROR: unable to allocate network socket\0",
		"ERROR: unable to bind to socket\0",
		"ERROR: problem connecting to server \0",
		"ERROR: problem disconnecting from server, assuming connection closed \0",
		"ERROR: problem listening on the bound port \0",
		"ERROR: unable to receive results, network error on recv()\0"
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
		{"pwd\0",sh_pwd,"Usage: pwd\0"},
		{"read\0",sh_read,"Usage: read <fd> <num_of_bytes>\0"},
		{"rm\0",sh_rm,"Usage: rm <filename>\0"},
		{"rmdir\0",sh_rmdir,"Usage: rmdir <dir name>\0"},
		{"seek\0",sh_seek,"Usage: seek <fd> <byte_offset>\n"
				"A negative byte offset will move back instead of forward in the file\0"},
		{"tree\0",sh_tree,"Usage: tree\0"},
		{"write\0",sh_write,"Usage: write <fd> <string>\0"}
};

char* cat_num(char* a, int n) {
	int len = 5;
	if(n>10000 && n<100000000) {
		len = 9;
	} else {
		len = 16;
	}
	char* b = calloc(1,len);
	sprintf(b,"%d",n);
	strcat(a,b);
	free(b);
	return a;
}

// incomplete function
// need to make case insensitive
// need quote handling
char* run_cmd(char *cmdstr) {
	char *cmd_argv[SH_MAX_ARGS];
	char *cmd_tok=NULL;
	int cmd_argc = 0;
	char *result=NULL;
	int last_stop = 0;
	int qopen = 0;
	int dqopen = 0;

	int cmdstr_len = strlen(cmdstr);
	for(int i = 0; i<cmdstr_len; i++) {
		char c = cmdstr[i];
		if (c == '\"' || c == '\'') {
			// quote handling
			if(c=='\"') {
				if(dqopen) dqopen=0;
				else dqopen=1;
			} else if (c=='\'') {
				if(qopen) qopen=0;
				else qopen=1;
			}
		} else if ((c == ' ' && !dqopen && !qopen) || c == '\n' || c == '\r'){
			if(cmd_tok) {
				cmd_argv[cmd_argc] = calloc(1,sizeof(char)*(i-last_stop));
				strncpy(cmd_argv[cmd_argc],cmdstr+last_stop+1,(i-last_stop-1));
				(cmd_argv[cmd_argc])[i-last_stop-1] = '\0';

				cmd_argc++;
			} else {
				cmd_tok = calloc(1,sizeof(char)*(i+1));
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
			result = mesg(result,SH_ERR_BADCMD,STR_TYPE_ERR,0);
		}
	} else {
		result = NULL;
	}
	if(cmd_tok) free(cmd_tok);
	int j=0;
	for(j=0;j<cmd_argc;j++) {
		free(cmd_argv[j]);
	}

	return result;
}

char* sh_exit(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;
	(void)(cmd_argc);
	(void)(cmd_argv);
	if(shell_client.status == CLIENT_STATUS_OPEN) {
		cmd_err = rclose();
		if(cmd_err<0) {
			char* str_errno = strerror(errno);
			result = mesg(result,SH_ERR_RCLOSE,STR_TYPE_ERR,strlen(str_errno));
			strcat(result, "\n");
			strcat(result, str_errno);
		} else {
			result = mesg(result,STR_SUCCESS_REXIT,STR_TYPE_STR,0);
		}
	} else {
		shell_server.status = SVR_STATUS_EXIT;
		result = mesg(result,STR_EXIT,STR_TYPE_STR,0);
	}

	return result;
}


char* sh_mkfs(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;
	(void)(cmd_argc);
	(void)(cmd_argv);
	cmd_err = blockdev_attach();
	if(cmd_err>=SH_ERR_SUCCESS) {
		cmd_err = cnmkfs();

		if(cmd_err<0) {
			// error
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		} else {
			cmd_err=cnmount();
			if(cmd_err<0) {
				// error
				result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
			} else {
				shell_server.vfs = VFS_STATUS_ON;
				result = mesg(result,STR_SUCCESS_MKFS,STR_TYPE_STR,0);
			}
		}
	} else {
		result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
	}
	return result;
}

char* sh_open(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc != 2) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_OPEN].help)+2));
		strcpy(result, sh_cmds[SH_CMD_OPEN].help);
	} else {
		int16_t f_fd = -1;
		int mode = 0;
		if(cmd_argv[1][0]=='r') mode = FD_READ;
		else if(cmd_argv[1][0]=='w') mode = FD_WRITE;
		else if(cmd_argv[1][0]=='\'' || cmd_argv[1][0]=='\"' ) {
			if(cmd_argv[1][1]=='r') mode = FD_READ;
			else if(cmd_argv[1][1]=='w') mode = FD_WRITE;
			else cmd_err = -1;
		}
		else cmd_err = -1;
		if(cmd_err>=0) {
			f_fd = cnopen(cwd, cmd_argv[0], mode);
			cmd_err = f_fd;
		}
		if(cmd_err<0) {
			// error
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		} else {
			result = calloc(1,sizeof(char)*(strlen("SUCCESS, fd = ")+5));
			strcpy(result, "SUCCESS, fd = ");
			cat_num(result, f_fd);
		}
	}
	return result;
}

char* sh_close(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc != 1) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_CLOSE].help)+2));
		strcpy(result, sh_cmds[SH_CMD_CLOSE].help);
	} else {
		int16_t f_fd = (int16_t)strtol(cmd_argv[0],(char **)NULL, 10);
		cmd_err = cnclose(f_fd);
		if(cmd_err<0) {
			// error
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		} else {
			result = calloc(1,sizeof(char)*(strlen("File closed.")+2));
			strcpy(result, "File closed.");
		}
	}
	return result;
}

char* sh_read(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;
	if(chk_vfs(&result)<0) return result;
	if(cmd_argc != 2) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_READ].help)+2));
		strcpy(result, sh_cmds[SH_CMD_READ].help);
	} else {
		size_t bytes = (size_t)strtol(cmd_argv[1],(char **)NULL, 10);
		result = calloc(1,sizeof(char)*bytes);
		int16_t f_fd = (int16_t)strtol(cmd_argv[0],(char **)NULL, 10);
		size_t bytes_read = 0;
		bytes_read = cnread((uint8_t*)result, bytes, f_fd);
		if(bytes_read==0) {
			cmd_err = bytes_read;
		}
		if(cmd_err<=0) {
			// error
			free(result);
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		}
	}
	return result;
}

char* sh_write(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc != 2) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_WRITE].help)+2));
		strcpy(result, sh_cmds[SH_CMD_WRITE].help);
	} else {
		size_t bytes = strlen(cmd_argv[1]);
		int16_t f_fd = (int16_t)strtol(cmd_argv[0],(char **)NULL, 10);
		size_t bytes_write = 0;
		bytes_write = cnwrite((uint8_t*)cmd_argv[1], bytes, f_fd);
		if(bytes_write==0) {
			cmd_err = bytes_write;
		}
		if(cmd_err<=0) {
			// error
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		} else {
			result = calloc(1,sizeof(char)*(strlen("Write successful.")+2));
			strcpy(result, "Write successful.");
		}
	}
	return result;
}

char* sh_seek(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc != 2) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_SEEK].help)+2));
		strcpy(result, sh_cmds[SH_CMD_SEEK].help);
	} else {
		uint32_t offset = (int16_t)strtol(cmd_argv[0],(char **)NULL, 10);
		int16_t f_fd = (int16_t)strtol(cmd_argv[0],(char **)NULL, 10);
		cmd_err = cnseek(f_fd, offset);
		if(cmd_err<=0) {
			// error
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		} else {
			result = calloc(1,sizeof(char)*(strlen("Seek successful.")+2));
			strcpy(result, "Seek successful.");
		}
	}
	return result;
}

char* sh_mkdir(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc != 1) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_MKDIR].help)+2));
		strcpy(result, sh_cmds[SH_CMD_MKDIR].help);
	} else {
		cmd_err = cnmkdir(cmd_argv[0]);
		if(cmd_err<0) {
			// error
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		} else {
			result = calloc(1,sizeof(char)*(strlen("Directory created.")+2));
			strcpy(result, "Directory created.");
		}
	}
	return result;
}

char* sh_rmdir(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc != 1) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_RMDIR].help)+2));
		strcpy(result, sh_cmds[SH_CMD_RMDIR].help);
	} else {
		(void)cmd_argv;
		cmd_err = cnrmdir(cmd_argv[0]);
		if(cmd_err<0) {
			// error
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		} else {
			result = calloc(1,sizeof(char)*(strlen("Directory removed.")+2));
			strcpy(result, "Directory removed.");
		}
	}
	return result;
}

char* sh_rm(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc != 1) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_RM].help)+2));
		strcpy(result, sh_cmds[SH_CMD_RM].help);
	} else {
		(void)cmd_argv;
		//cmd_err = cnrm(cmd_argv[0]);
		if(cmd_err<0) {
			// error
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		} else {
			result = calloc(1,sizeof(char)*(strlen("File removed.")+2));
			strcpy(result, "File removed.");
		}
	}
	return result;
}

char* sh_cat(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc != 1) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_CAT].help)+2));
		strcpy(result, sh_cmds[SH_CMD_CAT].help);
	} else {
		result = calloc(1,sizeof(char)*4096);
		cmd_err = cncat(cmd_argv[0],result);
		if(cmd_err<0) {
			// error
			free(result);
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		}
	}
	return result;
}

char* sh_cd(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc != 1) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_CD].help)+2));
		strcpy(result, sh_cmds[SH_CMD_CD].help);
	} else {
		cmd_err = cncd(cmd_argv[0]);
		if(cmd_err<0) {
			// error
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		} else {
			result = calloc(1,sizeof(char)*(strlen("Directory changed.")+2));
			strcpy(result, "Directory changed.");
		}
	}
	return result;
}

char* sh_ls(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc>1) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_LS].help)+2));
		strcpy(result, sh_cmds[SH_CMD_LS].help);
	} else {
		result = calloc(1,sizeof(char)*4096);
		if(cmd_argc==1) {
			cmd_err = cnls(cmd_argv[0],result);
		} else {
			cmd_err = cnls("",result);
		}
		if(cmd_err<0) {
			// error
			free(result);
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		}
	}
	return result;
}

char* sh_pwd(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;
	(void)cmd_argv;

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc>0) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_PWD].help)+2));
		strcpy(result, sh_cmds[SH_CMD_PWD].help);
	} else {
		result = calloc(1,sizeof(char)*256);
		cmd_err = cnpwd(result);
		if(cmd_err<0) {
			// error
			free(result);
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		}
	}
	return result;
}

char* sh_tree(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;
	(void)(cmd_argv);

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc>0) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_TREE].help)+2));
		strcpy(result, sh_cmds[SH_CMD_TREE].help);
	} else {
		result = calloc(1,sizeof(char)*4096);
		cmd_err = cntree(result);
		if(cmd_err<0) {
			// error
			free(result);
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		}
	}
	return result;
}

char* sh_import(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(chk_vfs(&result)<0) return result;
	if(cmd_argc != 2) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_IMPORT].help)+2));
		strcpy(result, sh_cmds[SH_CMD_IMPORT].help);
	} else {
		(void)cmd_argv;
		cmd_err = cnimport(cmd_argv[0], cmd_argv[1]);
		if(cmd_err<0) {
			// error
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		} else {
			result = calloc(1,sizeof(char)*(strlen("File imported.")+2));
			strcpy(result, "File imported.");
		}
	}
	return result;
}

char* sh_export(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(chk_vfs(&result)<0) return result;
	if(chk_vfs(&result)) return result;

	if(cmd_argc != 2) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_EXPORT].help)+2));
		strcpy(result, sh_cmds[SH_CMD_EXPORT].help);
	} else {
		(void)cmd_argv;
		cmd_err = cnimport(cmd_argv[0], cmd_argv[1]);
		if(cmd_err<0) {
			// error
			result = mesg(result,SH_ERR_UNK,STR_TYPE_ERR,0);
		} else {
			result = calloc(1,sizeof(char)*(strlen("File exported.")+2));
			strcpy(result, "File exported.");
		}
	}
	return result;
}

char* sh_connect(int cmd_argc, char* cmd_argv[]) {
	char* result = NULL;
	sh_err cmd_err = SH_ERR_SUCCESS;

	if(cmd_argc != 2) {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_CONNECT].help)+2));
		strcpy(result, sh_cmds[SH_CMD_CONNECT].help);
	} else {
		cmd_err = rconnect(cmd_argv[0],cmd_argv[1]);
		if(cmd_err>=0) {
			result = calloc(1,sizeof(char)*(strlen("Connected to server: ")+strlen(cmd_argv[0])+2));
			strcpy(result, "Connected to server: ");
			strcat(result, cmd_argv[0]);
		} else {
			char* str_errno = strerror(errno);
			result = calloc(1,sizeof(char)*(strlen(err_str(SH_ERR_RCONNECT))+strlen(cmd_argv[0])+strlen(str_errno)+2));
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
				result = calloc(1,sizeof(char)*(strlen(sh_cmds[i].help)));
				strcpy(result, sh_cmds[i].help);
				break;
			}
		}
		if(!result) {
			result = mesg(result,SH_ERR_BADCMD,STR_TYPE_ERR,0);
		}
	} else {
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[SH_CMD_HELP].help)+strlen(str_table[STR_CMDS])+2));
		strcpy(result, sh_cmds[SH_CMD_HELP].help);
		strcat(result, "\n");
		strcat(result, str_table[STR_CMDS]);
	}

	return result;
}

sh_err chk_vfs(char** result) {

	sh_err vfs_err = SH_ERR_SUCCESS;
	if(shell_server.vfs != VFS_STATUS_ON) {
		*result = calloc(1,sizeof(char)*(strlen(err_str(SH_ERR_NOVFS))+2));
		strcpy(*result,err_str(SH_ERR_NOVFS));
		vfs_err = SH_ERR_NOVFS;
	}
	return vfs_err;
}

char* mesg(char* result, int idx, int type, int extra) {


	if(type==STR_TYPE_STR) {
		// strings table
		result = calloc(1,sizeof(char)*(strlen(str_table[idx])+extra+2));
		strcpy(result, str_table[idx]);
	} else if(type==STR_TYPE_HELP){
		// help strings
		result = calloc(1,sizeof(char)*(strlen(sh_cmds[idx].help)+extra+2));
		strcpy(result, sh_cmds[idx].help);
	} else {
		// error strings
		result = calloc(1,sizeof(char)*(strlen(err_str(idx))+extra+2));
		strcpy(result,err_str(idx));
	}
	return result;
}

