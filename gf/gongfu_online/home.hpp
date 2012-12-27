/**
 *============================================================
 *  @file      home.hpp
 *  @brief    home related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_HOME_HPP_
#define KF_HOME_HPP_

#include <libtaomee++/bitmanip/bitmanip.hpp>

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/log.h>

#include <libtaomee/project/constants.h>
#include <libtaomee/project/types.h>

#include <async_serv/dll.h>
}

#include "fwd_decl.hpp"
#include "cli_proto.hpp"
#include "player.hpp"

/**
  * @brief command id for online-home_server communication
  */
enum home_cmd_t {
	/*! starting command id */
	home_cmd_start			= 10001,
	home_enter_home         = 10001,
	home_enter_other_home  	= 10002,
	home_leave_home         = 10003,	
	home_pet_move           = 10004,	
	home_pet_call           = 10005,//协议发给home_svr表示把宠物放到小屋，home_svr广播这个cli_proto_pet_home_call_broadcast
	home_pet_follow         = 10006,//协议发给home_svr表示让出战的宠物是否跟随,home_svr广播这个cli_proto_call_summon	
	
	home_list_user          = 10007,
	home_list_pet           = 10008,
	home_pve_syn_home_info  = 10009,//在小屋内进入战斗的时候先发这个包同步下小屋数据
	
	home_get_plants_list	= 10030,
	home_set_plant_status	= 10031,
	home_pick_fruit			= 10032,
	home_sow_plant_seed     = 10033,

    home_get_decorate_list  = 10040,
    home_set_decorate       = 10041,
    home_add_decorate_lv    = 10042,
    home_get_decorate_reward= 10043,

	home_player_walk        = 10050,
	home_walk_keyboard      = 10051,	
	home_player_stop        = 10052,
	home_player_jump        = 10053,
	home_player_talk        = 10054,
	home_query_player_count = 10055,

	home_player_enter_battle = 10070,
	home_player_leave_battle = 10071,

	home_player_off_line     = 10072,

	home_kick_off_player     = 10073,
	home_get_home_attr       = 10074,
	home_level_up            = 10075,

	home_transmit_only       = 12001,
	/*! ending command id */
	home_cmd_end,

	/*! max number of commands supported */
	home_cmd_max				= 13000
};

/**
  * @brief home error type
  */
enum home_err_t {
	home_err_player_not_found	= 1001,
	home_err_can_not_enter      = 1002,
	home_err_can_not_leave      = 1003,
	home_err_not_in_home        = 1004,
	home_err_no_btl_pet         = 1005,   
	home_err_not_kick_off       = 1006,
	home_err_not_home_level_up  = 1007,
	home_err_not_home_fumo_point = 1008,
	home_err_not_home_leveless = 1009,
	home_err_max
};

enum {
	/*! max acceptable length in bytes for each package from/to home server */
	homeproto_max_len	= 8 * 1024,
	
	/*! max number of home servers */
	max_home_svr_num	= 10,
};

/**
   * @brief home_grp_t
   */
struct home_grp_t
{
	uint32_t home_owner_id;
	uint32_t role_regtime;
	uint32_t fd_idx;
	uint32_t map_id;
};


#pragma pack(1)

/**
  * @brief protocol type for home server
  */
struct home_proto_t {
	/*! package length */
	uint32_t    len;
	/*! battle group id */
	uint32_t    seq;
	/*! command id */
	uint16_t    cmd;
	/*! errno */
	uint32_t    ret;
	/*! user id */
	userid_t    id;
	/*! package body */
	uint8_t     body[];
};


struct get_home_log_rsp_t {
	uint32_t count;
};

struct get_home_log_item_rsp_t {
	uint32_t op_uid;
	uint32_t op_utm;
	uint32_t role_type;
	char nick[16];
	uint32_t type;
	uint32_t access_type;
	uint32_t tm;

};

#pragma pack()

//------------------------------------------------------------

//-----------------------------------------------------------------------

/**
  * @param p the player who launches the request to home server
  * @param homepkgbuf body of the request
  * @param len length of homepkgbuf
  * @param ptr_bs_fd pointer to home svr fd
  * @return 0 on success, -1 on error
  */
int  send_to_home_svr(player_t* p, const void* homepkgbuf, uint32_t len, int index);
/**
  * @brief init handles to handle protocol packages from home server
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_home_proto_handles();
/**
  * @brief uniform handler to process packages from home server
  * @param data package data
  * @param len package length
  * @param bs_fd home server fd
  */
void handle_home_svr_return(home_proto_t* data, uint32_t len, int home_fd);

//-----------------------------------------------------------------------

/**
 * @brief load home servers' config from home_server.xml file
 * @param cur the pointer to xml root node
 * @return 0 on success, -1 on error
 */
int  load_home_servers(xmlNodePtr cur);
/**
 * @brief reload home svr config 
 */
void reload_home_svr_config();
/**
  * @brief get index of a home server identified by the given 'fd'
  * @param fd fd of a home server
  * @return index of a home server, or -1 if home server identified by 'fd' is not found
  */
int  get_home_fd_idx(int fd);

//-----------------------------------------------------------------------

/*! for packing protocol data and send to home server */
extern uint8_t homepkgbuf[homeproto_max_len];
/*! hold fds to home servers */
extern int home_fds[max_home_svr_num];

