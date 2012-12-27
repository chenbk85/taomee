/*
 * =====================================================================================
 *
 *       Filename:  global_data.cpp
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  01/19/11 20:17:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include "global_data.hpp"
#include "fight_team.hpp"

//version number 0 for default version; 1 for kaixin version
uint32_t g_version_number;

const char* channel_string_npc_shop = "npc_shop"; 								//商店买卖
const char* channel_string_storage_shop = "storage_shop";            //道具商城买卖
const char* channel_string_trade = "trade";                            //交易获得
const char* channel_string_task = "task";                           //任务
const char* channel_string_active = "active";                         //各种活动兑换获得
const char* channel_string_pick = "pick";                            //战斗中减去
const char* channel_string_box = "box";                             //宝箱获得
const char* channel_string_use = "use";                              //使用掉
const char* channel_string_discard = "discard";                        //用户自己丢弃销毁
const char* channel_string_other = "other";                            //其它
const char* channel_string_sec_pro = "sec_pro";
const char* channel_string_magic_num = "magic_num";
const char* channel_string_home = "home";
const char* channel_string_use_tongbao_item = "use_tb_item";
const char* channel_string_decompose = "decompose";
const char* channel_string_compose = "compose";
const char* channel_string_upgrade_item = "upgrade_item";
const char* channel_string_mail	= "mail";
const char* channel_string_strengthen = "strengthen";


//dailyaction static
fanfan_mrg g_fanfan_mrg; 


//
//for ambassador.cpp
//
ambassador_reward_t g_ambassador_reward[max_ambassador_reward_num + 1];
uint32_t			g_ambassador_reward_cnt;

//
//for battle.cpp
//
btlsvr_handle_t btl_handles[10000];
/*! for packing protocol data and send to battle server */
uint8_t	btlpkgbuf[btlproto_max_len];
/*! hold ip address and port of each battle servers */
battle_server_t	battle_svrs[max_battle_svr_num];
/*! hold number of battle servers */
int	battle_svr_cnt;
/*! hold fds to battle servers */
int battle_fds[max_battle_svr_num] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	                                           -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
/*! hold stage info which limit for entering */
stage_activity_limit_t stage_activety_limit[1000];
/*! hold specialstages*/
std::map<uint32_t, specialstages_t*> special_stages_map_;

/*! Footprinting pvp battle */
pvp_pos footprint[MAX_ARENA_NUM] = {{0},{0},{0}};
//---------------------------------------------------------------

//timer_struct_t* trigger_timer = NULL;
//
//for battle_switch.cpp
//
int battle_switch_fd = -1;
int trade_switch_fd = -1;
//
//for cache_switch.cpp
//

int cache_switch_fd;
/*! for packing protocol data and send to battle switch. 2M */  
uint8_t cachepkgbuf[1 << 21];

//
// for cachesvr.cpp
//

int cachesvr_fd = -1;

//
//for cli_proto.cpp
//

/*!  handles for handling protocol packages from client */
cli_proto_handle_t cli_handles[cli_proto_cmd_max];
/*! head node to link all players whose waitcmd is not 0 together */
list_head_t awaiting_playerlist;

/*! for packing protocol data and send to client. 2M */  
uint8_t pkgbuf[1 << 21];

//
//for dbproxy.cpp
//
/*! save handles for handling protocol packages from dbproxy */
dbproto_handle_t db_handles[65536];

/*! udp socket address for db server */
struct sockaddr_in dbsvr_udp_sa[3];

struct sockaddr_in dbsvr_onoff_udp_sa;


/*! dbproxy socket fd */
int proxysvr_fd = -1;

/*! seer dbproxy socket fd */
int seer_fd[2] = {-1, -1};
const char* seer_ip[2] = {"seer_dx_ip", "seer_wt_ip"};
const char* seer_port[2] = {"seer_dx_port", "seer_wt_port"};

