/*
 * recv_data.h
 *
 * Created on:	2011-8-4
 * Author:		Singku
 * Paltform:		Linux Fedora Core 8 x86-32
 *	Compiler:		GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef RECV_DATA_H_
#define RECV_DATA_H_


extern uint32_t memory_size;
extern volatile uint32_t	sum_data_in_mem[2];
extern char *g_buffer[2];
extern char *g_buffer_ptr[2];
extern int g_buffer_tag;

extern int recv_data(char *buf, uint32_t proto, uint32_t special_flag, uint32_t len);

#endif /* RECV_DATA_H_ */
