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
extern "C"{
#include "fcgi_proto.h"
#include "libtaomee/crypt/qdes.h"
#include "libtaomee/dataformatter/bin_str.h"
#include "libtaomee/conf_parser/config.h"
#include "log.h"
#include "json/json.h"
}
#include "tcpip.h"

void fcgi_print_result(struct fcgi_recv_fileser* recv_data,int result,uint32_t reason_id)
{
	struct json_object *my_object;
	struct json_object *sub_array;
	uint32_t i = 0;
	my_object = json_object_new_object();
	sub_array = json_object_new_array();
	char str_tmp[32];
	if(result == FAIL){
		json_object_object_add(my_object,"result",json_object_new_string("fail"));
		json_object_object_add(my_object,"reason",json_object_new_int(reason_id));
		goto PRINT_END;
	}
	if(recv_data == NULL){
		json_object_object_add(my_object,"result",json_object_new_string("fail"));
		json_object_object_add(my_object,"reason",json_object_new_int(reason_id));
		goto PRINT_END;
	}

	json_object_object_add(my_object,"result",json_object_new_string("success"));
	json_object_object_add(my_object,"key",json_object_new_int(recv_data->key));
	json_object_object_add(my_object,"lloccode",json_object_new_string(recv_data->lloccode));
	json_object_object_add(my_object,"cnt",json_object_new_int(recv_data->cnt));
	if(recv_data->cnt == 0){
		goto PRINT_END;
	}
	for(i = 0;i < recv_data->cnt && i < CNT_MAX; i++){
		sprintf(str_tmp,"%u",recv_data->Thumbid[i]);
		json_object_array_add(sub_array, json_object_new_string(str_tmp));
	}
	json_object_object_add(my_object,"thumbid",sub_array);
PRINT_END:
	printf("%s\n",json_object_to_json_string(my_object));
	json_object_put(sub_array);
	json_object_put(my_object);
	return ;
}


Ctcp* conn_fileserver = NULL;

int FCGI_change_thumb(struct fcgi_recv_webclient *recv_pkg){
	HeaderContentType("text/html");
	HeaderEnd;
	if (conn_fileserver==NULL){
		if(-1 == config_init("/opt/taomee/cgi_conf/bench.conf")){
			CGI_ERROR_LOG("read conf_file error");
			fcgi_print_result(NULL,FAIL,4000);
			return FAIL;								    
		}
		conn_fileserver = new Ctcp(config_get_strval("bind_fileserver_ip"),config_get_intval("bind_fileserver_port", 0),1,10);
	}

	char tmp[256]={0};
	int j = PROTO_H_SIZE;
	PKG_H_UINT32(tmp,recv_pkg->key,j);
	PKG_STR(tmp,recv_pkg->lloccode,j,64);
	PKG_H_UINT32(tmp,recv_pkg->cnt,j);
	uint32_t i =0;
	for(i=0;i<recv_pkg->cnt && i<CNT_MAX;i++){
		PKG_H_UINT32(tmp,recv_pkg->thumb_arg[i].W,j);
		PKG_H_UINT32(tmp,recv_pkg->thumb_arg[i].H,j);
		PKG_H_UINT32(tmp,recv_pkg->thumb_arg[i].start_x,j);
		PKG_H_UINT32(tmp,recv_pkg->thumb_arg[i].start_y,j);
		PKG_H_UINT32(tmp,recv_pkg->thumb_arg[i].thumb_x,j);
		PKG_H_UINT32(tmp,recv_pkg->thumb_arg[i].thumb_y,j);
	}
	init_proto_head(tmp, recv_pkg->userid, 2, j);
	protocol_t* recv = NULL;
	uint32_t recvlen = 0;
	struct fcgi_recv_fileser* prtp = NULL;
	//int i;
	int flag = conn_fileserver->do_net_io((const char *)tmp, j, (char**)&recv, (int*)&recvlen);
	if((flag != SUCC) || (recv == NULL) || (recvlen != recv->len)){
		CGI_ERROR_LOG("cgi connect fileserver error :errorid:%d",flag);
		fcgi_print_result(NULL,FAIL,4007);
		if(recv){
			free(recv);
		}
		return FAIL;								            
	}
	prtp = (struct fcgi_recv_fileser *)((char*)recv+PROTO_H_SIZE);
	prtp->lloccode[Lloccode_Len] = '\0';
	CGI_DEBUG_LOG("RECV[%u %s %u]",prtp->key,prtp->lloccode,prtp->cnt);
	fcgi_print_result(prtp,SUCC,0);
	if(recv){
		free(recv);
	}
	return SUCC;

}











