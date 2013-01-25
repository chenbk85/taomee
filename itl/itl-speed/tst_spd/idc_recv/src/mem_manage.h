/*
 * mem_manage.h
 *
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef MEM_MANAGE_H_
#define MEM_MANAGE_H_

/*
 * @typedef mem_queue_t
 * @brief 管理为每个ip探测结果结构分配的内存,回收时不释放,作为一个内存池
 */
typedef struct queue{
	store_result_t *head;
	store_result_t *tail;
}__attribute__((packed)) mem_queue_t;

//所有分配的小块内存用一个队列管理
extern mem_queue_t	mem_quque;

/*
 * @brief 返回一个探测结果结构类型的指针
 * @param no
 * @return store_result_t *
 */
extern store_result_t *get_mem();

/*
 * @brief 回收一块内存
 * @param store_result_t *addr, 一块待回首的内存的地址
 */
extern void recycle_mem(store_result_t *addr);

/*
 * @brief 初始化内存队列以及头部和尾部的两个锁
 */
extern void mem_queue_init();

/*
 * @brief 程序退出时 释放所有内存
 */
extern void mem_queue_destroy();


#endif /* MEM_MANAGE_H_ */
