/*
 * =====================================================================================
 *
 *       Filename:  process.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/07/2010 04:05:52 PM CST
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
#include <ghttp.h>
#include <stdlib.h>
#include <stdio.h>
#include <libtaomee/timer.h>
}

#include "process.hpp"
#include "mem.hpp"

struct timer_head_t {
    list_head_t timer_list;
}__attribute__((packed));
/*定时器链表*/
static timer_head_t post_tmr;
/*一次发送数据的次数*/
static int count = 0;
/*两次发送数据的时间间隔*/
static int diff = 60;
/*两次发的数据包时间间隔*/
static int pkg_diff = 1;
/*指向对象池的指针*/
static class Mem *p_mem;

/* @brierf 把二进制数据转换成十六进制字符串
 */
static void bin2hex(char *dst, char *src, int len, int max_len = 0XFFFF)
{
	int hex;
	int i;
	int di;
	if (len > max_len) {
		len = max_len;
	}
	for (i = 0;i < len; i++) {
		hex = ((unsigned char)src[i]) >> 4;
		di = i*2;
		dst[di] = hex < 10 ? '0' + hex : 'A' - 10 + hex ;
		hex = ((unsigned char)src[i]) & 0x0F;
		dst[di+1] = hex < 10 ? '0' + hex : 'A' - 10 + hex ;
	}
	dst[len*2] = 0;
}

/* @brief 把client的数据，以十六进制字符串打包到内存块中
 */
static int fill_data_to_mem_block(void *data, struct mem_block *ptr)
{
	uint8_t cli_data[8096];
	proto_header_t *header = (proto_header_t *)data;
	uint32_t body_len = header->proto_len - sizeof(proto_header_t);
	ptr->uid = header->uid;
	ptr->cmd = header->cmd_id;
	memcpy(cli_data + ptr->size, &ptr->uid, sizeof(ptr->uid));
	ptr->size += sizeof(ptr->uid);
	memcpy(cli_data + ptr->size, &ptr->cmd, sizeof(ptr->cmd));
	ptr->size += sizeof(ptr->cmd);
	memcpy(cli_data + ptr->size, (char *)data + sizeof(proto_header_t), body_len);
	ptr->size += body_len;
	bin2hex((char *)ptr->data, (char *)cli_data, ptr->size);
	ptr->size *= 2;
	return 0;
}

/* @brief 把client传过来的数据加到使用链表中
 */
int process(void *data, fdsession_t* fdsess)
{
	static uint32_t i = 0;
	int len = *(uint32_t *)data;
	struct mem_block *ptr = p_mem->get_free_block( 2 * len);
	if (ptr == NULL) {
		ERROR_RETURN(("GET BLOCK FAIL"), 0);
	}
	i++;
	fill_data_to_mem_block(data, ptr);
	return 0;
}

/* @brief 发送信息给webservice
 * @param url webservice的链接地址 
 * @param data 发送的数据
 * @param len 发送的数据的长度的
 * @return 0表示发送失败，1表示发送成功
 */
static int post(char *url, char data[], int len) 
{
	ghttp_request *request = NULL;
	ghttp_status status;

	request = ghttp_request_new();
	if (ghttp_set_uri(request, url) == -1) {
		ERROR_LOG("GHTTP_SET_URL");
		return 0;
	}
	if (ghttp_set_type(request, ghttp_type_post) == -1) {
		ERROR_LOG("GHTTP_SET_TYPE");
		return 0;
	}
	ghttp_set_body(request, data, len);
	ghttp_prepare(request);
	status = ghttp_process(request);
	if(status == ghttp_error) {
		return 0;
	}
	/* OK, done */
	//DEBUG_LOG("status code %d %s", ghttp_status_code(request), data);
	return 1;
}

/* @brief 从已使用链表中，得到数据，并且post数据给webservice服务器
 * @param url webservice的链接
 */
static int send_request_to_school_bar(char *url)
{
	struct mem_block *ptr = p_mem->get_used_block();
	if (ptr == NULL) {
		DEBUG_LOG("ptr is null");
		return 0;
	}
	if (!post(url, (char *)ptr->data, ptr->size)) {
		DEBUG_LOG("POST ERROR UID %u CMD %u DATA %s", ptr->uid, ptr->cmd, ptr->data);	
		p_mem->back_to_used_heap(ptr);
	}
	return 1;
}

/* @brief 定时发送数据的函数
 */
static int send_to_school_bar_timer_fun(void *owner, void *data)
{
	/*如果空闲链表过长，释放一定数目的对象*/
	p_mem->release_surplus_free_block();
	/*循环发送数据*/
	struct timeval diff_time;
	diff_time.tv_sec = 0;
	diff_time.tv_usec = pkg_diff;

	int i = 0;
	for (i = 0; i < count; i++) {
		select(0, NULL, NULL, NULL, &diff_time);
		if (!send_request_to_school_bar(config_get_strval("url"))) {
			break;
		}
	}
	/*重新启动定时器*/
	ADD_TIMER_EVENT(&post_tmr, send_to_school_bar_timer_fun, NULL, get_now_tv()->tv_sec + diff);
	return 0;
}

/* @brief 初始化变量和链表，并启动定时器
 */
int process_init()
{
	/*每次定时发送数据的个数*/
	count = config_get_intval("max_count", 1000);
	/*定时器的时间间隔*/
	diff = config_get_intval("timer_diff_time", 60);
	/*两次发送数据包的时间间隔*/
	pkg_diff = config_get_intval("pkg_diff_time", 1);
	/*初始化定时器列表*/
	INIT_LIST_HEAD(&post_tmr.timer_list);
	/*启动定时发送程序*/
	ADD_TIMER_EVENT(&post_tmr, send_to_school_bar_timer_fun, NULL, get_now_tv()->tv_sec + diff);
	p_mem = new Mem;
	/*对象池的初始话*/
	p_mem->mem_init();
	return 0;
}

int process_fini()
{
	p_mem->save_used_heap();
	return 0;
}
