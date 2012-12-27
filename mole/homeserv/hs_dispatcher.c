
#include <stdint.h>
#include <libtaomee/log.h>
#include <async_serv/dll.h>

#include "hs_online.h"
#include "proto.h"

#include "hs_dispatcher.h"

typedef int (*func_t)(void* pkg, int bodylen, fdsession_t* fdsess);

func_t funcs[5500];

void init_funcs()
{
	// For Online Server
	funcs[MCAST_home_msg - 61001]       = sprite_info_chg_op;
}

int dispatch(void* data, fdsession_t* fdsess)
{
	svr_proto_t* pkg = data;
	uint16_t cmd = pkg->cmd - 61001;
    if(pkg->cmd == proto_test_alive) {
        static uint8_t alive_buffer[1024];
        int idx = sizeof(svr_proto_t);
	    init_proto_head(alive_buffer, pkg->id, idx, pkg->seq, pkg->cmd, 0);
	    return send_pkg_to_client(fdsess, alive_buffer, idx);

    }
	if ((cmd < 1) || (cmd > 5500) || (funcs[cmd] == 0)) {
		ERROR_LOG("invalid cmd=%u from fd=%d", pkg->cmd, fdsess->fd);
		return -1;
	}
	
	communicator_header_t* chead = data;
	
	DEBUG_LOG("DISPATCH\t[%u %u %lu %u]", chead->cmd, chead->online_id, chead->mapid, chead->opid);
	if (chead->online_id > MAX_ONLINE_NUM)
		ERROR_RETURN(("bad online id"), 0);
	online[chead->online_id] = fdsess;
	DEBUG_LOG("ONLINE FDSESS\t[%u %u]", chead->online_id, fdsess->fd);

	return funcs[cmd](chead, chead->len, fdsess);
}

