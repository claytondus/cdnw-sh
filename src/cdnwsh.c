/*
 * cdnwsh.c
 *
 *  Created on: Feb 13, 2016
 *      Author: nwilder
 */

#include <cdnwsh.h>

#ifndef UNITY_FIXTURES
int main(int argc, char *argv[]) {

	(void)argc;
	(void)argv;

	sh_err svr_err = start_listening();

	if(svr_err >=0) {
		printf("\nServer now running on port %s\n",SVR_PORT);
	} else {
		printf("\n%s",err_str(svr_err));
	}
	fflush(stdout);
	sh_err run_err = run();

	return run_err;
}
#endif
