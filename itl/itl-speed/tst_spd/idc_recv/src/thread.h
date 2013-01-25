/*
 * thread.h
 *
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef THREAD_H_
#define THREAD_H_

#define	MAX_THREAD	256

//node组成的file_queue队列是多线程共享的必须互斥来获得和归还一个头节点
extern pthread_mutex_t file_queue_mutex;
extern pthread_cond_t  file_queue_cond;

//线程计数变量也需要互斥访问
extern pthread_mutex_t thread_count_mutex;
extern pthread_cond_t  thread_count_cond;

//线程计数变量，当线程数达到最大线程数时，需要主线程等待
extern volatile int thread_count;

/*
 * @brief 开启监听线程
 */
extern void start_threads();

/*
 * @brief 等待监听线程结束
 */
extern void wait_threads();

#endif /* THREAD_H_ */
