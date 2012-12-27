/*
 * =====================================================================================
 *
 *       Filename:  global_data.hpp
 *
 *    Description: 用于保存全局变量，其它目录文件下不得有其他的全局变量  
 *
 *        Version:  1.0
 *        Created:  01/19/11 20:17:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef GLOBAL_DATA_HPP
#define GLOBAL_DATA_HPP

#include "cli_proto.hpp"
#include "ambassador.hpp"
#include "battle.hpp"
#include "fwd_decl.hpp"
#include "home.hpp"
#include "item.hpp"
#include "trade.hpp"
#include "task.hpp"
#include "summon_monster.hpp"
#include "numen.hpp"
#include "store_sales.hpp"
#include "player.hpp"
#include "restriction.hpp"
#include "coupons.hpp"
#include "skill.hpp"
#include "stage_config.hpp"
#include "active_config.hpp"
#include "apothecary.hpp"
//#include "achievement.hpp"
#include "lua_engine.hpp"
#include "title_system.hpp"
#include "ap_toplist.hpp"
#include "ring_task.hpp"
#include "check_shop.hpp"
#include "home_impl.hpp"
#include "home.hpp"
#include "limit.hpp"
//#include "fight_team.hpp"
#include "random_event.hpp"
#include "contest.hpp"
#include "god_guard.hpp"
#include <kf/pet_attr.hpp>
#include <kf/title_attr.hpp>
#include <kf/memstream.hpp>
//added bycws 0530
#include <vector>
extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <libtaomee/list.h>
//
//for ambassador.cpp
//
extern ambassador_reward_t g_ambassador_reward[max_ambassador_reward_num + 1];
extern uint32_t	g_ambassador_reward_cnt;
extern uint32_t g_version_number;
extern fanfan_mrg g_fanfan_mrg;

//
//for battle.cpp
//
struct battle_server_t {
	/*! service_name */
	char		svr_name[16];
	/*! server_id */
	int16_t		id;
};

/**
  * @brief data type for handlers that handle protocol packages from battle server
  */
typedef int (*btlsvr_hdlr_t)(player_t* p, btl_proto_t* pkg);

/**
  * @brief data type for handles that handle protocol packages from battle server
  */
struct btlsvr_handle_t {
	btlsvr_handle_t(btlsvr_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
		{ hdlr = h; len = l; cmp_method = cmp; }

	btlsvr_hdlr_t		hdlr;
	uint32_t			len;
	uint8_t				cmp_method;
};

/*! save handles for handling protocol packages from battle server */
extern btlsvr_handle_t btl_handles[10000];
/*! for packing protocol data and send to battle server */
extern uint8_t	btlpkgbuf[btlproto_max_len];
/*! hold ip address and port of each battle servers */
extern battle_server_t	battle_svrs[max_battle_svr_num];
/*! hold number of battle servers */
extern int		battle_svr_cnt;
/*! hold fds to battle servers */
extern int		battle_fds[max_battle_svr_num];
/*! hold stage info which limit for entering */
extern stage_activity_limit_t stage_activety_limit[1000];
/*! hold specialstages*/
extern std::map<uint32_t, specialstages_t*> special_stages_map_;

/*! Footprinting pvp battle */
extern pvp_pos footprint[MAX_ARENA_NUM]; 
//---------------------------------------------------------------
//

//extern timer_struct_t* trigger_timer;
//
//for battle_switch.cpp
//
//

extern int battle_switch_fd;
extern int trade_switch_fd;
//
//for cache_switch.cpp
//
extern int cache_switch_fd;
/*! for packing protocol data and send to battle switch. 2M */  
extern uint8_t cachepkgbuf[1 << 21];

//
//for cachesvr.cpp
//
extern int cachesvr_fd;

//
//for cli_proto.cpp
/**
  * @brief data type for handlers that handle protocol packages from client
  */
typedef int (*cli_proto_hdlr_t)(player_t* p, uint8_t* body, uint32_t bodylen);

typedef bool (*check_time_limit_func)(player_t* p);


/**
  * @brief data type for handles that handle protocol packages from client
  */
struct cli_proto_handle_t {
	cli_proto_handle_t(cli_proto_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0, uint8_t noguest = 0, uint8_t timelimited = 0)
	{ 
		hdlr = h; 
		len = l; 
		cmp_method = cmp; 
		no_guest = noguest; 
		time_limited = timelimited;
	}

	cli_proto_hdlr_t	hdlr;
	uint32_t			len;
	uint8_t				cmp_method;
	uint8_t				no_guest;
	uint8_t				time_limited;

