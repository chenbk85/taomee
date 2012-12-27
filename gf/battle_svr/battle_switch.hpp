#ifndef _BATTLE_SWITCH_H
#define _BATTLE_SWITCH_H

extern "C" {
#include <libtaomee/project/types.h>
#include <stdint.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>
#include <libtaomee/log.h>
#include <libtaomee/project/stat_agent/msglog.h>
#include <async_serv/net_if.h>
}

#include "fwd_decl.hpp"
#include "player.hpp"

enum bsw_cmd_t
{
	bsw_cmd_init_pkg = 62001,
	bsw_cmd_create_room = 62002,
	bsw_cmd_destroy_room = 62003,
	bsw_cmd_join_room = 62004,
	bsw_cmd_leave_room = 62005,
	bsw_cmd_clear_all_room = 62006,
	bsw_cmd_room_hot_join = 62007,
	bsw_cmd_team_room_start = 62008,

	bsw_cmd_create_pvp_room = 63001,
	bsw_cmd_destroy_pvp_room = 63002,
	bsw_cmd_join_pvp_room = 63003,
	bsw_cmd_leave_pvp_room = 63004,
	bsw_cmd_pvp_room_start = 63005,
	bsw_cmd_pvp_btl_over  = 63006
	
};


enum {
	bsw_proto_max_len = 8 * 1024,
};
#pragma pack(1)
struct battle_switch_proto_t 
{
	uint32_t    len;
	uint32_t    seq;
	uint16_t    cmd;
	uint32_t    ret;
	uint32_t    sender_id;
	uint8_t     body[];
};
#pragma pack()


extern int battle_switch_fd;

void connect_to_battle_switch();

int  send_to_battle_switch(Player* p, uint16_t cmd, uint32_t body_len, void* body_buf, userid_t sender_id);

int connect_to_battle_switch_timely(void* owner, void* data);

void handle_battle_switch_return();

bool init_battle_switch_handle();

bool final_battle_switch_handle();

int notify_create_new_room(Player* p, uint32_t room_index, int32_t stage_id, int32_t diffcult, int32_t hot_join_flag, uint32_t limit_num = 2);

int notify_destroy_room(Player* p, uint32_t room_index, uint32_t stage_id, uint32_t diffcult);
int notify_join_room(Player* p, uint32_t room_index);
int notify_leave_room(Player* p, uint32_t room_index);
int notify_team_room_start(Player* p, uint32_t room_index);


int notify_join_pvp_room(Player* p, uint32_t room_index, uint32_t pvp_lv);

int notify_leave_pvp_room(Player* p, uint32_t room_index);
int notify_create_new_pvp_room(Player* p, uint32_t room_index, int32_t pvp_mode, uint32_t match_key = 0);

int notify_destroy_pvp_room(Player* p, uint32_t room_index, uint32_t pvp_lv, uint32_t pvp_mode);

int notify_pvp_room_start(Player* p, uint32_t room_index, uint32_t pvp_lv);

int notify_room_hot_join(Player* p, uint32_t room_index, int hot_join_flag);

int notify_pvp_over_2_switch(uint32_t room_index,
	   						uint32_t uid, 
							uint32_t pvp_lv, 
							uint32_t extra_info = 0); 


int notify_clear_all_rooms();

#endif
