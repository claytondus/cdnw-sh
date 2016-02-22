/*
 * server.h
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#ifndef INCLUDE_SERVER_H_
#define INCLUDE_SERVER_H_

#include <stdio.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SVR_PORT "5560"
#define SVR_TIMEOUT_CHKSOCK 2000000

#define SVR_STATUS_RUN		2
#define SVR_STATUS_INIT		1
#define SVR_STATUS_EXIT		0
#define SVR_STATUS_ERR		-1			// unknown error, else use SH_ERR codes

struct svrconf {
	int8_t status;
	struct sockaddr_in *svraddr;
	struct sockaddr_in *clientaddr;
	int max_socket;
	fd_set sockets_in, sockets_out, sockets_err;
	struct timeval tv_timeout_sock;
};

extern struct svrconf shell_server;

#endif /* INCLUDE_SERVER_H_ */
