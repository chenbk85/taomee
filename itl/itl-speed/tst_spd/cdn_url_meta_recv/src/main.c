/*
 * main.c
 *
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#include <pthread.h>
#include "initiate.h"
#include "thread.h"

int main(int argc, char **argv)
{
	while (1) {
		initiate(argv[0]);

		start_threads();//listen thread->multi-recv-thread, store thread,
		wait_threads();

		destroy();
	}
	return 0;
}
