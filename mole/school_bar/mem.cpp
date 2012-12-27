/*
 * =====================================================================================
 *
 *       Filename:  mem.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/07/2010 04:23:22 PM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

extern "C" {
#include <string.h>
#include <libtaomee/list.h>
}

#include "mem.hpp"
#include "process.hpp"

/* @brief 对变量，链表初始化
 */
int Mem :: mem_init()
{
	/*空闲块最少的数目*/
	min_block_num = config_get_intval("min_block_num", 10000);
	/*空闲块和使用块的比率*/
	free_div_used = config_get_intval("free_div_used", 3);
	INIT_LIST_HEAD(&used_block.head);
	for (int i = 0; i < max_data_size; i++) {
		INIT_LIST_HEAD(&free_block[i].head);
	}
	return 0;
}

/* @brief 根据数据的长度，计算分配空间的大小
 * @param len 数据的长度
 * @param type 根据长度，返回块的类型
 */
int Mem :: get_malloc_size(int len, int *type)
{
	int i = 0;
	for (i = 0; i < max_data_size; i++) {
		if ((len / (128 * (1 << i))) == 0) {
			break;
		}
	}

	if (i == max_data_size) {
		ERROR_LOG("CLIENT DATA TOO LARGE data len %d", len);
		return -1;
	}
	*type = i;

	return (128 * (1 << i));
}

/* @brief 新分配一块缓存
 * @param len 数据块的长度
 */
struct mem_block * Mem :: mem_block_new(int len) 
{
	struct mem_block *ptr = (struct mem_block *)malloc(sizeof(struct mem_block));
	memset(ptr, 0, sizeof(struct mem_block));
	int type = 0;
	int size = 0;
	if ((size = get_malloc_size(len, &type)) == -1) {
		return NULL;
	}
	ptr->data = (uint8_t *)malloc(size);
	INIT_LIST_HEAD(&ptr->list);
	ptr->type = type;
	list_add(&ptr->list, &used_block.head);
	used_block.block_num++;
	return ptr;
}

/* @brief 释放链表的第一块内存
 * @param header 链表表头
 */
int Mem :: release_mem_block(struct mem_heap *header)
{
	if (list_empty(&header->head)) {
		return 0;
	}
	mem_block_t *ptr = list_entry(header->head.next, mem_block_t, list);
	list_del(header->head.next);
	header->block_num--;
	free(ptr->data);
	free(ptr);
	return 1;
}

/* @brief 从空闲链表中得到一块空闲缓存
 * @param len 数据块的长度
 */
struct mem_block * Mem :: get_free_block_from_free_heap(int len)
{
	int type = 0;
	if (get_malloc_size(len, &type) == -1) {
		return NULL;
	}
	
	if (list_empty(&free_block[type].head)) {
		return NULL;
	} 
	mem_block_t *ptr = list_entry(free_block[type].head.next, mem_block_t, list);
	list_del(free_block[type].head.next);
	free_block[type].block_num--;
	ptr->size = 0;
	
	list_add(&ptr->list, &used_block.head);
	used_block.block_num++;

	return ptr;
}

/* @breif 从已使用数据块的链表中得到数据块
 */
struct mem_block * Mem :: get_used_block_from_used_heap()
{
	if (list_empty(&used_block.head)) {
		return NULL;
	}

	mem_block_t *ptr = list_entry(used_block.head.next, mem_block_t, list);
	list_del(used_block.head.next);
	used_block.block_num--;

	list_add(&ptr->list, &free_block[ptr->type].head);
	free_block[ptr->type].block_num++;
	return ptr;
}

/* @brief 如果空闲块过多，释放多余的空闲块
 */
int Mem :: release_surplus_free_block() 
{
	int start = min_block_num;
	for (int i = 0; i < max_data_size; i++) {
		if (free_block[i].block_num <= min_block_num) {
			continue;
		}

		if (used_block.block_num == 0) {
			start = min_block_num;	
		} else if (free_block[i].block_num / used_block.block_num > free_div_used) {
			start = used_block.block_num * free_div_used;
			if (start < min_block_num) {
				start = min_block_num;
			}
		}
		int end = free_block[i].block_num;
		for (int j = start; j < end; j++) {
			release_mem_block(&free_block[i]);
		}
	}
	return 0;
}

/* @brief 从新保存到使用队列中
 */
void Mem :: back_to_used_heap(mem_block_t *ptr)
{
	list_move(&ptr->list, &used_block.head);
	used_block.block_num++;
}

/* @brief 保存已使用队列的数据到文件中
 */
void Mem :: save_used_heap()
{
	struct mem_block *ptr = NULL;
	while (used_block.block_num) {
		ptr =  get_used_block_from_used_heap();
		DEBUG_LOG("SAVE MESSAGE %u %u %s", ptr->uid, ptr->cmd, ptr->data);
	}
}

/* @brief 得到一块空闲的内存块,如果空闲链表里没有，则malloc一块新的内存
 */
struct mem_block * Mem :: get_free_block(int len)
{
	struct mem_block *ptr = get_free_block_from_free_heap(len);
	if (ptr == NULL) {
		ptr = mem_block_new(len);
	}

	return ptr;
}

/* @brief 得到一块保存数据的内存块
 */
struct mem_block * Mem :: get_used_block()
{
	return get_used_block_from_used_heap();
}