//-----------------------------------------------------------------------
/**
  * @brief init header of home server protocol
  * @param p the player who is going to send a pkg to home server
  * @param id
  * @param header header to be initialized
  * @param len length of the whole protocol package
  * @param cmd client command id  
  */
inline void init_home_proto_head(const player_t* p, userid_t id, void* header, int len, uint32_t cmd)
{
	home_proto_t* pkg = reinterpret_cast<home_proto_t*>(header);

	pkg->len = len;
	pkg->seq = ((p) ? ( (p->fd) << 16 ) | (p->waitcmd) : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
}

int get_homesvr_fd_idx(int fd);

inline void free_home_grp(home_grp_t* grp, player_t* p)
{
	if(!grp)return;
	g_slice_free1(sizeof(home_grp_t), grp);
}

inline home_grp_t* alloc_home_grp()
{
	return reinterpret_cast<home_grp_t*>(g_slice_alloc0(sizeof(home_grp_t)));
}

void enter_own_home(player_t* p);

void enter_other_home(player_t* p, uint32_t home_owner_id, uint32_t role_regtime);

void leave_home(player_t* p);

bool check_in_home(player_t* p);

bool check_in_trade(player_t* p);

bool check_enter_home(player_t* p);

bool change_home(player_t* p, uint32_t home_owner_id, uint32_t role_regtime);

bool create_home(player_t* p, uint32_t home_owner_id, uint32_t role_regtime);

home_grp_t* get_home_grp(player_t* p);

int  homesvr_enter_own_home(player_t* p);

int  homesvr_enter_other_home(player_t* p, uint32_t home_owner_id, uint32_t role_regtime);

int  homesvr_leave_home(player_t* p, uint32_t home_owner_id, uint32_t role_regtime);

void pack_home_player_info(uint8_t* buf, int32_t& idx,  player_t* p, uint32_t home_owner_id,  uint32_t role_regtime);

int enter_own_home_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int enter_other_home_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int enter_other_home_random_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int leave_home_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int pet_move_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int homesvr_pet_move(player_t* p, uint32_t pet_tm, uint32_t x, uint32_t y, uint32_t dir);

int homesvr_player_walk(player_t* p, uint32_t x, uint32_t y, uint32_t flag);

int homesvr_walk_keyboard(player_t* p, uint32_t x, uint32_t y, uint8_t dir, uint8_t state);

int homesvr_player_jump(player_t* p, uint32_t x, uint32_t y);

int homesvr_player_stop(player_t* p, uint32_t x, uint32_t y, uint8_t dir);

int homesvr_player_talk(player_t* p, uint8_t* msg, uint32_t msg_len, userid_t recvid);

int pet_home_call_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int homesvr_pet_home_call(player_t*p , uint32_t pet_tm, uint32_t pet_state);

int homesvr_pet_call_callback(player_t* p, home_proto_t* hpkg);

int homesvr_player_enter_battle(player_t* p);

int homesvr_player_leave_battle(player_t* p);

int homesvr_player_home_list_user(player_t* p);

int homesvr_player_home_list_pet(player_t* p);

int homesvr_enter_other_home_callback(player_t* p, home_proto_t* hpkg);

int homesvr_enter_home_callback(player_t* p, home_proto_t* hpkg);

int homesvr_leave_home_callback(player_t* p, home_proto_t* hpkg);

int homesvr_transmit_only_callback(player_t* p, home_proto_t* pkg);

int get_plants_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int sow_plant_seed_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

int set_plant_status_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int homesvr_set_plant_status_callback(player_t* p, home_proto_t* hpkg);

int pick_fruit_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int homesvr_pick_fruit_callback(player_t* p, home_proto_t* hpkg);

int get_home_list_pet_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int pve_syn_home_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int homesvr_pve_syn_home_info(player_t* p);

int homesvr_pve_syn_home_info_callback(player_t* p, home_proto_t* hpkg);

int homesvr_home_owner_off_line(player_t* p);

int home_kick_off_guest_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int homesvr_kick_off_guest(player_t* p, uint32_t user_id, uint32_t role_regtime);

int homesvr_kick_off_player_callback(player_t* p, home_proto_t* pkg);

int get_home_attr_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int homesvr_home_attr(player_t* p);

int homesvr_sow_plant_seed(player_t *p , uint32_t field_id, uint32_t plant, uint32_t seed);

int homesvr_sow_plant_seed_callback(player_t *p, home_proto_t * pkg);

int homesvr_pick_plant(player_t *p, uint32_t field_id);

int homesvr_pick_plant_callback(player_t *p, home_proto_t * pkg);

int insert_vitality_2_home_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int home_level_up_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int homesvr_level_up(player_t* p);

int homesvr_level_up_callback(player_t* p, home_proto_t* pkg);

int get_home_log_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int db_get_home_log_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

//------------------- for decorate ---------------------
int get_decorate_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int set_decorate_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int add_decorate_lv_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int homesvr_add_decorate_lv_callback(player_t* p, home_proto_t* hpkg);

int get_decorate_reward_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int homesvr_get_decorate_reward_callback(player_t* p, home_proto_t* hpkg);

int query_home_player_count_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int homesvr_query_home_player_count_callback(player_t* p, home_proto_t* hpkg);
#endif //KF_HOME_HPP_