/*! dbsvr udp socket fd */
int dbsvr_udp_fd[2] = {-1, -1};

/*! dbsvr udp socket fd */
int dbsvr_onoff_udp_fd = -1;


/*! for packing protocol data and send to dbproxy */
uint8_t dbpkgbuf[dbproto_max_len];


//
//for fwd_decl.cpp
//
/* hold common timers */
timer_head_t g_events;
/* pathname where to store the statistic log info */
char* statistic_logfile;
/*! maximum battle time in a day */
uint32_t battle_time_limit;

//
//for home.cpp
//

/**
  * @brief struct to hold ip and port of a home server
  */
/*! save handles for handling protocol packages from home server */
homesvr_handle_t home_handles[10000];
/*! for packing protocol data and send to home server */
uint8_t			homepkgbuf[homeproto_max_len];
/*! hold ip address and port of each home servers */
home_server_t	home_svrs[max_home_svr_num];
/*! hold number of home servers */
int				home_svr_cnt;
/*! hold fds to home servers */
int				home_fds[max_home_svr_num] = { -1,-1,-1,-1,-1, -1,-1,-1,-1,-1 };

//
//for item.cpp
//

/*! the pointer to ItemManager for handling all items*/
GfItemManager g_items("./conf/items_nor.xml", "./conf/items_attire.xml", "./conf/items_shop.xml");
GfItemManager* items = &g_items;

/*! for handling all compose product*/
std::map<uint32_t, method_t*> methods_map_;
/*! for unique get item*/
std::map<uint32_t, unique_item_pos_t> unique_item_map;
/*! for save all strengthen material*/
std::map<uint32_t, strengthen_t> material_map;
/* rate of level 0 when strengthen attire*/
uint32_t reset_level_rate[12];
/*! strengthen fail, rate of level cut 1*/
uint32_t scut_level_rate[12];
/*! for strengthen fail */
/*
uint32_t strengthen_fail_rule [max_attire_level][2] = {
    {0, 100}, {0, 100}, {8, 92}, {8, 92}, {65, 35}, {92, 8}, {92, 8},
    {92, 8}, {99, 1}, {99, 1}, {99, 1}, {99, 1}};
    */
uint32_t strengthen_consume_coins[max_attire_level] = {
    3000, 3000, 3000, 5000, 5000, 6000, 6000, 7000, 8000, 8000, 8000, 8000};

suit_t suit_arr[max_suit_id + 1];


//
//for magic_number.cpp
//

int magic_number_svr_fd = -1;

//
//for map.cpp
//
list_head_t all_maps[c_bucket_size];

//
//for player.cpp
//

/*! hashes all players within this server by fd */
GHashTable*	all_players;

GHashTable* in_trade_players;

/*! hashes all players within this server by user id */
list_head_t	uid_buckets[c_bucket_size];
/*! number of players within this server */
uint32_t	player_num;

/*! maximum battle time of one day for one week */
uint32_t time_limit_arr[7];
/*! maximum double experience time of on day */
uint32_t time_double_arr[7];
uint32_t income_per_arr[7][24];

//
//for restriction.cpp
//
restriction_t g_all_restr[max_restr_count];
//
//for coupons.cpp
//
exchange_t g_all_coupons[max_coupons_exchange];

std::map<uint32_t, store_item_t> store_item_map_;
std::map<uint32_t, store_feedback_t> store_feedback_map_;
std::map<uint32_t, coupon_product_item_t> product_item_map_;

//
// for secondary_profession.cpp
//
base_exp2level_calculator   s_second_pro_calculator[10];
//
//for skill.cpp
//

/*! the all skills*/
std::map<uint32_t, skills_t*> skills_map_;
/*! the all buff skills*/
std::map<uint32_t, buff_skill_t> buff_skill_map_;

//std::map<uint32_t role_skill_t> role_skill_map_[max_role_type];
//
//for stage_config.cpp
//
stage_data_ptr* data_array = NULL;
//
//for store_sales.cpp
//
/*! for packing protocol data and send to mbsvr */
uint8_t vippkgbuf[vipproto_max_len];

