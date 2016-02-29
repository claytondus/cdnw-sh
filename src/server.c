/*
 * server.c
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#include <cdnwsh.h>
#include <sys/select.h>
#include </usr/include/netdb.h>
#include <errno.h>
#include <poll.h>

struct svrconf shell_server;

sh_err start_listening(void) {

	sh_err net_err = SH_ERR_SUCCESS;
	sh_err detail_err = SH_ERR_SUCCESS;

	shell_server.status = SVR_STATUS_INIT;

	shell_server.svrfd = -1;
	shell_server.clientfd = -1;
	shell_server.client = CLIENT_STATUS_CLOSE;
	shell_server.num_fds = 1;

	struct addrinfo hints;
	struct addrinfo *res;
	int sockfd;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;  		// IPv4
	hints.ai_socktype = SOCK_STREAM;	// TCP
	hints.ai_flags = AI_PASSIVE;     	// fill in IP

	getaddrinfo(NULL, SVR_PORT, &hints, &res);

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

	if(sockfd>0) {
		net_err = bind(sockfd, res->ai_addr, res->ai_addrlen);
		if(net_err<0) {
			detail_err = errno;
			net_err = SH_ERR_BINDSOCK;
			printf("\nErrno: %d\n",detail_err);
		} else {
			shell_server.svrfd = sockfd;
			shell_server.num_fds++;
		}
	} else {
		net_err = SH_ERR_STARTSOCK;
	}

	shell_server.status = SVR_STATUS_RUN;

	return net_err;
}


char* recv_cmd(void) {
	int size=0;
	char *cmdbuffer;
	cmdbuffer = malloc(sizeof(char)*SH_MAX_STR);
	cmdbuffer[0] = '\0';

	size = recv(shell_server.svrfd, cmdbuffer, SH_MAX_STR*sizeof(char), 0);
	if(size <= 0) {
		// handle error
		cmdbuffer[0] = '\0';
		cmdbuffer[1] = SH_ERR_RECV;
		cmdbuffer[2] = errno;

	}
	return cmdbuffer;
}


sh_err send_results(char* resultstr) {
	sh_err send_err;
	int bytes_sent;

	int result_len = strlen(resultstr)*sizeof(char);
	int bytes_remain = result_len;
	char* cursor = resultstr;

	while(bytes_remain > 0) {
		int send_len = result_len > (SVR_MAX_PAYLOAD-1) ? (SVR_MAX_PAYLOAD-1) : result_len;

		bytes_sent = send(shell_server.svrfd,cursor,sizeof(char)*send_len,0);

		if(bytes_sent > 0) {
			bytes_remain -= bytes_sent;
			cursor += bytes_sent / sizeof(char);
		} else {
			bytes_remain = bytes_sent;
		}
	}
	if(bytes_sent<1) send_err = bytes_sent;

	return send_err;
}


sh_err run(void) {

	char cmdbuffer[SH_MAX_STR];
	char* result;

	sh_err run_err = SH_ERR_SUCCESS;

	printf("%s",str_table[STR_PROMPT]);
	fflush(stdout);

	struct pollfd pfd_in[3];

	while(shell_server.status == SVR_STATUS_RUN) {

		result = NULL;
		pfd_in[0].fd = 0;
		pfd_in[0].events = POLLIN;
		pfd_in[0].revents = 0;
		if(shell_server.svrfd != -1) {
			pfd_in[1].fd = shell_server.svrfd;
			pfd_in[1].events = POLLIN;
			pfd_in[1].revents = 0;
		} else if(shell_server.clientfd != -1) {
			pfd_in[1].fd = shell_server.clientfd;
			pfd_in[1].events = POLLIN;
			pfd_in[1].revents = 0;
		}
		if(shell_server.num_fds == 3) {
			pfd_in[2].fd = shell_server.clientfd;
			pfd_in[2].events = POLLIN;
			pfd_in[2].revents = 0;
		}

		sh_err pol_err = poll(pfd_in, shell_server.num_fds, SVR_TIMEOUT_CHKSOCK);
		if(pol_err<1) {
			printf("%s\n",err_str(SH_ERR_SOCKET));
			perror("Errno");
			fflush(stdout);
		}
		if(pfd_in[0].revents & POLLIN) {
			fgets(cmdbuffer,SH_MAX_STR,stdin);
			//client sending cmd
			if(shell_server.client == CLIENT_STATUS_OPEN) {
				run_err = send_cmd(cmdbuffer);
				if(run_err<0) {
					printf("ERROR: problem with sending command to remote system\n");
					perror("Errno");
				}
				printf("\n%s",str_table[STR_REMOTE_PROMPT]);
			} else {
			// stdin
				result = run_cmd(cmdbuffer);
				if(result) printf("%s",result);
				printf("\n%s",str_table[STR_PROMPT]);
			}
			fflush(stdout);
		}
		if(pfd_in[1].revents & POLLIN) {
			// remote client sending cmd
			result = recv_cmd();
			if(result[0]=='\0') {
				run_err = result[1];
				int err_errno = result[2];
				printf("\n%s",err_str(run_err));
				printf("\n%s\n",strerror(err_errno));
			} else {
				run_err = send_results(run_cmd(result));
			}
			if(run_err<0) {
				printf("\n%s",err_str(run_err));
				perror("Errno");
				fflush(stdout);
			}
		}
		if(pfd_in[2].revents & POLLIN) {
			// client receiving cmd results
			result = recv_results();
			if(result[0]=='\0') {
				run_err = result[1];
				int err_errno = result[2];
				printf("\n%s",err_str(run_err));
				printf("\n%s\n",strerror(err_errno));
			} else {
				printf("%s",result);
			}
			fflush(stdin);
			printf("\n%s",str_table[STR_REMOTE_PROMPT]);
			fflush(stdout);
		}
		if(result) free(result);

	}
	if(!run_err) run_err = shell_server.status;
	return run_err;
}
