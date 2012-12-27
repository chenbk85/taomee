#include <cstring>
#include <libtaomee++/inet/pdumanip.hpp>
#include <arpa/inet.h>

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
}

#include "battle_switch.hpp"
#include "battle_impl.hpp"

using namespace taomee;

int battle_switch_fd = -1;
/*! for packing protocol data and send to battle switch. 2M */  
uint8_t bswpkgbuf[1 << 21];

static void send_init_pkg();


void send_init_pkg()
{
	DEBUG_LOG("SEND INIT PKG");
	int idx = 0;

	pack_h(bswpkgbuf, get_server_id()  , idx);
	
	send_to_battle_switch(0, bsw_cmd_init_pkg, idx, bswpkgbuf, 0);
}


int notify_create_new_room(Player* p, uint32_t room_index, int32_t stage_id, int32_t diffcult, int32_t hot_join_flag, uint32_t limit_num)
{
	if (hot_join_flag) {
		return 0;
	}

	TRACE_LOG("RM\t[%u %u %u %u %u %u]", p->id, get_server_id(), room_index, stage_id, diffcult, hot_join_flag);
	int idx = 0;
	pack_h(bswpkgbuf, get_server_id(), idx);
	pack_h(bswpkgbuf, room_index, idx);
	pack_h(bswpkgbuf, stage_id, idx);
	pack_h(bswpkgbuf, diffcult, idx);

	return send_to_battle_switch(0, bsw_cmd_create_room, idx, bswpkgbuf, p->id);
}

int notify_destroy_room(Player* p, uint32_t room_index, uint32_t stage_id, uint32_t diffcult)
{
	TRACE_LOG("D R\t%u %u", get_server_id(), room_index);
	int idx = 0;
	pack_h(bswpkgbuf, get_server_id(), idx);
	pack_h(bswpkgbuf, room_index, idx);
	return send_to_battle_switch(0, bsw_cmd_destroy_room, idx, bswpkgbuf, p ? p->id : 0);
}

int notify_join_room(Player* p, uint32_t room_index)
{
	TRACE_LOG("J R\t%u %u %u", p->id, get_server_id(), room_index);
	int idx = 0;
	pack_h(bswpkgbuf, get_server_id(), idx);
	pack_h(bswpkgbuf, room_index, idx);
	return send_to_battle_switch(0, bsw_cmd_join_room, idx, bswpkgbuf, p->id);
}

int notify_leave_room(Player* p, uint32_t room_index)
{
	TRACE_LOG("L R\t%u %u %u", p->id, get_server_id(), room_index);
	int idx = 0;
	pack_h(bswpkgbuf, get_server_id(), idx);
	pack_h(bswpkgbuf, room_index, idx);
	return send_to_battle_switch(0, bsw_cmd_leave_room, idx, bswpkgbuf, p->id);
}

int notify_team_room_start(Player* p, uint32_t room_index)
{
	int idx = 0;
	pack_h(bswpkgbuf, get_server_id(), idx);
	pack_h(bswpkgbuf, room_index, idx);
	return send_to_battle_switch(0, bsw_cmd_team_room_start, idx, bswpkgbuf, p ? p->id: 0);
}

int notify_room_hot_join(Player* p, uint32_t room_index, int hot_join_flag)
{
	int idx = 0;
	pack_h(bswpkgbuf, get_server_id(), idx);
	pack_h(bswpkgbuf, room_index, idx);
	pack_h(bswpkgbuf, hot_join_flag, idx);
	return send_to_battle_switch(0, bsw_cmd_room_hot_join, idx, bswpkgbuf, p->id);
}

#ifndef local_pvp
int notify_create_new_pvp_room(Player* p, uint32_t room_index, int32_t pvp_mode, uint32_t match_key)
{
	int idx = 0;
	TRACE_LOG("CPR\t%u %u %u", p->id, room_index, pvp_mode);
	pack_h(bswpkgbuf, p->id, idx);
	pack_h(bswpkgbuf, p->role_tm, idx);
	pack_h(bswpkgbuf, p->role_type, idx);
	pack(bswpkgbuf, p->nick, 16, idx);
	pack_h(bswpkgbuf, get_server_id(), idx);
	pack_h(bswpkgbuf, room_index, idx);
	pack_h(bswpkgbuf, pvp_mode, idx);
	if (match_key) {
		pack_h(bswpkgbuf, match_key, idx);
	} else {
		pack_h(bswpkgbuf, static_cast<uint32_t>(p->lv), idx);
	}
	return send_to_battle_switch(0, bsw_cmd_create_pvp_room, idx, bswpkgbuf, p->id);
}

int notify_destroy_pvp_room(Player* p, uint32_t room_index, uint32_t pvp_lv, uint32_t pvp_mode)
{
	TRACE_LOG("DPR\t%u %u", p ? p->id : 0, room_index);
	int idx = 0;
	pack_h(bswpkgbuf, get_server_id(), idx);
	pack_h(bswpkgbuf, room_index, idx);
//	pack_h(bswpkgbuf, winner, idx);
	return send_to_battle_switch(0, bsw_cmd_destroy_pvp_room, idx, bswpkgbuf, p ? p->id : 0);
}