/*! vipproxy socket fd */
int storesvr_fd = -1;
//
//for vip_impl.cpp
//
/*! for packing protocol data and send to mbsvr */
uint8_t vippkgbuf_2[vipproto_max_len];

/*! vipproxy socket fd */
int vipsvr_fd = -1;


//
//for summon_monster.cpp
//
std::map<uint32_t, summon_info_t> summons_map_;

std::map<uint32_t, summon_skill_scroll_t> sum_scroll_map_;

//----- for numen.cpp
std::map<uint32_t, numen_info_t> numens_map_;
std::map<uint32_t, call_numen_odds_t> call_numen_rule_map_;

//
//for switch.cpp
//
int switch_fd = -1;

//
//for task.cpp
//

task_t  all_tasks[max_task_id];
task_t  all_bonus_tasks[max_bonus_task_num];

uint32_t  max_nor_daily_task_times;
daily_task_t  all_daily_tasks[player_max_level + 1];

std::map<uint32_t, task_t> daily_task_map;
//
//for trade.cpp
//

trade_server_t trade_svrs[max_trade_svr_num];

int trade_fds[max_trade_svr_num];

uint32_t trade_svr_cnt;

trsvr_handle_t trsvr_handles[10000];

uint8_t trpkgbuf[8192];

//for chat.cpp chat to knowall
std::vector<QA> quesAns;

//new add for trade mcast
trade_list_t trade_players;

//for gf_cryptogram.cpp
int gf_cryptogram_svr_fd = -1;

static vip_config_data_mgr s_vip_config_data_mgr;
vip_config_data_mgr* get_vip_config_data_mgr()
{
	return &s_vip_config_data_mgr;
}

static stage_data_mgr   s_stage_data_mgr;
stage_data_mgr*   get_stage_data_mgr()
{
	return &s_stage_data_mgr;
}

static buy_item_limit_mgr s_buy_item_limit_mgr;

buy_item_limit_mgr* get_buy_item_limit_mgr()
{
	return &s_buy_item_limit_mgr;
}

static formulation_data_mgr s_formulation_data_mgr;
formulation_data_mgr* get_formulation_data_mgr()
{
	return &s_formulation_data_mgr;
}

static achievement_data_mgr s_achievement_data_mgr;
achievement_data_mgr* get_achievement_data_mgr()
{
	return &s_achievement_data_mgr;
}

static ap_toplist s_toplist;
ap_toplist*           get_ap_toplist()
{
	return &s_toplist;
}

lua_State* s_lua;


static ring_task_mgr s_ring_task_mgr;
ring_task_mgr*        get_ring_task_mgr()
{
	return &s_ring_task_mgr;
}

static shop_mgr     s_shop_mgr;
shop_mgr*   get_shop_mgr()
{
	return & s_shop_mgr;
}

static title_attr_data_mgr s_title_attr_data_mgr;
title_attr_data_mgr* get_title_attr_data_mgr()
{
	return &s_title_attr_data_mgr;
}

//achievement_callback_handle handles[ MAX_ACHIEVEMENT_COUNT+ 1];

//global v
title_condition_t g_title_condition;

//home_exp2level    g_home_exp2level[MAX_HOME_LEVEL];


