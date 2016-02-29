/*
 * client.c
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#include <cdnwsh.h>
#include <poll.h>

sh_err rconnect(char* server, char* port)
{
	sh_err cmd_err = SH_ERR_SUCCESS;

	struct addrinfo hints, *res;
	int sockfd;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(server, port, &hints, &res);

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	cmd_err = connect(sockfd, res->ai_addr, res->ai_addrlen);
	if(cmd_err>=0) {
		shell_client.cfd = sockfd;
		shell_client.status = CLIENT_STATUS_OPEN;
	} else {
		//error
		cmd_err = SH_ERR_RCONNECT;
	}


	return cmd_err;
}

sh_err rclose(void) {
	sh_err run_err = close(shell_client.cfd);
	shell_client.cfd = -1;
	shell_client.status = CLIENT_STATUS_CLOSE;
	return run_err;
}

sh_err send_cmd(char* cmdbuffer) {
	sh_err send_err = SH_ERR_SUCCESS;

	int cmd_len = strlen(cmdbuffer);

	if(cmd_len > SH_MAX_STR) {
		cmd_len = SH_MAX_STR;
		//warn about truncated cmd
	}
	int bytes_sent = send(shell_client.cfd,cmdbuffer,sizeof(char)*cmd_len,0);

	if(bytes_sent<1) send_err = bytes_sent;

	return send_err;
}

char* recv_results(void) {

	int size=0;
	char *results;
	results = malloc(sizeof(char)*SVR_MAX_PAYLOAD);
	results[0] = '\0';

	size = recv(shell_client.cfd, results, SVR_MAX_PAYLOAD*sizeof(char), 0);
	if(size < 0) {
		// handle error
		results[0] = '\0';
		results[1] = SH_ERR_RECV;
		results[2] = errno;
	}
	return results;

}
