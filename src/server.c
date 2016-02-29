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
struct clientconf shell_client;

sh_err start_listening(void) {

	sh_err net_err = SH_ERR_SUCCESS;

	shell_server.status = SVR_STATUS_INIT;

	shell_server.svrfd = -1;
	shell_server.clientfd = -1;
	shell_server.client = CLIENT_STATUS_CLOSE;
	shell_server.num_fds = 1;

	shell_client.cfd = -1;
	shell_client.status = CLIENT_STATUS_CLOSE;

	struct addrinfo hints;
	struct addrinfo *res;
	int sockfd;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;  		// IPv4
	hints.ai_socktype = SOCK_STREAM;	// TCP
	hints.ai_flags = AI_PASSIVE;     	// fill in IP

	getaddrinfo(NULL, SVR_PORT, &hints, &res);

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

	if(sockfd>0) {
		net_err = bind(sockfd, res->ai_addr, res->ai_addrlen);
		if(net_err<0) {
			net_err = SH_ERR_BINDSOCK;
			perror("\nBind");
		} else {
			net_err = listen(sockfd, 1);
			if(net_err<0) {
				net_err = SH_ERR_LISTEN;
				perror("\nListen");
			} else {
				shell_server.svrfd = sockfd;
				shell_server.num_fds++;
			}
		}
	} else {
		net_err = SH_ERR_STARTSOCK;
		perror("\nSocket");
	}

	shell_server.status = SVR_STATUS_RUN;

	return net_err;
}


char* recv_cmd(void) {
	int size=0;
	char *cmdbuffer;
	cmdbuffer = malloc(sizeof(char)*SH_MAX_STR);
	cmdbuffer[0] = '\0';

	size = recv(shell_server.clientfd, cmdbuffer, SH_MAX_STR*sizeof(char), 0);
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

		bytes_sent = send(shell_server.clientfd,cursor,sizeof(char)*send_len,0);

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

sh_err accept_client(void) {
	int clientfd = -1;
	char str_addr[INET6_ADDRSTRLEN];
	sh_err cmd_err = SH_ERR_SUCCESS;
	struct sockaddr_storage client_addr;
	socklen_t client_socklen = sizeof(struct sockaddr_storage);

	clientfd = accept(shell_server.svrfd, (struct sockaddr *)&client_addr, &client_socklen);
	if(clientfd>0) {
		shell_server.client = CLIENT_STATUS_OPEN;
		shell_server.clientfd = clientfd;
	    if (client_addr.ss_family == AF_INET) {
	        inet_ntop(client_addr.ss_family,&(((struct sockaddr_in*)&client_addr)->sin_addr),str_addr, sizeof(str_addr));
	    } else {
	        inet_ntop(client_addr.ss_family,&(((struct sockaddr_in6*)&client_addr)->sin6_addr),str_addr, sizeof(str_addr));
	    }
		printf("\nClient connected from %s\n",str_addr);
	} else {
		cmd_err = clientfd;
		perror("\nAccept");
	}

	return cmd_err;
}


sh_err run(void) {

	char cmdbuffer[SH_MAX_STR];
	char* result;

	sh_err run_err = SH_ERR_SUCCESS;

	printf("%s",str_table[STR_PROMPT]);
	fflush(stdout);

	struct pollfd pfd_in[4];

	while(shell_server.status == SVR_STATUS_RUN) {

		result = NULL;
		pfd_in[0].fd = 0;
		pfd_in[0].events = POLLIN;
		pfd_in[0].revents = 0;
		int num_fds = 1;
		if(shell_server.svrfd != -1) {
			pfd_in[num_fds].fd = shell_server.svrfd;
			pfd_in[num_fds].events = POLLIN;
			pfd_in[num_fds].revents = 0;
			num_fds++;
		}
		if(shell_server.clientfd != -1) {
			pfd_in[num_fds].fd = shell_server.clientfd;
			pfd_in[num_fds].events = POLLIN;
			pfd_in[num_fds].revents = 0;
			num_fds++;
		}
		if(shell_client.cfd != -1) {
			pfd_in[num_fds].fd = shell_client.cfd;
			pfd_in[num_fds].events = POLLIN;
			pfd_in[num_fds].revents = 0;
			num_fds++;
		}

		sh_err pol_err = poll(pfd_in, shell_server.num_fds, SVR_TIMEOUT_CHKSOCK);
		if(pol_err<0) {
			printf("%s\n",err_str(SH_ERR_SOCKET));
			perror("Errno");
			fflush(stdout);
		}
		if(pfd_in[0].revents & POLLIN) {
			fgets(cmdbuffer,SH_MAX_STR,stdin);
			//client sending cmd
			if(shell_client.status == CLIENT_STATUS_OPEN) {
				run_err = send_cmd(cmdbuffer);
				if(run_err<0) {
					printf("ERROR: problem with sending command to remote system\n");
					perror("Errno");
				}

			} else {
			// stdin
				result = run_cmd(cmdbuffer);
				if(result) printf("%s",result);

			}
			if(shell_server.status == SVR_STATUS_RUN) {
				if(shell_client.status == CLIENT_STATUS_OPEN) {
					printf("\n%s",str_table[STR_REMOTE_PROMPT]);
				} else {
					printf("\n%s",str_table[STR_PROMPT]);
				}
			} else {
				printf("\n\n");
			}
			fflush(stdout);
		}
		int i = 1;
		for(i = 1; i<num_fds; i++) {
			if(pfd_in[i].revents & POLLIN) {
				// client asking to open connection
				if(pfd_in[i].fd == shell_server.svrfd) {
					run_err = accept_client();
					if(run_err<0) {
						printf("\n%s",err_str(run_err));
						perror("Accept");
						fflush(stdout);
					}
				}
				// remote client sending cmd
				if(pfd_in[i].fd == shell_server.clientfd) {
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
						perror("Recv cmd");
						fflush(stdout);
					}
				}
				// client receiving cmd results
				if(pfd_in[i].fd == shell_client.cfd) {
					result = recv_results();
					if(result[0]=='\0') {
						run_err = result[1];
						int err_errno = result[2];
						printf("\n%s",err_str(run_err));
						printf("\n%s\n",strerror(err_errno));
					} else {
						printf("%s",result);
					}
					fflush(stdout);
				}
			}
		}
		if(result) free(result);

	}
	if(!run_err) run_err = shell_server.status;
	return run_err;
}