uint32_t need_clear_task_id_arr[] = 
	{1001, 1002, 1003, 1004, 1006, 1009, 1018, 1157, 1161, 1182, 
	 1098, 1005, 1008, 1049, 1010, 1011, 1231, 1007, 1124, 1125, 
	 1012, 1013, 1099, 1014, 1015, 1019, 1016, 1017, 1020, 1066, 
	 1192, 1096, 1097, 1205, 1242, 1021, 1022, 1023, 1110, 1111, 
	 1024, 1025, 1026, 1027, 1029, 1112, 1113, 1028, 1030, 1031, 
	 1101, 1115, 1151, 1152, 1032, 1033, 1034, 1114, 1116, 1117, 
	 1153, 1154, 1155, 1035, 1102, 1118, 1156, 1256, 1036, 1037, 
	 1038, 1039, 1119, 1040, 1041, 1042, 1104, 1120, 1043, 1044, 
	 1045, 1100, 1121, 1122, 1046, 1047, 1048, 1106, 1123, 1050, 
	 1107, 1162, 1051, 1052, 1053, 1054, 1055, 1056, 1057, 1058, 
	 1105, 1059, 1060, 1061, 1062, 1063, 1064, 1065, 1103, 1067, 
	 1068, 1069, 1070, 1071, 1072, 1073, 1074, 1075, 1076, 1077, 
	 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 
	 1088, 1089, 1090, 1091, 1092, 1093, 1094, 1126, 1127, 1128, 
	 1129, 1130, 1131, 1132, 1133, 1134, 1329, 1325, 1326, 1157,
	 1736, 1753, 1754, 1147, 3004, 1759, 1760, 1757, 1758, 1323,
     1766, 1719};

std::set<uint32_t> need_clear_task_id_set(need_clear_task_id_arr, need_clear_task_id_arr + sizeof(need_clear_task_id_arr)/sizeof(need_clear_task_id_arr[0]));// = std::set<uint32_t>(need_clear_task_id_arr, sizeof(need_clear_task_id_arr)/sizeof(need_clear_task_id_arr[0]));

bool is_need_clear_task_id(uint32_t task_id)
{
	if (need_clear_task_id_set.find(task_id) != need_clear_task_id_set.end()) {
		return true;
	}
	return false;
}

uint32_t outmoded_ring_task_arr[] = 
{5001, 6001, 6002, 6003, 6011, 6012, 6013, 6021, 6022, 6023, 6031, 6032, 6033,
 6041, 6042, 6043, 5002, 6101, 6102, 6103, 6111, 6112, 6113, 6121, 6122, 6123, 
 6131, 6132, 6133, 6141, 6142, 6143 
};

std::set<uint32_t> outmoded_ring_task_set(outmoded_ring_task_arr, outmoded_ring_task_arr + sizeof(outmoded_ring_task_arr)/sizeof(outmoded_ring_task_arr[0]));

bool is_outmoded_ring_task(uint32_t taskid)
{
    if (outmoded_ring_task_set.find(taskid) != outmoded_ring_task_set.end()) {
        return true;
    }
    return false;
}


items_upgrade_mrg g_items_upgrade_mrg;

swap_action_mrg	g_swap_action_mrg;


char_content_t char_content[100];

limit_data_mrg g_limit_data_mrg;
Summon_dragon_list_mrg g_summon_dragon_list;

mem_stream net_stream(cli_proto_max_len);

team_top10_chache_t  g_team_top10_chache;
team_active_top100_cache_t g_team_active_chache;

//contest.cpp
pvp_game_rank_cache_t g_pvp_game_cache;

//active_config
static active_data_mgr   s_active_data_mgr;
active_data_mgr*   get_active_data_mgr()
{
	return &s_active_data_mgr;
}

static event_trigger_mgr   s_event_trigger_mgr;
event_trigger_mgr*   get_event_trigger_mgr()
{
	return &s_event_trigger_mgr;
}

//god_guard
static god_guard_mgr   s_god_guard_mgr;
god_guard_mgr*   get_god_guard_mgr()
{
	return &s_god_guard_mgr;
}

server_config_t g_server_config[max_server_config_id];

//randmon_event
static sum_event_mgr g_sum_event_mgr;
sum_event_mgr * get_sum_event_mgr()
{
    return &g_sum_event_mgr;
}

reward_player_cache_t g_reward_player_cache;

char branch_sys_ip[128];
uint16_t branch_sys_port;
//--------------------------------
