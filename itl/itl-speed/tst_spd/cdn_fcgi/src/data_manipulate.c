/*
 * data_manipulate.c
 *
 *  Created on:	2011-7-14
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <fcgi_stdio.h>

#include "cgic.h"
#include "data_manipulate.h"
#include "log.h"
#include "choose_server.h"
#include "initiate.h"
#include "protocol.h"

#ifdef		ENCODE_
#include "qdes.h"
#endif

static uint8_t  pic_empty_gif[] = {

    'G', 'I', 'F', '8', '9', 'a',  /* header                                 */

                                   /* logical screen descriptor              */
    0x01, 0x00,                    /* logical screen width                   */
    0x01, 0x00,                    /* logical screen height                  */
    0x80,                          /* global 1-bit color table               */
    0x01,                          /* background color #1                    */
    0x00,                          /* no aspect ratio                        */

                                   /* global color table                     */
    0x00, 0x00, 0x00,              /* #0: black                              */
    0xff, 0xff, 0xff,              /* #1: white                              */

                                   /* graphic control extension              */
    0x21,                          /* extension introducer                   */
    0xf9,                          /* graphic control label                  */
    0x04,                          /* block size                             */
    0x01,                          /* transparent color is given,            */
                                   /*     no disposal specified,             */
                                   /*     user input is not expected         */
    0x00, 0x00,                    /* delay time                             */
    0x01,                          /* transparent color #1                   */
    0x00,                          /* block terminator                       */

                                   /* image descriptor                       */
    0x2c,                          /* image separator                        */
    0x00, 0x00,                    /* image left position                    */
    0x00, 0x00,                    /* image top position                     */
    0x01, 0x00,                    /* image width                            */
    0x01, 0x00,                    /* image height                           */
    0x00,                          /* no local color table, no interlaced    */

                                   /* table based image data                 */
    0x02,                          /* LZW minimum code size,                 */
                                   /*     must be at least 2-bit             */
    0x02,                          /* block size                             */
    0x4c, 0x01,                    /* compressed bytes 01_001_100, 0000000_1 */
                                   /* 100: clear code                        */
                                   /* 001: 1                                 */
                                   /* 101: end of information code           */
    0x00,                          /* block terminator                       */

    0x3B                           /* trailer                                */
};

int connect_2_server(uint32_t serverip, uint16_t port, int *sockfd, char *str)
{
	struct sockaddr_in server;
	server.sin_addr.s_addr = htonl(serverip);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	bzero(&(server.sin_zero),sizeof(server.sin_zero));

	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		write_log("connect %s server:create socket failed!%s\n", str, strerror(errno));
		*sockfd = -1;
		return -1;
	}

	if (connect(*sockfd, (struct sockaddr*)(&server),sizeof(server)) == -1) {
		write_log("connect %s server failed!%s\n", str, strerror(errno));
		close(*sockfd);
		*sockfd = -1;
		return -1;
	}
	write_log("connect %s server ok!\n", str);
	return 0;
}

int connect_2_analyze_server(int *sockfd)
{
	static int ana_retry_count = 0;
	uint32_t serverip;
	if (if_choose_server)
		serverip = choose_server();
	else
		serverip = analyze_server_set->ipvalue;

	if (connect_2_server(serverip,server_analyze_port, sockfd, "analyze") == -1) {
		ana_retry_count ++;
		if (ana_retry_count == 3) {
			write_log("Connect to analyze server:retry 3 times,all failed, server maybe done or get bad config file!\n");
			ana_retry_count = 0;
			if (initiate(NULL) == -1)//重新读取配置文件
				return -1;
		}
		return -1;
	}

	return 0;
}

int connect_2_backup_server(int *sockfd)
{
	static int bkp_retry_count = 0;

	if (connect_2_server(data_backup_server, data_backup_port, sockfd, "backup") == -1) {
		bkp_retry_count ++;
		if (bkp_retry_count == 3) {
			write_log("Connect to bkp server:retry 3 times,failed, server maybe done or get bad config file!\n");
			bkp_retry_count = 0;
			if (initiate(NULL) == -1)//重新读取配置文件
				return -1;
		}
		return -1;
	}

	return 0;
}

//公共数据
static char type[MAX_POST_STRING_LEN] = {0};//类型
static char user_id[MAX_POST_STRING_LEN] = {0};	//米米号
static char project_number[MAX_POST_STRING_LEN] = {0};//项目号
static char client_ip[MAX_POST_STRING_LEN] = {0};//客户端IP地址
//CDN数据
static char cdn_ip[MAX_POST_STRING_LEN] = {0};//CDNIP地址
static char speed[MAX_POST_STRING_LEN] = {0};//下载速度 Byte/s
//URL数据
static char str[MAX_POST_STRING_LEN];
static char spent_time[MAX_POST_STRING_LEN]={0};//两次断点间隔时间