	bool is_time_limit(player_t *p)
	{
		if (time_limited == 1 || time_limited == 3) {
			if (is_time_limit_1(p)) {
				return true;
			}
		}

		if (time_limited == 2 || time_limited == 3) {
			if (is_time_limit_2(p)) {
				return true;
			}
		}
		if (time_limited == 4) {
			if (is_time_limit_3(p)) {
				return true;
			}
		}
		return false;
	}

};


/*! save handles for handling protocol packages from client */
extern  cli_proto_handle_t cli_handles[cli_proto_cmd_max];
/*! head node to link all players whose waitcmd is not 0 together */
extern  list_head_t awaiting_playerlist;

/*! for packing protocol data and send to client. 2M */  
extern uint8_t pkgbuf[1 << 21];

//
//for dbproxy.cpp
//
/**
  * @brief data type for handlers that handle protocol packages from dbproxy
  */
typedef int (*dbproto_hdlr_t)(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief data type for handles that handle protocol packages from dbproxy
  */
struct dbproto_handle_t {
	dbproto_handle_t(dbproto_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
		{ hdlr = h; len = l; cmp_method = cmp; }

	dbproto_hdlr_t		hdlr;
	uint32_t			len;
	uint8_t				cmp_method;
};

extern dbproto_handle_t db_handles[65536];

/*! udp socket address for db server */
extern struct sockaddr_in dbsvr_udp_sa[3];

extern struct sockaddr_in dbsvr_onoff_udp_sa;


/*! dbproxy socket fd */
extern int proxysvr_fd;

/*! seer dbproxy socket fd */ 
extern int seer_fd[2];
extern const char* seer_ip[2];
extern const char* seer_port[2];

/*! dbsvr udp socket fd */
extern int dbsvr_udp_fd[2];

/*! dbsvr udp socket fd */
extern int dbsvr_onoff_udp_fd;


/*! for packing protocol data and send to dbproxy */
extern uint8_t dbpkgbuf[dbproto_max_len];
//-----------------------------------------------------------------

//
//for fwd_decl.cpp
//
/* hold common timers */
extern timer_head_t g_events;
/* pathname where to store the statistic log info */
extern char* statistic_logfile;
/*! maximum battle time in a day */
extern uint32_t battle_time_limit;

//
//for home.cpp
//

/**
  * @brief struct to hold ip and port of a home server
  */
struct home_server_t {
	/*! service_name */
	char		svr_name[16];
	/*! server_id */
	int16_t		id;
};

/**
  * @brief data type for handlers that handle protocol packages from home server
  */
typedef int (*homesvr_hdlr_t)(player_t* p, home_proto_t* pkg);

/**
  * @brief data type for handles that handle protocol packages from home server
  */
struct homesvr_handle_t {
	homesvr_handle_t(homesvr_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
		{ hdlr = h; len = l; cmp_method = cmp; }

	homesvr_hdlr_t		hdlr;
	uint32_t			len;
	uint8_t				cmp_method;
};

/*! save handles for handling protocol packages from home server */
extern homesvr_handle_t home_handles[10000];
/*! for packing protocol data and send to home server */
extern uint8_t		homepkgbuf[homeproto_max_len];
/*! hold ip address and port of each home servers */
extern home_server_t	home_svrs[max_home_svr_num];
/*! hold number of home servers */
extern int		home_svr_cnt;
/*! hold fds to home servers */
extern int		home_fds[max_home_svr_num];
//

//
//for item.cpp
//
/*! the pointer to ItemManager for handling all items*/
extern GfItemManager* items;
/*! for handling all compose product*/
extern std::map<uint32_t, method_t*> methods_map_;
/*! for unique get item*/
extern std::map<uint32_t, unique_item_pos_t> unique_item_map;
/*! for save all strengthen material*/
extern std::map<uint32_t, strengthen_t> material_map;
/* rate of level 0 when strengthen attire*/
extern uint32_t reset_level_rate[12];

/*! strengthen fail, rate of level cut 1*/
extern uint32_t scut_level_rate[12];

/*! for strengthen fail */
extern uint32_t strengthen_fail_rule [max_attire_level][2]; 
extern uint32_t strengthen_consume_coins[max_attire_level]; 

extern suit_t suit_arr[max_suit_id + 1];

//
//for magic_number.cpp
//
extern int magic_number_svr_fd;

//
//for map.cpp
//
extern list_head_t all_maps[c_bucket_size];

//
//for player.cpp
//

/*! hashes all players within this server by fd */
extern GHashTable*	all_players;

/*! hashes all players within this server by user id */
extern list_head_t	uid_buckets[c_bucket_size];
/*! number of players within this server */
extern uint32_t			player_num;

/*! maximum battle time of one day for one week */
extern uint32_t time_limit_arr[7];
/*! maximum double experience time of on day */
extern uint32_t time_double_arr[7];

/*!  */
extern uint32_t income_per_arr[7][24];
//
//for restriction.cpp
//
extern restriction_t g_all_restr[max_restr_count];
//
// for secondary_profession.cpp
//
extern base_exp2level_calculator   s_second_pro_calculator[10];
//
//for skill.cpp
//

/*! the all skills*/
extern std::map<uint32_t, skills_t*> skills_map_;
/*! the all buff skills*/
extern std::map<uint32_t, buff_skill_t> buff_skill_map_;
//
//for stage_config.cpp
//
extern stage_data_ptr * data_array;
//
//for store_sales.cpp
//
/*! for packing protocol data and send to mbsvr */
extern uint8_t vippkgbuf[vipproto_max_len];

extern std::map<uint32_t, store_item_t> store_item_map_;
extern std::map<uint32_t, store_feedback_t> store_feedback_map_;
extern std::map<uint32_t, coupon_product_item_t> product_item_map_;

/*! vipproxy socket fd */
extern int storesvr_fd;

//
//for vip_impl.cpp
//
/*! for packing protocol data and send to mbsvr */
extern uint8_t vippkgbuf_2[vipproto_max_len];

/*! vipproxy socket fd */
extern int vipsvr_fd;


//
//for summon_monster.cpp
//
extern std::map<uint32_t, summon_info_t> summons_map_;

extern std::map<uint32_t, summon_skill_scroll_t> sum_scroll_map_;

//----- for numen.cpp
extern std::map<uint32_t, numen_info_t> numens_map_;
extern std::map<uint32_t, call_numen_odds_t> call_numen_rule_map_;

//
//for switch.cpp
//
extern int switch_fd;

//
//for task.cpp
//

extern task_t  all_tasks[max_task_id];
extern task_t  all_bonus_tasks[max_bonus_task_num];

extern uint32_t  max_nor_daily_task_times;
extern daily_task_t  all_daily_tasks[player_max_level + 1];

extern std::map<uint32_t, task_t> daily_task_map;

//for chat.cpp chat to knowall
struct QA{
	char  questions[1024];
	char answers[1024];
};
extern std::vector<QA> quesAns;
//
//for trade.cpp
//
//
/**
  * @brief struct to hold ip and port of a trade server
  */
struct trade_server_t {
	/*! service_name */
	char		svr_name[16];
	/*! server_id */
	int16_t		id;
};

extern trade_server_t trade_svrs[max_trade_svr_num];

extern int trade_fds[max_trade_svr_num];

extern uint32_t trade_svr_cnt;


typedef int (*trsvr_hdl_t)(player_t * p, tr_proto_t * pkg);

struct trsvr_handle_t {
	trsvr_handle_t(trsvr_hdl_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
	{
		hdlr = h; len = l; cmp_method = cmp;
	}
	trsvr_hdl_t hdlr;
	uint32_t len;
	uint8_t cmp_method;
};

extern trsvr_handle_t trsvr_handles[10000];

extern uint8_t trpkgbuf[8192];

struct trade_list_t {
	list_head_t player_list;
};
// a list link all the player in trade market 
extern trade_list_t trade_players;

//for gf_cryptogram.cpp

extern int gf_cryptogram_svr_fd;

vip_config_data_mgr* get_vip_config_data_mgr();

stage_data_mgr*   get_stage_data_mgr();

buy_item_limit_mgr* get_buy_item_limit_mgr();

formulation_data_mgr* get_formulation_data_mgr();

class achievement_data_mgr; 
achievement_data_mgr* get_achievement_data_mgr();

ap_toplist*           get_ap_toplist();

ring_task_mgr*        get_ring_task_mgr();

shop_mgr*             get_shop_mgr();

//pet_attr_data_mgr* get_pet_attr_data_mgr();

title_attr_data_mgr* get_title_attr_data_mgr();

bool is_need_clear_task_id(uint32_t task_id);

bool is_outmoded_ring_task(uint32_t taskid);

extern lua_State* s_lua;

//extern achievement_callback_handle handles[ MAX_ACHIEVEMENT_COUNT+ 1];


extern title_condition_t g_title_condition;

//extern home_exp2level    g_home_exp2level[MAX_HOME_LEVEL];


extern items_upgrade_mrg g_items_upgrade_mrg;

extern swap_action_mrg	g_swap_action_mrg;

struct char_content_t {
	uint32_t id;
	char     content[512];
	char     name[512];
};
extern char_content_t char_content[];

struct server_config_t {
	uint32_t id;
	uint32_t value;
};
extern server_config_t g_server_config[max_server_config_id];

extern limit_data_mrg g_limit_data_mrg;
extern Summon_dragon_list_mrg g_summon_dragon_list;

extern mem_stream net_stream;

struct team_top10_chache_t;
struct team_active_top100_cache_t;

//fight_team
extern team_top10_chache_t g_team_top10_chache;
extern team_active_top100_cache_t g_team_active_chache;

struct pvp_game_rank_cache_t;

//contest
extern pvp_game_rank_cache_t g_pvp_game_cache;

//active_config
active_data_mgr*  get_active_data_mgr();

event_trigger_mgr*  get_event_trigger_mgr();

//god_guard
god_guard_mgr*  get_god_guard_mgr();

//random_event
sum_event_mgr * get_sum_event_mgr();

extern reward_player_cache_t g_reward_player_cache;

extern char branch_sys_ip[128];
extern uint16_t branch_sys_port;
#endif


