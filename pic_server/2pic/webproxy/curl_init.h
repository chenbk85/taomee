/*
 * =====================================================================================
 *
 *       Filename:  curl_init.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年01月11日 14时22分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  james
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef _CURL_WEB_H
#define _CURL_WEB_H

#include <string.h>
#include <stdint.h>
#include <curl/curl.h>

#include "json_parser.h"

#define HOST_NAME_LEN	128
typedef enum{
	success = 1,
	code,
	uid,
	photoid
}web_r_type;

typedef struct _buffer {
	int	len;
	char 	data[256];
}__attribute__((packed)) buffer_t; 

typedef struct _web_result {
	uint32_t	ret;
	uint32_t	errorid;
	uint32_t	userid;
	uint32_t	photoid;
}__attribute__((packed)) web_result_t; 

extern char web_serv_host[HOST_NAME_LEN];

int curl_init(CURL** pconn, char* url, buffer_t* p_buffer);
int writer(char *data, uint32_t size, uint32_t nmemb, buffer_t *write_data);
int send_llocc(char *bus_url,web_result_t* wet_ret,int (*p_callback)(void *ctx, int type, const JSON_value 
*value));
int json_callback(void *ctx, int type, const JSON_value *value);


#endif
