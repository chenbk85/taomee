/*
 * =========================================================================
 *
 *        Filename: global.h
 *
 *        Version:  1.0
 *        Created:  2011-08-16 08:05:28
 *        Description:  
 *
 *        Author:  xxx (xxxj@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */


#ifndef POP_GLOBAL_H
#define POP_GLOBAL_H
#include "benchapi.h"
#include "Ctimer.h"
#define db_proto_max_len   (4096*40)

class Csprite_map;
class Ctask_map ;
class Ctimer ;
class Citem_conf_map;
class Cmap_conf_map;
class Cgame_conf_map;
class Citem_movement_conf;
class Citem_rand_group;

extern Csprite_map  *g_sprite_map ;
extern Ctimer *g_timer_map ;
extern Ctask_map g_task_map ;
extern Citem_conf_map g_item_conf_map;
extern Citem_movement_conf g_item_movement_map;
extern std::map<uint32_t, Citem_rand_group> g_rand_group_map;
extern int  g_log_send_buf_hex_flag;
extern Cmap_conf_map  g_map_conf_map ;
extern Cgame_conf_map g_game_conf_map;

/*! dbproxy socket fd */
extern int proxysvr_fd ;
/*! siwtch socket fd */
extern int switch_fd ;
extern int udp_post_fd ;
extern struct sockaddr_in udp_post_addr;
extern char statistic_file[255];
//是不是测试环境
extern bool g_is_test_env;

extern bool is_add_timer_to_connect_switch ;
extern std::map<uint32_t,std::set<uint32_t> > user_reg_config_map;

// npcid转换表
extern std::map<uint32_t, uint32_t> npc_trans_map;

extern std::map<uint32_t, std::set<uint32_t> > item_card_map;

#endif
