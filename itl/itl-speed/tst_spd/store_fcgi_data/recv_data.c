/*
 * recv_data.c
 *
 * Created on:	2011-8-4
 * Author:		Singku
 * Paltform:		Linux Fedora Core 8 x86-32
 *	Compiler:		GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#include <string.h>
#include <libtaomee/log.h>

#include "net_if.h"
#include "initiate.h"
#include "proto.h"
#include "recv_data.h"

uint32_t memory_size = 67108864;

char *g_buffer[2] = {NULL, NULL};
char *g_buffer_ptr[2] = {NULL, NULL};
volatile uint32_t sum_data_in_mem[2] = {0, 0};
int g_buffer_tag = 0;

fcgi_store_t aresult;

char forward_buffer[1024];

static uint32_t proto_data_len;
//static uint32_t actual_data_len;
int recv_data(char *buf, uint32_t proto, uint32_t special_flag, uint32_t len)
{
	int tag = g_buffer_tag;

	if ((memory_size -(g_buffer_ptr[tag] - g_buffer[tag])) < sizeof(aresult))
		ERROR_RETURN(("MEMORY IS NOT ENOUGH"), 0);

	proto_data_len = len- sizeof(protocol_t);
	//actual_data_len = proto_data_len + sizeof(aresult.type) + sizeof(aresult.special_flag);

	aresult.type = proto;
	aresult.special_flag = special_flag;
	memcpy(&(aresult.project_number), buf, proto_data_len);
	memcpy(g_buffer_ptr[tag], &aresult, sizeof(aresult));

	sum_data_in_mem[tag] ++;
	g_buffer_ptr[tag] += sizeof(aresult);//actual_data_len;

	if (data_forward) {
		if (forward_fd == -1)
			forward_fd = connect_to_svr(forward_ip, forward_port, 1024, 1);
		if (forward_fd != -1) {
			init_proto_head(forward_buffer, len, proto, 0, 0, 0);
			char *ptr = forward_buffer + sizeof(protocol_t);
			memcpy(ptr, buf, proto_data_len);
			net_send(forward_fd, forward_buffer, len);
		}
	}

	return 0;
}
