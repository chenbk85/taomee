/*
 * recv_data.h
 *
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef RECV_DATA_H_
#define RECV_DATA_H_

// needed for uintxx_t
#include <stdint.h>

/*
 * @typedef q_node_t
 * @brief每一个node维护一个指向store_result_t*类型的指针 recv_link,指向收取某个文件的全部数据
 *  number维护node的编号
 */
typedef struct q_node{
	store_result_t *recv_link;
	uint32_t number;
	struct q_node *next;
}__attribute__((packed)) q_node_t;

/*
 * @typedef file_queue_t
 * @brief 所有node组成一个收文件的node队列
 */
typedef struct file_queue{
	q_node_t *head;
	q_node_t *tail;
}__attribute__((packed)) file_queue_t;

//与客户端交互数据时以des_pass_phrase为密码进行des加密
extern char	des_pass_phrase[PASSWD_LEN];

//256个node
extern q_node_t *file_recv_node[256];

// 记录每个node的链表长度
extern uint32_t file_recv_node_len[256];

/*
 * @brief 初始化队列，将256个空闲node全部入队列，node的获取与回收由get_node和recycle_node完成
 * @brief 在initiate()中调用
 */
extern void file_queue_init();

/*
 * @brief 销毁队列，释放为每个node分配的内存，node拥有的recv_link的内存由mem_manage来管理，自身只维护一个指针。
 * @brief 在destroy()中调用
 */
extern void file_queue_destroy();

/*
 * @brief listen每次accept一个连接，则创建一个线程收取文件，线程的call_back函数即为handler，参数是accept的fd
 * @param int fd
 */
extern int handler(int fd);

#endif /* RECV_DATA_H_ */
