extern "C" {
#include <stdint.h>

#include <libtaomee/log.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
}

#include <libtaomee++/proto/proto_util.h>
#include "proto.hpp"

#include "dispatcher.h"
#include "dirty_word.h"
#include "global.h"
#include "timer_func.h"


int segment(char* buffer, int length)
{
	Segmenter* seg = g_seg_mgr.getSegmenter();

    seg->setBuffer((u1*)buffer, length);
    u2 len = 0, symlen = 0;
    u2 kwlen = 0, kwsymlen = 0;
    //check 1st token.
    char txtHead[3] = {239,187,191};
    char* tok = (char*)seg->peekToken(len, symlen);
    seg->popToken(len);
    if(seg->isSentenceEnd()){
        do {
            char* kwtok = (char*)seg->peekToken(kwlen , kwsymlen,1);
            if(kwsymlen)
                printf("[kw]%*.*s/x ",kwsymlen,kwsymlen,kwtok);
        }while(kwsymlen);
    }

    if(len == 3 && memcmp(tok,txtHead,sizeof(char)*3) == 0){
    }else{
        printf("%*.*s/x ",symlen,symlen,tok);
    }	

    while(1){
        len = 0;
        char* tok = (char*)seg->peekToken(len,symlen);
        if(!tok || !*tok || !len)
            break;
        seg->popToken(len);
        if(seg->isSentenceEnd()){
            do {
                char* kwtok = (char*)seg->peekToken(kwlen , kwsymlen,1);
                if(kwsymlen)
                    printf("[kw]%*.*s/x ",kwsymlen,kwsymlen,kwtok);
            }while(kwsymlen);
        }

        if(*tok == '\r')
            continue;
        if(*tok == '\n'){
            printf("\n");
            continue;
        }


        printf("%*.*s/x ",symlen,symlen,tok);
        {
            const char* thesaurus_ptr = seg->thesaurus(tok, symlen);
            while(thesaurus_ptr && *thesaurus_ptr) {
                len = strlen(thesaurus_ptr);
                printf("%*.*s/s ",len,len,thesaurus_ptr);
                thesaurus_ptr += len + 1; //move next
            }
        }
    }
	return 0;
}

void try_free_msg_timely()
{
}

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
		KDEBUG_LOG(pmsg->userid, "has forbidden in 5 min");
		return 0;
	}

	bool check_ret = pcheck->check_msg(pmsg->userid, pmsg->timestamp, pmsg->msg, pmsg->msglen);

	if (!check_ret) {
		pcheck->chat_forbid.send_pkg_forbid_user(pmsg->userid);
		uint32_t addtime = pcheck->chat_forbid.add_forbid_user(pmsg->userid);
		if (addtime) {
			KDEBUG_LOG(pmsg->userid, "AAAAAAA\t[%u]", addtime);
			ADD_ONLINE_TIMER(&g_events, n_chat_forbid_pop, pcheck, addtime);
		}
	}

	char buff[4096];
	int len = pmsg->msglen >= 4096 ? 4095 : pmsg->msglen;
	memcpy(buff, pmsg->msg, len);
	buff[len] = '\0';

	KINFO_LOG(pmsg->userid, " %u %s %d", pmsg->timestamp, buff, (int)(!check_ret));

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

	//pmsg->msg[pmsg->msglen] = 0;
	//segment(pmsg->msg, pmsg->msglen);

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
		//pcheck->chat_forbid.send_pkg_forbid_user(pmsg->userid);
		uint32_t addtime = pcheck->chat_forbid.add_forbid_user(pkg->id);
		KDEBUG_LOG(pkg->id, "AAAAAAA\t[%u]", addtime);
		if (addtime) {
			ADD_ONLINE_TIMER(&g_events, n_chat_forbid_pop, pcheck, addtime);
		}
	}

	return 0;
}

int get_dirty_md5(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	char buff[64];
	svr_proto_t* sendpkg = (svr_proto_t*)buff;
	sendpkg->cmd = pkg->cmd;
	sendpkg->len = sizeof(svr_proto_t) + Cdirty_word::dirty_file_md5_len;
	memcpy(sendpkg->body, g_dirty_word->file_md5, Cdirty_word::dirty_file_md5_len);
	send_pkg_to_client(fdsess, buff, sendpkg->len);
	return 0;
}

struct file_stream {
	char		md5[Cdirty_word::dirty_file_md5_len];
	uint32_t	totallen;
	uint32_t	endable;
	char		reserve[32];
	uint32_t	sendlen;
	char		file_bit_stream[];
}__attribute__((packed));

int get_dirty_words(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	char buff[8192] = {};
	svr_proto_t* sendpkg = (svr_proto_t*)buff;
	sendpkg->cmd = pkg->cmd;
	file_stream* fstream = (file_stream *)sendpkg->body;
	memcpy(fstream->md5, g_dirty_word->file_md5, Cdirty_word::dirty_file_md5_len);
	fstream->totallen = g_dirty_word->total_len;
	for (uint32_t len = 0; len < fstream->totallen; len += 4096) {
		fstream->sendlen = 4096;
		if (len + 4096 >= fstream->totallen) {
			fstream->sendlen = fstream->totallen - len;
			fstream->endable = 1;
		}
		sendpkg->len = sizeof(svr_proto_t) + sizeof(file_stream) + fstream->sendlen;
		memcpy(fstream->file_bit_stream, g_dirty_word->file_cache + len, fstream->sendlen);	
		send_pkg_to_client(fdsess, buff, sendpkg->len);
		DEBUG_LOG("SEND PKG\t[%u]", sendpkg->len);
	}
	return 0;
}

typedef int (*func_t)(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

func_t funcs[50000];

void init_funcs()
{
	funcs[adm_check_msg - 60001]       = check_msg;
	funcs[adm_get_dirty_words - 60001] = get_dirty_words;
	funcs[adm_get_dirty_md5 - 60001] = get_dirty_md5;
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

	TRACE_LOG("userid:%u,cmdid:%u len:%u",pkg->id, pkg->cmd, pkg->len);
	int cmd = pkg->cmd - 60001;
	if ((cmd < 1) || (cmd > 50000) || (funcs[cmd] == 0)) {
		ERROR_LOG("invalid cmd=%u from fd=%d", pkg->cmd, fdsess ? fdsess->fd : 0);
		return -1;
	}

	return funcs[cmd](pkg, pkg->len - sizeof(svr_proto_t), fdsess);
}

