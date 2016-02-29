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
#include <cdnwsh.h>
#include <sys/select.h>
#include <netdb.h>
#include <errno.h>

#define SVR_PORT "5560"
#define SVR_TIMEOUT_CHKSOCK 2000
#define SVR_MAX_PAYLOAD		4096

#define SVR_STATUS_CLIENT	3			//a client is currently connected
#define SVR_STATUS_RUN		2
#define SVR_STATUS_INIT		1
#define SVR_STATUS_EXIT		0
#define SVR_STATUS_ERR		-1			// unknown error, else use SH_ERR codes

#define CLIENT_STATUS_CLOSE		0
#define CLIENT_STATUS_OPEN		1
#define CLIENT_STATUS_ERR		-1
#define CLIENT_CODE_CLOSE		7

#define VFS_STATUS_OFF			0
#define VFS_STATUS_ON			1


struct svrconf {
	int8_t status;
	int svrfd;
	int timeout_sock;
	int8_t client;	// status of client
	int clientfd;
	int num_fds;
	int8_t vfs;
};

struct clientconf {
	int8_t status;
	int cfd;
};

extern struct svrconf shell_server;
extern struct clientconf shell_client;

#endif /* INCLUDE_SERVER_H_ */
