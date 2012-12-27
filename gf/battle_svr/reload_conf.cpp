#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}

#include "skill.hpp"
#include "monster.hpp"
#include "item.hpp"
#include "reload_conf.hpp"
#include "stage.hpp"


inline int reload_items()
{
	if (items) {
		try {
			items->reload_items();
		} catch (...) {
			INFO_LOG("reload items catched exception");
			return -1;
		}
		INFO_LOG("reload items succ");
		return 0;
	} 
	
	INFO_LOG("reload items never, items == NULL");
	return -1;
}

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
	case reload_skills_cmd:
		//if (g_skills) {
		//	g_skills->reload_skills();
		//}
		break;
	case reload_monst_cmd:
		//reload_monsters();
		break;
	case reload_stages_cmd:
		break;
	case reload_quality_cmd:
		reload_quality();
		break;
	case reload_items_cmd:
		reload_items();
		break;
	default:
		ERROR_LOG("unsurported cmd %u", cmd);
		break;
	}
}


