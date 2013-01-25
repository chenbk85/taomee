extern "C" {
#include <stdint.h>

#include <libtaomee/log.h>

#include <async_serv/async_serv.h>
}

#include <libtaomee++/proto/proto_util.h>
#include "proto.hpp"

#include "dispatcher.h"
#include "dirty_word.h"
#include "global.h"
#include "timer_func.h"



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

        char outbuf[13000];
        bin2hex(outbuf,(char*)buff, sendpkg->len, 8192);
        DEBUG_LOG("WORD CO[%u][%u][%s]", sendpkg->len, fstream->sendlen, outbuf  );
	}
	return 0;
}

typedef int (*func_t)(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

func_t funcs[50000];

void init_funcs()
{
	funcs[adm_get_dirty_words - 60001] = get_dirty_words;
	funcs[adm_get_dirty_md5 - 60001] = get_dirty_md5;
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

	DEBUG_LOG("userid:%u,cmdid:%u len:%u",pkg->id, pkg->cmd, pkg->len);
	int cmd = pkg->cmd - 60001;
	if ((cmd < 1) || (cmd > 50000) || (funcs[cmd] == 0)) {
		ERROR_LOG("invalid cmd=%u from fd=%d", pkg->cmd, fdsess ? fdsess->fd : 0);
		return -1;
	}

	return funcs[cmd](pkg, pkg->len - sizeof(svr_proto_t), fdsess);
}

