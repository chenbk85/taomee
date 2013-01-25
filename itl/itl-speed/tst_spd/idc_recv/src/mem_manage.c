/*
 * mem_manage.c 内存管理
 *
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */


// needed for malloc  free
#include <stdlib.h>

// needed for strerror
#include <string.h>

// neede for pthread_exit and pthread_lock/unlock pthread_spin_init
#include <pthread.h>

// needed for errno
#include <errno.h>

// needed for store_result_t
#include "protocol.h"

// needed for timestring
#include "initiate.h"

// needed for write_log
#include "log.h"

#include "mem_manage.h"

// 内存队列
mem_queue_t mem_queue;

// 获取内存自悬锁，同时只能有一个线程取内存队列的头部
pthread_spinlock_t lock_mem_queue_get;
// 回收内存自悬锁，同时只能有一个线程入内存队列的尾部
pthread_spinlock_t lock_mem_queue_recycle;

/*
 * @brief 返回一个探测结果结构类型的指针
 * @param no
 * @return store_result_t *
 */
store_result_t *get_mem()
{
	store_result_t *tmp = NULL;
	pthread_spin_lock(&lock_mem_queue_get);
	if (mem_queue.head != NULL && mem_queue.head != mem_queue.tail) {
		tmp = mem_queue.head;
		mem_queue.head = mem_queue.head->next;
	}
	pthread_spin_unlock(&lock_mem_queue_get);

	if (tmp)
		return tmp;

	if ((tmp = (store_result_t*)malloc(sizeof(store_result_t))) == NULL) {
		write_log("Get_mem,malloc failed:%s\n", strerror(errno));
		pthread_exit(NULL);
	}
	return tmp;
}

/*
 * @brief 回收一块内存
 * @param store_result_t *addr, 一块待回首的内存的地址
 */
void recycle_mem(store_result_t *addr)
{
	pthread_spin_lock(&lock_mem_queue_recycle);
	if(mem_queue.head == NULL) {
		mem_queue.tail = addr;
		mem_queue.head = addr;
	}
	else {
		mem_queue.tail->next = addr;
		mem_queue.tail = addr;
	}
	pthread_spin_unlock(&lock_mem_queue_recycle);
}

/*
 * @brief 初始化内存队列以及头部和尾部的两个锁
 */
void mem_queue_init()
{
	mem_queue.head = mem_queue.tail = NULL;
	pthread_spin_init(&lock_mem_queue_get,0);
	pthread_spin_init(&lock_mem_queue_recycle,0);
}

/*
 * @brief 程序退出时 释放所有内存
 */
void mem_queue_destroy()
{
	store_result_t *tmp,*tmpnext;
	tmpnext= mem_queue.head;

	while(tmpnext != NULL) {
		tmp = tmpnext;
		tmpnext = tmpnext->next;
		free(tmp);
	}
	pthread_spin_destroy(&lock_mem_queue_get);
	pthread_spin_destroy(&lock_mem_queue_recycle);
}