int prepare_data(store_result_t *result)
{
	struct in_addr inp;

	FCGI_fwrite(pic_empty_gif, 1, sizeof(pic_empty_gif), FCGI_stdout);

	//取得公共数据
	cgiFormString("Type", type, (MAX_POST_STRING_LEN - 1));
	cgiFormString("UserID", user_id, (MAX_POST_STRING_LEN - 1));
	cgiFormString("ProjNo", project_number, (MAX_POST_STRING_LEN - 1));
	cgiFormString("Client", client_ip, (MAX_POST_STRING_LEN - 1));
	if (0 == inet_aton(client_ip,&inp)) {
		snprintf(client_ip,sizeof(client_ip), "%s", cgiRemoteAddr);
		if (0 == inet_aton(client_ip,&inp))
			return -1;
	}
	result->client_ip = ntohl(inp.s_addr);
	result->user_id = strtoul(user_id, NULL, 10);
	result->project_number = strtoul(project_number, NULL, 10);
	result->test_time = time((time_t*)NULL);
	result->type = strtoul(type, NULL, 10);
	if (result->type == TYPE_CDN) {
		//cdn
		cgiFormString("CDNIP",cdn_ip,(MAX_POST_STRING_LEN - 1));
		if (0 == inet_aton(cdn_ip, &inp))
			return -1;
		result->record.m_cdn.cdn_ip = ntohl(inp.s_addr);

		cgiFormString("Rate",speed,(MAX_POST_STRING_LEN - 1));
		result->record.m_cdn.speed = strtoul(speed, NULL, 10);
		if (if_log_data) {
			write_log("User:%u,Proj:%u,Client:%u,CDN:%u,Speed:%u\n",
						result->user_id,
						result->project_number,
						result->client_ip,
						result->record.m_cdn.cdn_ip,
						result->record.m_cdn.speed);
		}
	} else if (result->type == TYPE_URL) {
		//url
		uint32_t count;
		cgiFormString("Count", str, (MAX_POST_STRING_LEN - 1));
		count = strtoul(str, NULL, 10);
		if (count == 0 || count > MAX_POINT)
			return -1;
		result->record.m_url.count = count;

		uint32_t page_id;
		cgiFormString("PageID", str, (MAX_POST_STRING_LEN - 1));
		page_id = strtoul(str, NULL, 10);
		if (page_id == 0)
			return -1;
		result->record.m_url.page_id = page_id;

		char spent[MAX_POST_STRING_LEN];
		int i;
		for (i = 1; i <= count; i++) {
			snprintf(spent, sizeof(spent), "T%d", i);
			cgiFormString(spent, spent_time, (MAX_POST_STRING_LEN - 1));
			result->record.m_url.vlist[ i-1 ] = strtoul(spent_time, NULL, 10);
		}
		if (if_log_data) {
			int k = 0, n = 0;
			char str[1024];
			for (k = 0; k < result->record.m_url.count; k++) {
				n += sprintf(str + n, " %u", result->record.m_url.vlist[k]);
			}
			write_log("User:%u,Client:%u,Proj:%u,Page:%u,Count:%u,List:%s\n",
						result->user_id,
						result->client_ip,
						result->project_number,
						result->record.m_url.page_id,
						result->record.m_url.count,
						str);
		}
	} else
		return -1;

	return 0;
}

static char buffer[8192];

int send_2_server(store_result_t *result, int *sockfd, char *str, uint32_t flag)
{
	char *buf_ptr;

	//sent to server
	int send_len = 0;
	int total_send_len = 0;
	int data_len;
	int effective_len;
	buf_ptr = buffer;

	if (result->type == TYPE_CDN) {
		data_len =	sizeof(protocol_t) +
						sizeof(fcgi_common_t) +
						sizeof(cdn_speed_t);
		effective_len = data_len - sizeof(protocol_t);
		init_proto_head(buf_ptr, data_len, proto_fcgi_cdn_report, 0, flag, 0);
	}
	else if (result->type == TYPE_URL) {
		data_len =	sizeof(protocol_t) +
						sizeof(fcgi_common_t) +
						sizeof(result->record.m_url.count) +
						sizeof(result->record.m_url.page_id) +
						result->record.m_url.count * sizeof(result->record.m_url.vlist[0]);
		effective_len = data_len - sizeof(protocol_t);
		init_proto_head(buf_ptr, data_len, proto_fcgi_url_report, 0, flag, 0);
	}
	else
		return 0;

	buf_ptr += sizeof(protocol_t);
	memcpy(buf_ptr, &result->project_number, effective_len);

	while ((send_len = send(*sockfd, buffer+total_send_len, (data_len-total_send_len), 0) )
			< (data_len - total_send_len) ) {
		if (send_len == -1) {
			if (errno == EINTR)
				continue;
			write_log("send 2 %s server error:%s\n", str, strerror(errno));
			close(*sockfd);
			*sockfd = -1;
			return -1;
		}
		else
			total_send_len += send_len;
	}
	return 0;
}
