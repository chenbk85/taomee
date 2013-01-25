extern "C" {
#include <stdint.h>
#include <libtaomee/log.h>
}

#include <libtaomee++/proto/proto_util.h>
#include "proto.hpp"

#include "dispatcher.h"
#include "global.h"
#include "timer_func.h"

int check_msg(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct report_msg {
		uint32_t			gameid;
		uint32_t			userid;
		uint32_t			recvid;
		uint32_t			onlineid;
		uint32_t			maptype;
		uint32_t			mapid;
		uint32_t			timestamp;
		uint32_t			msglen;
		char				msg[];	
	}__attribute__((packed));
	

	if (bodylen <= sizeof(report_msg)) {
		KERROR_LOG(pkg->id, "invalid len\t[%u]", bodylen);
		return 0;
	}

	report_msg* pmsg = (report_msg *)pkg->body;
	if (pmsg->msglen + sizeof(report_msg) != bodylen) {
		KERROR_LOG(pkg->id, "invalid len\t[%u %u]", bodylen, pmsg->msglen);
		return 0;
	}

	if (pmsg->recvid > 50000) {
		KDEBUG_LOG(pmsg->userid, "no public chat\t[%u]", pmsg->recvid);
		return 0;
	}

	std::map<uint32_t, CChatCheck*>::iterator it = game_chat_check.find(pmsg->gameid);
	if (it == game_chat_check.end()){
		ERROR_LOG("invalid gameid\t[%u]", pmsg->gameid);
		return 0;
	}

	CChatCheck* pcheck = it->second;

	if (pcheck->chat_forbid.is_forbidden(pmsg->userid)) {
		KDEBUG_LOG(pmsg->userid, "FORBIDEN");
		return 0;
	}

	bool check_ret = pcheck->check_msg(pmsg->userid, pmsg->timestamp, pmsg->msg, pmsg->msglen);

	if (!check_ret) {
		pcheck->chat_forbid.send_pkg_forbid_user(pmsg->userid, pcheck->chat_protocol, pcheck->send_val);
		uint32_t addtime = pcheck->chat_forbid.add_forbid_user(pmsg->userid);
		if (addtime) {
			ADD_ONLINE_TIMER(&g_events, n_chat_forbid_pop, pcheck, addtime);
		}
	}

	char buff[4096];
	int len = pmsg->msglen >= 4096 ? 4095 : pmsg->msglen;
	memcpy(buff, pmsg->msg, len);
	buff[len] = '\0';

	KINFO_LOG(pmsg->userid, " %u %u %s %d", pmsg->gameid, pmsg->timestamp, buff, (int)(!check_ret));

	return 0;
}

int check_msg_common(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct report_msg_common {
		uint32_t			gameid;
		uint32_t			userid;
		uint32_t			recvid;
		uint32_t			onlineid;
		uint32_t			timestamp;
		uint32_t			msglen;
		char				msg[];	
	}__attribute__((packed));

	if (bodylen <= sizeof(report_msg_common)) {
		KERROR_LOG(pkg->id, "invalid len\t[%u]", bodylen);
		return 0;
	}

	report_msg_common* pmsg = (report_msg_common *)pkg->body;
	if (pmsg->msglen + sizeof(report_msg_common) != bodylen) {
		KERROR_LOG(pkg->id, "invalid len\t[%u %u %lu]", bodylen, pmsg->msglen, sizeof(report_msg_common));
		return 0;
	}

	if (pmsg->recvid > 50000) {
		KDEBUG_LOG(pkg->id, "no public chat\t[%u]", pmsg->recvid);
		return 0;
	}

	std::map<uint32_t, CChatCheck*>::iterator it = game_chat_check.find(pmsg->gameid);
	if (it == game_chat_check.end()){
		ERROR_LOG("invalid gameid\t[%u]", pmsg->gameid);
		return 0;
	}

	CChatCheck* pcheck = it->second;

	if (pcheck->chat_forbid.is_forbidden(pkg->id)) {
		KDEBUG_LOG(pkg->id, "has forbidden in 5 min");
		return 0;
	}

	if (!pcheck->check_msg(pkg->id, pmsg->timestamp, pmsg->msg, pmsg->msglen)) {
		//pcheck->chat_forbid.send_pkg_forbid_user(pmsg->userid, pcheck->chat_protocol, pcheck->send_val);
		/*
		uint32_t addtime = pcheck->chat_forbid.add_forbid_user(pkg->id);
		KDEBUG_LOG(pkg->id, "AAAAAAA\t[%u]", addtime);
		if (addtime) {
			ADD_ONLINE_TIMER(&g_events, n_chat_forbid_pop, pcheck, addtime);
		}
		*/
	}

	return 0;
}

typedef int (*func_t)(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

func_t funcs[50000];

void init_funcs()
{
	funcs[adm_check_msg - 60001]       = check_msg;
	funcs[adm_check_msg_common - 60001] = check_msg_common;
}

int dispatch(void* data, fdsession_t* fdsess)
{
	svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(data);

	if (false)
	{
        char outbuf[13000];
        bin2hex(outbuf,(char*)data,pkg->len  ,2000);
        KDEBUG_LOG(pkg->id,"CI[%u][%s]", pkg->cmd,outbuf  );
    }

	int fd = fdsess ? fdsess->fd : 0;
	uint32_t ip = fdsess ? get_remote_ip(fd) : 0;

	DEBUG_LOG("userid:%u,cmdid:%u len:%u ip:%u fd:%d",pkg->id, pkg->cmd, pkg->len, ip, fd);
	int cmd = pkg->cmd - 60001;
	if ((cmd < 1) || (cmd > 50000) || (funcs[cmd] == 0)) {
		ERROR_LOG("invalid cmd=%u from fd=%d", pkg->cmd, fdsess ? fdsess->fd : 0);
		return -1;
	}

	return funcs[cmd](pkg, pkg->len - sizeof(svr_proto_t), fdsess);
}

