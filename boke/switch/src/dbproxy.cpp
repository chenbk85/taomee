#include <cassert>
#include <cstring>

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>

#include <async_serv/async_serv.h>
}
#include <libtaomee++/proto/Ccmd_map.h>

#include "dbproxy.hpp"
#include "online.hpp"
//-------------------------------------------------------------
//对应的结构体
#include "./proto/pop_db.h"

//函数定义
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    void proto_name( fdsession_t* fdsess, uint32_t id, uint32_t seq, Cmessage* c_in, uint32_t ret ) ;
#include "./proto/pop_db_bind_for_switch.h"


//-------------------------------------------------------------
//命令绑定
typedef   void (*P_DEALFUN_T)( fdsession_t* fdsess, uint32_t id, uint32_t seq, Cmessage* c_out, uint32_t ret );

#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    {cmdid, new (c_out), md5_tag,bind_bitmap ,proto_name },

Ccmd<P_DEALFUN_T> g_proxy_cmd_list[]={
#include "./proto/pop_db_bind_for_switch.h"
};
//命令map
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_proxy_cmd_map;

void init_db_handle_funs()
{
    g_proxy_cmd_map.initlist(g_proxy_cmd_list,sizeof(g_proxy_cmd_list)/sizeof(g_proxy_cmd_list[0]));
}

int proxysvr_fd = -1;

int send_request_to_db(userid_t id, uint32_t seq, uint16_t cmd, const void* body_buf, int body_len)
{
	assert(body_len >= 0);

	static uint8_t dbbuf[pkg_size];

	if (proxysvr_fd == -1) {
		proxysvr_fd = connect_to_service(config_get_strval("dbproxy_ip"), 0, 65535, 1);
	}

	uint32_t len = sizeof(svr_proto_t) + body_len;
	if ((proxysvr_fd == -1) || 	(len > sizeof(dbbuf))) {
		ERROR_LOG("send to dbproxy failed: fd=%d len=%d", proxysvr_fd, len);
		return -1;
	}

	svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(dbbuf);
	pkg->len = len;
	pkg->seq = seq;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	memcpy(pkg->body, body_buf, body_len);

	return net_send(proxysvr_fd, dbbuf, len);
}

void handle_db_return(svr_proto_t* dbpkg, uint32_t pkglen)
{
	if (!dbpkg->seq)
		return;

	uint32_t waitcmd = dbpkg->seq & 0xFFFF;
	int      connfd  = dbpkg->seq >> 16;
    Ccmd< P_DEALFUN_T> * p_cmd_item =g_proxy_cmd_map.getitem( dbpkg->cmd );
    KDEBUG_LOG(dbpkg->id, "SI\t[c=%u 0x%04X ret=%u]", waitcmd, dbpkg->cmd, dbpkg->ret);
	if (p_cmd_item == NULL)
		return;

	if (user_cmd_list.find(connfd) == user_cmd_list.end()) {
		KERROR_LOG(dbpkg->id, "no find fd\t[%d]", connfd);
		return;
	}

	if (!user_cmd_list[connfd].is_waiting(dbpkg->id, waitcmd)) {
		KERROR_LOG(dbpkg->id, "no waiting\t[%u]", waitcmd);
		return;
	}

	Cmessage * msg = NULL;
	if (dbpkg->ret==0){//成功
		if (!p_cmd_item->proto_pri_msg->read_from_buf_ex(
					(char *)dbpkg->body, dbpkg->len - sizeof(*dbpkg))){
			ERROR_LOG("err: pkglen error");
			return;
		}
		msg=p_cmd_item->proto_pri_msg;
	}
	uint32_t cli_seq = user_cmd_list[connfd].remove_waiting(dbpkg->id, waitcmd);
	p_cmd_item->func(user_cmd_list[connfd].fdsess, dbpkg->id, cli_seq, msg, dbpkg->ret);
}

void pop_get_base_info( fdsession_t* fdsess, uint32_t id, uint32_t seq,  Cmessage* c_out, uint32_t ret)
{
	uint32_t last_online_id = 0;
	if (!ret) {
		pop_get_base_info_out* p_out = P_OUT;
		last_online_id = p_out->last_online_id;
	}

	KDEBUG_LOG(id, "SEND RECOMM SVR\t[%u %u]", last_online_id, seq);

	Online::send_recommend_svr(fdsess, id, seq, last_online_id);
}
