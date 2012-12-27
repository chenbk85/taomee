#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/log.h>
}

#include "reload_conf.hpp"

/**
  * @brief reload configs
  */
void reload_conf_op(uint32_t cmd, const uint8_t* data, uint32_t datalen)
{
	TRACE_LOG("datalen %u", datalen);
	if (datalen != 4) {
		return;
	}

	uint32_t target_id = 0;
	int idx = 0;
	taomee::unpack_h(data, target_id, idx);
	TRACE_LOG("reload_conf_op target_id %u", target_id);
	if (target_id && target_id != get_server_id()) {
		return;
	}

	switch (cmd) {
	default:
		ERROR_LOG("unsurported cmd %u", cmd);
		break;
	}
}


