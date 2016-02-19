/*
 * config.h
 *
 *  Created on: Feb 18, 2016
 *      Author: parallels
 */

#ifndef INCLUDE_BLOCK_H_
#define INCLUDE_BLOCK_H_

#include <stdint.h>

#define BLOCK_SIZE 4096

typedef struct {
	int8_t byte[BLOCK_SIZE];
} block;

#endif /* INCLUDE_BLOCK_H_ */