int notify_join_pvp_room(Player* p, uint32_t room_index, uint32_t pvp_lv)
{
	TRACE_LOG("JPR\t%u %u", p ? p->id : 0, room_index);
	int idx = 0;
	pack_h(bswpkgbuf, get_server_id(), idx);
	pack_h(bswpkgbuf, room_index, idx);
	pack_h(bswpkgbuf, pvp_lv, idx);
	return send_to_battle_switch(0, bsw_cmd_join_pvp_room, idx, bswpkgbuf, p->id);
}

int notify_leave_pvp_room(Player* p, uint32_t room_index)
{
	TRACE_LOG("LPR\t%u %u", p ? p->id : 0, room_index);
	int idx = 0;
	pack_h(bswpkgbuf, get_server_id(), idx);
	pack_h(bswpkgbuf, room_index, idx);
	return send_to_battle_switch(0, bsw_cmd_leave_pvp_room, idx, bswpkgbuf, p ? p->id: 0);
}

int notify_pvp_room_start(Player* p, uint32_t room_index, uint32_t pvp_lv)
{
	TRACE_LOG("PRS\t%u %u", p ? p->id : 0, room_index);
	int idx = 0;
	pack_h(bswpkgbuf, get_server_id(), idx);
	pack_h(bswpkgbuf, room_index, idx);
	return send_to_battle_switch(0, bsw_cmd_pvp_room_start, idx, bswpkgbuf, p ? p->id: 0);
}
#else
int notify_create_new_pvp_room(Player* p, uint32_t room_index, int32_t pvp_mode, uint32_t match_key)
{
	return 0;
}

int notify_destroy_pvp_room(Player* p, uint32_t room_index)
{
	return 0;
}


int notify_join_pvp_room(Player* p, uint32_t room_index)
{
	return 0;
}

int notify_leave_pvp_room(Player* p, uint32_t room_index)
{
	return 0;
}

int notify_pvp_room_start(Player* p, uint32_t room_index)
{
	return 0;
}

#endif
int notify_clear_all_rooms()
{
	int idx = 0;
	pack_h(bswpkgbuf, get_server_id(), idx);
	return send_to_battle_switch(0, bsw_cmd_clear_all_room, idx, bswpkgbuf, 0);
}

int notify_pvp_over_2_switch(uint32_t room_index, 
							 uint32_t uid,
							 uint32_t pvp_lv, 
							 uint32_t extra_info)
{
	int idx = 0;
	pack_h(bswpkgbuf, get_server_id(), idx);
	pack_h(bswpkgbuf, room_index, idx);
	pack_h(bswpkgbuf, pvp_lv, idx);
	pack_h(bswpkgbuf, uid, idx);
	pack_h(bswpkgbuf, extra_info, idx);
	return send_to_battle_switch(0, bsw_cmd_pvp_btl_over, idx, bswpkgbuf, uid);
}

void connect_to_battle_switch()
{
	battle_switch_fd = connect_to_service(config_get_strval("battle_switch_name"),  0, 65535, 1);
	if( battle_switch_fd != -1)
	{
		send_init_pkg();
		DEBUG_LOG("BEGIN CONNECTING TO SWITCH AND SEND BATTLE_SERVER ID");
	}
}

int connect_to_battle_switch_timely(void* owner, void* data)
{
	if(!data){
		ADD_TIMER_EVENT(&g_events, connect_to_battle_switch_timely, reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 10);
	} else if (battle_switch_fd == -1){
		connect_to_battle_switch();
		if (battle_switch_fd == -1)
		{
			ADD_TIMER_EVENT(&g_events, connect_to_battle_switch_timely, reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 10);
		}
	}
	return 0;
}

int  send_to_battle_switch(Player* p, uint16_t cmd, uint32_t body_len, void* body_buf, userid_t sender_id)
{
	static uint8_t bsw_pkg[ bsw_proto_max_len ] = { 0 };
	if( battle_switch_fd == -1){
		connect_to_battle_switch();
	}
	int len = sizeof(battle_switch_proto_t) + body_len;
	if(  battle_switch_fd == -1 || body_len > int(sizeof(bsw_pkg) - sizeof(battle_switch_proto_t))){
		ERROR_LOG("FAILED SENDING PACKAGE TO BATTLE SWITCH\t[switch_fd=%d]", battle_switch_fd);
		return -1;
	}

	battle_switch_proto_t* pkg = reinterpret_cast<battle_switch_proto_t*>(bsw_pkg);
	pkg->len = len;
	pkg->seq = (p ? ((p->fd) << 16) | (p->waitcmd) : 0); // set higher 16 bytes fd and lower 16 bytes waitcmd
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->sender_id  = sender_id;

	memcpy(pkg->body, body_buf, body_len);
	if (p) {
		DEBUG_LOG("SEND REQ TO  BATTLE SWITCH\t[uid=%u cmd=%u waitcmd=%u]", p->id, cmd, p->waitcmd);
	}
	return net_send(battle_switch_fd, bsw_pkg, len);
}

void handle_battle_switch_return()
{

}

bool init_battle_switch_handle()
{
	return true;
}

bool final_battle_switch_handle()
{
	return true;
}
