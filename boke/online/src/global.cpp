#include "global.h"
#include "Citem_conf.h"
#include "Csprite.h"
#include "Cmap_conf.h"
#include "Cgame.h"


/*! dbproxy socket fd */
int proxysvr_fd = -1;
/*! siwtch socket fd */
int switch_fd = -1;
int udp_post_fd = -1;
struct sockaddr_in udp_post_addr;
bool is_add_timer_to_connect_switch = false;
int g_log_send_buf_hex_flag;
char statistic_file[255];
//是不是测试环境
bool g_is_test_env;

Csprite_map  *g_sprite_map ;
Ctimer *g_timer_map ;

Ctask_map g_task_map ;
Cmap_conf_map  g_map_conf_map ;
Cgame_conf_map g_game_conf_map;
//id, itemid, count
std::map<uint32_t,std::set<uint32_t> > user_reg_config_map;

//用户注册时，可用物品的配置
Citem_conf_map g_item_conf_map;

// 配置信息
Citem_movement_conf g_item_movement_map;
std::map<uint32_t, Citem_rand_group> g_rand_group_map;
std::map<uint32_t, uint32_t> npc_trans_map;
std::map<uint32_t, std::set<uint32_t> > item_card_map;
