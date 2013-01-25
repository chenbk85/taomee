/*
 * store_data.h
 *
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef STORE_DATA_H_
#define STORE_DATA_H_

//数据存储路径
extern char data_store_path[FULL_PATH_LEN];

/*
 * @brief 将一个recv_node对应的链表中的数据写入一个文件中
 * @param store_result_t *linkhead,也即recv_node对应的链表头
 * @param uint32 len,此链表中有多少个数据。
 * @return 0表示些成功，1表示写失败.
 */
extern int write_file(const char *filename, const char *buf, uint32_t data_len);

#endif /* STORE_DATA_H_ */
