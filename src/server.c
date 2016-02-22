/*
 * server.c
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#include <cdnwsh.h>
#include <sys/select.h>

struct svrconf shell_server;


void prep_select(void) {

	shell_server.tv_timeout_sock.tv_sec = 2;
	shell_server.tv_timeout_sock.tv_usec = 0;

	FD_ZERO(&(shell_server.sockets_in));
	FD_SET(0, &(shell_server.sockets_in));

	shell_server.max_socket = 1;

}

in_port_t start_listening(void) {

	shell_server.status = SVR_STATUS_INIT;

	// init shell_server;
	shell_server.tv_timeout_sock.tv_sec = SVR_TIMEOUT_CHKSOCK/1000000;
	shell_server.tv_timeout_sock.tv_usec = SVR_TIMEOUT_CHKSOCK%1000000;

	FD_ZERO( &(shell_server.sockets_in) );
	FD_ZERO( &(shell_server.sockets_out) );
	FD_ZERO( &(shell_server.sockets_err) );

	FD_SET(0, &(shell_server.sockets_in));	// listen to standard in
	/*		This block is a work in progress - possible compiler flag conflict?
	struct addrinfo hints;
	struct addrinfo *res;
	int sockfd;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;  		// IPv4
	hints.ai_socktype = SOCK_STREAM;	// TCP
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	getaddrinfo(NULL, "3490", &hints, &res);

	// make a socket:

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	bind(sockfd, res->ai_addr, res->ai_addrlen);

	*/
	shell_server.status = SVR_STATUS_RUN;

	return 0;
}


char* recv_cmd(void) {

	return NULL;
}


sh_err send_results(char* resultstr) {
	sh_err send_err;

	int result_len = strlen(resultstr);

	int bytes_sent = send(shell_server.svraddr->sin_port,resultstr,sizeof(char)*result_len,0);

	if(bytes_sent<1) send_err = bytes_sent;

	return send_err;
}


sh_err run(void) {

	char cmdbuffer[SH_MAX_STR];
	char* result;

	sh_err run_err = SH_ERR_SUCCESS;

	printf("%s",str_table[STR_PROMPT]);
	fflush(stdout);

	while(shell_server.status == SVR_STATUS_RUN) {

		prep_select();

		if(select(shell_server.max_socket+1, &(shell_server.sockets_in),
				NULL, NULL,
				&(shell_server.tv_timeout_sock))<0) {
			printf("%s",err_str(SH_ERR_SOCKET));
			fflush(stdout);
		}

		int i = 0;
		for(i = 0; i<=(shell_server.max_socket); i++) {

			if(FD_ISSET(i, &(shell_server.sockets_in))) {
				if(!i) {
					// stdin
					fgets(cmdbuffer,SH_MAX_STR,stdin);
					result = run_cmd(cmdbuffer);
					if(result) printf("%s",result);
					printf("\n%s",str_table[STR_PROMPT]);
					fflush(stdout);
				} else {
					// remote client
					run_err = send_results(run_cmd(recv_cmd()));
				}

			}

		}

	}
	if(!run_err) run_err = shell_server.status;
	return run_err;
}

