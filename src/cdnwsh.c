/*
 * cdnwsh.c
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#include <cdnwsh.h>

int main(int argc, char *argv[]) {

	(void)argc;
	(void)argv;

	in_port_t svr_port = start_listening();

	printf("\nServer now running on port %d\n",svr_port);

	sh_err run_err = run();

	return run_err;
}
