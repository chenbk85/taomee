/*
 * collect.h
 *
 *  Created on:	2011-7-5
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef COLLECT_H_
#define COLLECT_H_


/*
 * @typedef my_thread_t
 * @brief 线程节点。
 */
typedef struct my_thread{
	pthread_t athread;
	struct my_thread *next;
}my_thread_t;

/*
 * @typedef thread_queue_t
 * @brief 用线程节点组成线程队列，以管理和控制最大线程数
 */
typedef struct thread_queue{
	my_thread_t *head;
	my_thread_t *tail;
}thread_queue_t;

enum max_thread{
	//可同时探测IP的线程最大数目，如果不设上限，待探测IP集合太大会导致启动线程失败或者系统很卡
	MAX_THREAD				= 32,
};

//初始化程序需要读IP入这个链表
extern ip_addr_t			*probe_ip_link;	//待探测IP链表

//采集数据线程函数
extern void* data_collect();

#endif /* COLLECT_H_ */
