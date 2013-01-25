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

#include "init_and_destroy.h"
#include "send.h"

int main(int argc,char **argv)
{
	if (initiate(argv[0]) == -1)
		return -1;

	data_send();

	destroy();
	return 0;
}
