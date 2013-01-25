/*
 * thread.h 创建与等待收发线程
 *
 *  Created on:	2011-7-5
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef THREAD_H_
#define THREAD_H_

/*
 * @brief 创建收发线程
 */
extern int start_thread();
/*
 * @brief 等待收发线程
 */
extern int wait_thread();

#endif /* THREAD_H_ */
