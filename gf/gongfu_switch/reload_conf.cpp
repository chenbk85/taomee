#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <libtaomee/conf_parser/config.h>
#include <async_serv/dll.h>
#include <libtaomee/log.h>
}

#include "reload_conf.hpp"
#include "online.hpp"

/**
  * @brief reload configs
  */
void reload_conf_op(uint32_t cmd, const uint8_t* data, uint32_t datalen)
{
	DEBUG_LOG("here2");
	if (datalen != 4) {
		return;
	}

	uint32_t target_id = 0;
	int idx = 0;
	taomee::unpack_h(data, target_id, idx);
	if (target_id && target_id != get_server_id()) {
		return;
	}

	switch (cmd) {
	case reload_bench_config_cmd:
		DEBUG_LOG("here");
		reload_bench_conf();
		break;
	default:
		ERROR_LOG("unsurported cmd %u", cmd);
		break;
	}
}

/**
  * @brief reload bench.conf configs
  */
void reload_bench_conf(void)
{
	DEBUG_LOG("here3");
	int err = config_init("./bench.conf");
	Online::rewrite_olid_file();
	INFO_LOG("Reload File '%s' %s", "./bench.conf", (err ? "Failed" : "Succeeded"));
}
