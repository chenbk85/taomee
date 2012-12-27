#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/inet/byteswap.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>

extern "C" {
#include <libtaomee/conf_parser/config.h>
}

#include "item.hpp"
#include "battle.hpp"
#include "reload_conf.hpp"
#include "restriction.hpp"
#include "task.hpp"
#include "global_data.hpp"

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
	TRACE_LOG("target_id %u", target_id);
	if (target_id && target_id != get_server_id()) {
		return;
	}

	switch (cmd) {
	case reload_item_config_cmd:
		reload_items();
		break;
	case reload_battle_config_cmd:
		reload_battle_svr_config();
		break;
	case reload_daily_activity_cmd:
		reload_daily_activity();
		break;
	case reload_task_cmd:
		reload_tasks();
		break;
	case reload_active_stage_cmd:
		get_stage_data_mgr()->init("./conf/stage.xml");
		break;
	case reload_swap_action_cmd:
		g_swap_action_mrg.init("./conf/swapAction.xml");
		break;
	case reload_active_data_cmd:
		get_active_data_mgr()->init("./conf/active_time_limit.xml");
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

	int err = config_init("./bench.conf");

	if (err == -1) {
		goto out;
	}
#if 0 /* JUST FOR TRACE */ 
#define LOG_STR(item) do { INFO_LOG(item":%s", config_get_strval(item)); } while (0)
#define LOG_INT(item) do { INFO_LOG(item":%d", config_get_intval(item, -1)); } while (0)
	LOG_STR("mcast_ip");
	LOG_INT("mcast_port");
	LOG_STR("dbproxy_ip");
	LOG_INT("dbproxy_port");
	LOG_STR("switch_ip");
	LOG_INT("switch_port");
	LOG_STR("cachesvr_ip");
	LOG_INT("cachesvr_port");
#endif
	/*if (init_time_limit_day(config_get_strval("time_limit"))) {
		ERROR_LOG("init time limit day error");	
		goto out;
	} else {
		struct tm tm_tmp = *get_now_tm();
		tm_tmp.tm_hour = 23;
		tm_tmp.tm_min  = 59;
		tm_tmp.tm_sec  = 0;
		int32_t exptm   = mktime(&tm_tmp);
	
		if (get_now_tv()->tv_sec > exptm) {
			set_battle_time_limit(1);
		} else {
			set_battle_time_limit(0);
		}
	
		statistic_logfile = config_get_strval("statistic_log");
	}*/

out:
	INFO_LOG("Reload File '%s' %s", "./bench.conf", (err ? "Failed" : "Succeeded"));
}

/**
  * @brief reload items configs
  */
inline void reload_items()
{
	try {
		items->reload_items();
	}  catch (const std::exception& e) {
		INFO_LOG("Fail to Reload items. Err: %s", e.what());
	}
}

/**
 * @brief reload dailyactivity.xml
 */
inline void reload_daily_activity()
{
 	taomee::load_xmlconf("./conf/dailyActivity.xml", load_restrictions);
}

/**
 * @brief reload tasks.xml
 */
inline void reload_tasks()
{
 	taomee::load_xmlconf("./conf/tasks.xml", load_tasks_config);
}


