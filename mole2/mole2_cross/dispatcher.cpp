extern "C" {
#include <stdint.h>
#include <arpa/inet.h>

#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
}
#include "utils.hpp"
#include "proto.hpp"
#include "dispatcher.hpp"
int dispatch(IJobDispatcher* dispatcher, void* data, fdsession_t* fdsess)
{
	svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(data);

	if (pkg->cmd != proto_cross_data && pkg->cmd != proto_test_alive){
		ERROR_LOG("recv trash data from fd=%d", fdsess->fd);
		return -1;
	}
	
    if(pkg->cmd == proto_test_alive) {
        static uint8_t alive_buffer[1024];
        int idx = sizeof(svr_proto_t);
	    init_proto_head(alive_buffer, pkg->id, idx, pkg->seq, pkg->cmd, 0);
	    return send_pkg_to_client(fdsess, alive_buffer, idx);
    }

	mole2cross_proto_t* mole_cross = NULL;
	CONVERT_PTR_TO_PTR(data, mole_cross);
	
	if ((mole_cross->opid <= CROSS_MIN_PROT_COMMID) || (mole_cross->opid >= CROSS_MAX_PROT_COMMID)) {
		ERROR_LOG("invalid opid=%u from fd=%d", mole_cross->opid, fdsess->fd);
		return -1;
	}


	// dispatch protocol event.
	return dispatcher->Dispatch(mole_cross->opid, data, fdsess);
}

