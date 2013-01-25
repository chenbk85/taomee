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
#include <time.h>
#include "proto.h"
extern "C"{
#include "fcgi_proto.h"
#include "libtaomee/crypt/qdes.h"
#include "libtaomee/dataformatter/bin_str.h"
#include "libtaomee/conf_parser/config.h"
#include "log.h"
}
#include "tcpip.h"


Ctcp* conn_thumbserver = NULL;

int FCGI_get_thumb(uint32_t userid,uint32_t branchtype,uint32_t thumbid,uint32_t key){
	if (conn_thumbserver==NULL){
		if(-1 == config_init("/opt/taomee/cgi_conf/bench.conf")){
			HeaderContentType("image/jpeg");
			HeaderStatus(404,"Not Found");
			HeaderEnd;
			CGI_ERROR_LOG("read conf_file error");
			return FAIL;								    
		}
		conn_thumbserver = new Ctcp(config_get_strval("bind_thumb_ip"),config_get_intval("bind_thumb_port", 0),1,10);
	}

	time_t now_time;
	struct tm *ptr;
	now_time = time(NULL);
	ptr=gmtime(&now_time);
	char tmpbuf[128];
	strftime(tmpbuf, 128, "%a, %d %b %Y %T GMT",ptr);


	char tmp[256]={0};
	int j = PROTO_H_SIZE;
	PKG_H_UINT32(tmp, key, j);
	PKG_H_UINT32(tmp, thumbid, j);
	init_proto_head(tmp, userid, 1003, j);
	protocol_t* recv = NULL;
	uint32_t recvlen = 0;
	char* str;
	struct recv_thumb_pkg* prtp = NULL;
	char filename[32] = {0};
	//int i;
	int flag = conn_thumbserver->do_net_io((const char *)tmp, j, (char**)&recv, (int*)&recvlen);
	if((flag != SUCC) || (recv == NULL) || (recvlen != recv->len)){
		CGI_ERROR_LOG("cgi connect thumb server error :errorid:%d",flag);
		goto err;												            
	}
	str = (char*)recv + PROTO_H_SIZE;
	prtp = (struct recv_thumb_pkg *)str;
	CGI_DEBUG_LOG("RECV thumb_len:%u",prtp->thumb_len);
	if(prtp->thumb_len == 0){
		CGI_ERROR_LOG("thumbid error");
		goto err;
	}

	sprintf(filename,"%8X",thumbid);
	switch(branchtype){
	case JPG:
		HeaderContentType("image/jpeg");
		HeaderLastModified(tmpbuf);
		HeaderContentDisposition(filename);
		HeaderEnd;
		break;
	case PNG:
		HeaderContentType("image/png");
		HeaderLastModified(tmpbuf);
		HeaderContentDisposition(filename);
		HeaderEnd;
		break;
	case GIF:
		HeaderContentType("image/gif");
		HeaderLastModified(tmpbuf);
		HeaderContentDisposition(filename);
		HeaderEnd;
		break;
	default:
	    HeaderContentType("image/jpeg");
	    HeaderStatus(404,"Not Found");
		HeaderEnd;
		return FAIL;
	}
#ifndef _FASTCGI    
	fwrite(prtp->thumb_data, 1, prtp->thumb_len, stdout);
#else
	FCGI_fwrite(prtp->thumb_data, 1, prtp->thumb_len, FCGI_stdout);
#endif
	//delete conn_thumbserver;
	if (recv)
		free(recv);
	return SUCC;

err:
	HeaderContentType("text/html");
	HeaderStatus(404,"Not Found");
	HeaderEnd;
	//delete conn_thumbserver;
	if (recv)
		free(recv);
	return FAIL;
}












