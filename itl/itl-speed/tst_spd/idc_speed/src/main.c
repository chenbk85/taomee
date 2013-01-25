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
#include <unistd.h>

#include "init_and_destroy.h"
#include "thread.h"
#include "log.h"

int main(int argc,char **argv)
{
	initiate(argv[0]);

    int count = 0;
	while (1) {
        count++;

		//创建收线程与发线程
		if (0 != start_thread()) {
            write_log("%dth --> start_thread fail.\n", count);
        }
        else {
            write_log("%dth --> start_thread succ.\n", count);
        }

		//等待线程结束
		if (0 != wait_thread()) {
            write_log("%dth --> wait_thread fail.\n", count);
        }
        else {
            write_log("%dth --> wait_thread succ.\n", count);
        }

        sleep(1);
	}

	destroy();
	return 0;
}
