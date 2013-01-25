#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <libtaomee/log.h>
#include <async_serv/dll.h>
#ifdef __cplusplus
}
#endif

#include "bt_online.h"
#include "proto.h"

#include "bt_dispatcher.h"
#include "global.h"

void init_funcs()
{
	// For Online Server
	funcs[battle_info_msg - 61001]       = battle_info_op;
}

int dispatch(void* data, fdsession_t* fdsess)
{
	batrserv_proto_t* chead = (batrserv_proto_t*)data;
	uint16_t cmd = chead->cmd - 61001;
  
	if ((cmd < 1) || (cmd > 5500) || (funcs[cmd] == 0)) {
		ERROR_LOG("invalid cmd=%u from fd=%d", chead->cmd, fdsess->fd);
		return -1;
	}
	
	if (chead->online_id <= 0 || chead->online_id > MAX_ONLINE_NUM){
		ERROR_RETURN(("bad online id\t[%u]", chead->online_id), -1);
	}
	
	online[chead->online_id] = fdsess;
	int ret=funcs[cmd](chead, chead->len, fdsess);
	return ret;
}

