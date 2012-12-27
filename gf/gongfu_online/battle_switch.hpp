#ifndef _BATTLE_SWITCH_H
#define _BATTLE_SWITCH_H

#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {

#include <stdint.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>
#include <libtaomee/log.h>

#include <libtaomee/project/stat_agent/msglog.h>

#include <async_serv/net_if.h>

}

#include "utils.hpp"

#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "fwd_decl.hpp"
#include "player.hpp"


#pragma pack(1)


struct battle_switch_proto_t {
	uint32_t    len;
	uint32_t    seq;
	uint16_t    cmd;
	uint32_t    ret;
	uint32_t    sender_id;
	uint8_t     body[];
};

#pragma pack()


enum battle_switch_cmd {
 	bsw_cmd_init_pkg = 60001,
	bsw_cmd_keepalive = 60002,
	bsw_cmd_user_login_noti = 61002,
	bsw_cmd_room_list       = 61003,
	bsw_cmd_auto_join_room  = 61004,
	bsw_cmd_auto_join_pvp_room = 61005,
	bsw_cmd_recommend_market   = 61006,
	bsw_cmd_change_market = 61007,
	bsw_cmd_get_ap_toplist = 61008,
	bsw_cmd_get_contest_list = 61009,
	bsw_cmd_auto_join_contest_group = 61010,
	bsw_cmd_list_contest_group_info = 61011,
	bsw_cmd_leave_contest_group     = 61012,
	bsw_cmd_contest_group_auto_msg = 61013,
	bsw_cmd_contest_group_enter_room = 61014,
	bsw_cmd_contest_btl = 61015,
	bsw_cmd_contest_all_over = 61016,
	bsw_cmd_player_win_contest = 61017,
	bsw_cmd_contest_guess_champion = 61018,
	bsw_cmd_list_all_contest_player = 61019,
	bsw_cmd_list_passed_contest_info = 61020,
	bsw_cmd_team_contest_auto_join   = 61031,
	bsw_cmd_team_contest_get_server_id = 61032,

	bsw_cmd_consume_global_data = 63101,
	bsw_cmd_reset_global_data   = 63102,
};

enum {
	    bswproto_max_len = 8192
};

typedef int (*btl_switch_handle)(player_t* p, battle_switch_proto_t* pkg);

extern int battle_switch_fd;
extern int trade_switch_fd;

bool init_btl_switch_proto_handles();

bool final_btl_switch_proto_handles();

void connect_to_battle_switch();

void connect_to_trade_switch();

void init_bsw_keepalive_timer();

int connect_to_battle_switch_timely(void* owner, void* data);

int connect_to_trade_switch_timely(void * owner, void *data);

int send_to_battle_switch(const player_t* p, uint16_t cmd, uint32_t body_len, void* body_buf, userid_t sender_id);

int send_to_trade_switch(const player_t *p, uint16_t cmd, uint32_t bodylen, void * body, userid_t sender);

void handle_battle_switch_return(battle_switch_proto_t* data, uint32_t len);

void notify_user_on_off(player_t* p, uint8_t login);

int  auto_join_room(player_t* p, int32_t stage_id, int32_t diffcult);

int  auto_join_pvp_room(player_t* p, uint32_t match_key,  int32_t pvp_mode);

int  get_room_list(player_t *p, uint32_t begin_index);

int recommend_market(player_t *p);

int get_pvp_room_info(player_t * p, uint32_t pvp_mode);

int trade_sw_change_market(player_t *p, uint32_t server_id, uint32_t market_id);

int  consume_global_data(uint32_t item_id, uint32_t cnt, uint32_t add_del_type = 0, uint32_t reset_type = 2);

int reset_global_data(uint32_t add_del_type, uint32_t reset_type);

int send_bsw_keepalive_pkg(void *owner, void *data);

int get_ap_toplist(player_t* p, uint32_t begin_index, uint32_t end_index);

int auto_join_contest_group(player_t * p);

int list_contest_group_info(player_t * p);

int leave_contest_group(player_t * p);

int btsw_enter_contest_btl(player_t * p);

int player_guess_contest_champion(player_t * p, uint32_t guess_id, uint32_t guess_tm);

int list_all_contest_player(player_t * p);

int list_all_passed_contest(player_t * p);

int btlsw_team_contest_auto_join(player_t * p);

int btlsw_team_contest_get_server_id(player_t * p);

int  temp_team_pvp_match(player_t* p);


#endif



