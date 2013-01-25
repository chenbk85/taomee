#ifndef _FASTCGI
#include <stdio.h>
#else
#include "fcgi_stdio.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "proto.h"
#include <time.h>
extern "C"{
#include "log.h"
#include "libtaomee/crypt/qdes.h"
#include "libtaomee/dataformatter/bin_str.h"
#include "fcgi_proto.h"
}
#include "fcgi_pic.h"

int parser_session_key(const char* session_key,uint32_t* cmdid,uint8_t body[]){
	uint8_t tmp[256];
	uint8_t decry_date[256];
	memset(tmp, 0, sizeof(tmp));
	int i =0;
	int session_len = strlen(session_key);
	int tmp_len;

	if (session_len % 2 == 0 && session_len > 8)
		tmp_len = session_len / 2;
   	else
		return FAIL;

	int decry_len = (tmp_len % 8)?(tmp_len/8 + 1):(tmp_len/8);

	str2hex(session_key, tmp_len * 2, tmp);
	des_decrypt_n(KEY,tmp,decry_date,decry_len);
	UNPKG_H_UINT32(decry_date,*cmdid,i);
	UNPKG_STR(decry_date,body,i,tmp_len - 4);
	return SUCC;
}
int main(void){
	char lloccode[65];
	uint32_t cmdid = 0;
	uint32_t userid = 0;
	uint32_t albumid = 0;
	uint32_t thumbid = 0;
	uint32_t branchtype = 0;
	int fcgi_req_cnt = 0;
	char sess_key_buf[512];
	uint8_t body[1024];
	log_init("/cgi/log/",(log_lvl_t)8,log_size,0,"");
	CGI_DEBUG_LOG("fastcgi begin");
	while(FCGI_Accept() >= 0){	
		fcgi_req_cnt ++;
		CGI_DEBUG_LOG("Request number %d running on host %s Process ID: %d",fcgi_req_cnt, getenv("SERVER_NAME"), getpid());
		char *r_method=getenv("REQUEST_METHOD");
		if(r_method == NULL || (strncmp(r_method,"GET",3) != 0)){
			CGI_ERROR_LOG("Request method error.[%s]",r_method);
			HeaderContentType("text/html");
			HeaderStatus(404,"Not Found");
			HeaderEnd;	
			FCGI_Finish();
			continue;
		}

		char* request=getenv("QUERY_STRING");
		if (request == NULL){
			HeaderContentType("text/html");
			HeaderStatus(404,"Not Found");
			HeaderEnd;
			FCGI_Finish();
			continue;			
		}

		sess_key_buf[0] = '\0';
		memset(body, 0, sizeof(body));
		sscanf(request,"session_key=%511s",sess_key_buf);
		if (sess_key_buf[0] == '\0'){
			CGI_ERROR_LOG("Request string error.[%s]", request);
			HeaderContentType("text/html");
			HeaderStatus(404,"Not Found");
			HeaderEnd;
			FCGI_Finish();
			continue;
		}

		if(parser_session_key(sess_key_buf, &cmdid, body) == FAIL)
		{
			CGI_ERROR_LOG("parser session_key error.sess_key_buf:[%s]",sess_key_buf);
			HeaderContentType("text/html");
			HeaderStatus(404,"Not Found");
			HeaderEnd;
			FCGI_Finish();
			continue;
		}
		switch(cmdid){
		case proto_get_image:
		{
			int ret = 0;
			int j =0;
			uint32_t ip = 0;
			uint32_t Time = 0;
			
			//UNPKG_H_UINT32(body,ip,j);
			//UNPKG_H_UINT32(body,time,j);
			char *modified=getenv("HTTP_IF_MODIFIED_SINCE");
			if(NULL != modified){
				HeaderStatus(304,"Not Modified")
				HeaderLastModified(modified);	
				HeaderEnd;
				FCGI_Finish();
				break;
			}			
			UNPKG_STR(body,lloccode,j,64);
			lloccode[64] = '\0';
			ret = FCGI_GET_IMAG_Main(lloccode,ip,Time);
			FCGI_Finish();
			break;
		}
		case proto_get_thumb:
		{
			int j = 0 ;
			uint32_t ip = 0;
			uint32_t Time = 0;
			char *modified=getenv("HTTP_IF_MODIFIED_SINCE");

			if(NULL != modified){
				HeaderStatus(304,"Not Modified");
				HeaderLastModified(modified);
				HeaderEnd;
				FCGI_Finish();
				break;
			}
			
			UNPKG_H_UINT32(body,userid,j);
		//	UNPKG_H_UINT32(body,ip,j);
		//	UNPKG_H_UINT32(body,time,j);
			UNPKG_H_UINT32(body,albumid,j);
			UNPKG_H_UINT32(body,thumbid,j);
			UNPKG_H_UINT32(body,branchtype,j);
			CGI_DEBUG_LOG("GET_THUMB SEND[%u %u %u %u %u %u]",userid,ip,Time,albumid,thumbid,branchtype);
			FCGI_get_thumb(userid,branchtype,thumbid,albumid);
			FCGI_Finish();
			break;
		}
		case proto_change_thumb:
		{
			int j = 0;
			uint32_t i =0;
			struct fcgi_recv_webclient recv;
			memset(&recv,0,sizeof(struct fcgi_recv_webclient));
			UNPKG_H_UINT32(body,recv.userid,j);
			UNPKG_H_UINT32(body,recv.key,j);
//			UNPKG_H_UINT32(body,recv.ip,j);
//			UNPKG_H_UINT32(body,recv.time,j);
			UNPKG_STR(body,recv.lloccode,j,64);
			UNPKG_H_UINT32(body,recv.cnt,j);
			CGI_DEBUG_LOG("CHANGE_THUMB SEND [%u %u %s %u]",
							recv.userid,
							recv.key,
//							recv.ip,
//							recv.time,
							recv.lloccode,
							recv.cnt);
			for(i=0;i<recv.cnt && i<CNT_MAX;i++){
				UNPKG_H_UINT32(body,recv.thumb_arg[i].W,j);
				UNPKG_H_UINT32(body,recv.thumb_arg[i].H,j);
				UNPKG_H_UINT32(body,recv.thumb_arg[i].start_x,j);
				UNPKG_H_UINT32(body,recv.thumb_arg[i].start_y,j);
				UNPKG_H_UINT32(body,recv.thumb_arg[i].thumb_x,j);
				UNPKG_H_UINT32(body,recv.thumb_arg[i].thumb_y,j);
				CGI_DEBUG_LOG("[%u %u %u %u %u %u]",
								recv.thumb_arg[i].W,
								recv.thumb_arg[i].H,
								recv.thumb_arg[i].start_x,
								recv.thumb_arg[i].start_y,
								recv.thumb_arg[i].thumb_x,
								recv.thumb_arg[i].thumb_y);
			}
			FCGI_change_thumb(&recv);
			FCGI_Finish();
			break;	
		}
		default:
			CGI_ERROR_LOG("cmdid error");
			HeaderContentType("text/html");
			HeaderStatus(404,"Not Found");
			HeaderEnd;
			FCGI_Finish();
			break;
		} // swich
	} // while
	return 0;
}

















