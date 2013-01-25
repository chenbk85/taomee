/*
 * main.c 主控逻辑，无限循环
 *
 *  Created on:	2011-7-4
 *  Author:		singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

/*
 * @brief main control logical of data sampling
 * @param no parameters
 */
#include <stdio.h>
#include "init_and_destroy.h"
#include "send.h"
#include "log.h"

int main(int argc,char **argv)
{
	if (initiate(argv[0]) == -1) {
		fprintf(stderr, "init failed!\n");
		return -1;
	}

	write_log("start to sending data\n");

	if (data_send() == -1)
		write_log("send data not complete!\n");
	else
		write_log("send data ok!\n");

	destroy();
	return 0;
}
