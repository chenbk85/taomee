#include "lloccode.h"
#include "proto.h"
#include "curl_init.h"

#include "worker.h"

int post_llocc_cmd(uint32_t uid, uint16_t cmd, char *recvbuf, int rcvlen, char **sendbuf, int *sndlen)
{
	if (rcvlen != 12 + LLOCCODE_LEN + MAX_NAME_LEN){
		ERROR_LOG("recv post lloccode pkg len error len :%u",rcvlen);
		return -1;
	}

	int j = 0;
	recv_fileserv_t r_fileserv;
	r_fileserv.lloccode[LLOCCODE_LEN] = '\0';
	r_fileserv.filename[MAX_NAME_LEN] = '\0';
	UNPKG_H_UINT32(recvbuf, r_fileserv.albumid, j);
	UNPKG_H_UINT32(recvbuf, r_fileserv.hostid, j);
	UNPKG_H_UINT32(recvbuf, r_fileserv.filelen, j);
	UNPKG_STR(recvbuf, r_fileserv.lloccode, j, LLOCCODE_LEN);
	UNPKG_STR(recvbuf, r_fileserv.filename, j, MAX_NAME_LEN);
	DEBUG_LOG("albumid [%u] hostid[%u] filelen[%u] llocc[%s] filename[%s]",
				r_fileserv.albumid,
				r_fileserv.hostid,
				r_fileserv.filelen,
				r_fileserv.lloccode,
				r_fileserv.filename);

	char bus_url[URL_LEN]={0};
	web_result_t w_ret;
	char g_buff[1024];
	memset(&w_ret,0,sizeof w_ret);
	sprintf(bus_url, "%s?method=bus.album.addPhoto&uid=%u&hostid=%u&albumid=%u&len=%u&llocc=%s&file_name=%s", 
								web_serv_host,
								uid,
								r_fileserv.hostid,
								r_fileserv.albumid,
								r_fileserv.filelen,
								r_fileserv.lloccode,
								r_fileserv.filename);
	DEBUG_LOG("bus_url[%s]",bus_url);
	
	if (send_llocc(bus_url,&w_ret,json_callback) == 0) {
		DEBUG_LOG("send succ[%u %u %u %u ]",w_ret.ret,w_ret.errorid,w_ret.userid,w_ret.photoid);
		if(w_ret.ret ==0 && w_ret.errorid == 0){
			int k = sizeof(protocol_t);
			PKG_H_UINT32(g_buff,w_ret.photoid,k);
			init_proto_head(g_buff, uid, cmd, k);
			*sendbuf = g_buff;
			*sndlen = k;
			return 0;
		}else{
			int k = 0;	
			PKG_H_UINT32(g_buff,18,k);	//len
			PKG_H_UINT16(g_buff,cmd,k); //cmd
			PKG_H_UINT32(g_buff,uid,k); //uid
			PKG_H_UINT32(g_buff,0,k);	//seq
			PKG_H_UINT32(g_buff,w_ret.errorid,k);	//ret
			*sendbuf = g_buff;
			*sndlen = k;
			return 0;
		}
	}
	DEBUG_LOG("fail");
	int k = 0;
	PKG_H_UINT32(g_buff,18,k);	//len
	PKG_H_UINT16(g_buff,cmd,k);	//cmd
	PKG_H_UINT32(g_buff,uid,k);	//uid
	PKG_H_UINT32(g_buff,0,k);	//seq
	PKG_H_UINT32(g_buff,CONNECT_WEB_ERROR,k);	//ret
	*sendbuf = g_buff;
	*sndlen = k;
	return 0;
}

