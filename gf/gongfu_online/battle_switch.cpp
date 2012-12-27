#include "battle_switch.hpp"
#include "battle.hpp"
#include "trade.hpp"
#include "global_data.hpp"
#include "ap_toplist.hpp"
#include "temporary_team.hpp"

using namespace taomee;


//int battle_switch_fd = -1;
//int trade_switch_fd = -1;

static void send_init_pkg();

void send_init_pkg()
{
	 KDEBUG_LOG(0, "SEND INIT PKG");
	 int idx = 0;
	 uint8_t pkgbody[24];

	 pack_h(pkgbody, static_cast<uint16_t>(config_get_intval("domain", 0)), idx);
	 pack_h(pkgbody, get_server_id(), idx);
	 pack(pkgbody, config_cache.bc_elem->bind_ip, 16, idx);
	 pack_h(pkgbody, static_cast<uint16_t>(config_cache.bc_elem->bind_port), idx);

	 send_to_battle_switch(0, bsw_cmd_init_pkg, sizeof(pkgbody), pkgbody, 0);   // sender id 0
}

int  send_bsw_keepalive_pkg(void* owner, void* data)
{
	send_to_battle_switch(0, bsw_cmd_keepalive, 0, 0, 0);   // send keepalive header package to battle switch
	ADD_TIMER_EVENT_EX(&g_events, send_bsw_keepalive_pkg_idx, 0, get_now_tv()->tv_sec + 30);
	return 0;
}



/*------------------------------------------------------------------------------*/
void notify_user_on_off(player_t* p, uint8_t login)
{
	int idx = 0;
	uint8_t buf[1024] = {0};
	pack_h(buf, login, idx);
	pack_h(buf, p->role_tm, idx);
	pack_h(buf, p->role_type, idx);
	pack(buf, p->nick, sizeof(p->nick), idx);
	pack_h(buf, p->achieve_point, idx);
	pack_h(buf, p->last_update_ap_tm, idx);
	send_to_battle_switch(p, bsw_cmd_user_login_noti, idx, buf, p->id);
}

int  auto_join_room(player_t* p, int32_t stage_id, int32_t diffcult)
{
	int idx = 0;
	uint8_t buf[8] = { 0 };
	pack_h(buf, stage_id, idx);
	pack_h(buf, diffcult, idx);
	return send_to_battle_switch(p, bsw_cmd_auto_join_room, sizeof(buf), buf, p->id);
}

int  auto_join_pvp_room(player_t* p, uint32_t match_key, int32_t pvp_mode)
{
	int idx = 0;
//	uint8_t buf[8] = { 0 };
	uint8_t buf[36] = { 0 };
	pack_h(buf, p->id, idx);
	pack_h(buf, p->role_tm, idx);
	pack_h(buf, p->role_type, idx);
	pack(buf, p->nick, 16, idx);
	pack_h(buf, static_cast<uint32_t>(match_key), idx);
	pack_h(buf, pvp_mode, idx);
//	pack_h(buf, p->attire_gs, idx);

	KDEBUG_LOG(p->id, "%u %u ", match_key, pvp_mode);
	return send_to_battle_switch(p, bsw_cmd_auto_join_pvp_room, sizeof(buf), buf, p->id);
}

int  get_room_list(player_t *p, uint32_t begin_index)
{
	int idx = 0;
	uint8_t buf[4] = { 0 };
	pack_h(buf, begin_index, idx);
	return send_to_battle_switch(p, bsw_cmd_room_list, sizeof(buf), buf, p->id);
}

int get_ap_toplist(player_t* p, uint32_t begin_index, uint32_t end_index)
{
	int idx = 0;
	uint8_t buf[1024] = { 0 };
	pack_h(buf, begin_index, idx);
	pack_h(buf, end_index, idx);
	return send_to_battle_switch(p, bsw_cmd_get_ap_toplist, idx, buf, p->id);
}


void connect_to_battle_switch()
{	
	battle_switch_fd = connect_to_service(config_get_strval("battle_switch_name"),  0, 65535, 1);
	if (battle_switch_fd != -1) {
		KDEBUG_LOG(0, "BEGIN CONNECTING TO SWITCH AND SEND INIT PKG");
		send_init_pkg();				      
	}
}

void connect_to_trade_switch()
{
	trade_switch_fd = connect_to_service(config_get_strval("trade_switch_name"),  0, 65535, 1);
	if (trade_switch_fd != -1) {
		KDEBUG_LOG(0, "BEGIN CONNECTING TO TRADE SWITCH AND SEND INIT PKG");
		send_init_pkg();				      
	}
}

int recommend_market(player_t *p)
{
	uint32_t online_id = get_server_id();
	uint32_t tr_svr = 0;
	uint32_t min_market = 0;
	uint32_t max_market = 0;
	uint32_t c_market_frist = 0;
	if (online_id <= max_old_online_id)
	{
		c_market_frist = (online_id - 1) * max_market_pre_online + 1; 
	} else {
		c_market_frist = (online_id - max_old_online_id - 1) * max_market_pre_online + 1;
	}

	tr_svr = (c_market_frist - 1) / max_market_num  + 1;
	min_market = (c_market_frist - 1) % max_market_num;
	max_market = min_market + max_market_pre_online - 1; 

	int idx = 0;
	uint8_t buf[12] = {0};
	pack_h(buf, tr_svr, idx);
	pack_h(buf, min_market, idx);
	pack_h(buf, max_market, idx);
	return send_to_trade_switch(p, bsw_cmd_recommend_market, sizeof(buf), buf, p->id);
}

int trade_sw_change_market(player_t *p, uint32_t server_id, uint32_t market_id)
{
	int idx = 0;
	uint8_t buf[16] = {0};
	pack_h(buf, p->trade_grp->server_id, idx);
	pack_h(buf, p->trade_grp->market_id, idx); 	
	pack_h(buf, server_id, idx);
	pack_h(buf, market_id, idx);
	return send_to_trade_switch(p, bsw_cmd_change_market, sizeof(buf), buf, p->id);
}


int  consume_global_data(uint32_t item_id, uint32_t cnt, uint32_t add_del_type, uint32_t reset_type)
{
	int idx = 0;
	uint8_t buf[16] = { 0 };
	pack_h(buf, add_del_type, idx);
	pack_h(buf, reset_type, idx);
	pack_h(buf, item_id, idx);
	pack_h(buf, cnt, idx);
	return send_to_battle_switch(0, bsw_cmd_consume_global_data, sizeof(buf), buf, 0);
}

int reset_global_data(uint32_t add_del_type, uint32_t reset_type)
{
	int idx = 0;
    TRACE_LOG("reset_global_data [%u | %u]", add_del_type, reset_type);
	uint8_t buf[16] = { 0 };
	pack_h(buf, add_del_type, idx);
	pack_h(buf, reset_type, idx);
	return send_to_battle_switch(0, bsw_cmd_reset_global_data, idx, buf, 0);
}

void init_bsw_keepalive_timer()
{
	 //ADD_TIMER_EVENT(&g_events, send_bsw_keepalive_pkg, 0, get_now_tv()->tv_sec + 30);
}

int connect_to_battle_switch_timely(void* owner, void* data)
{
	if (!data) {
		ADD_TIMER_EVENT_EX(&g_events, connect_to_battle_switch_timely_idx, reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 10);
	} else if (battle_switch_fd == -1) {
		connect_to_battle_switch();
		if (battle_switch_fd == -1)
		{
			ADD_TIMER_EVENT_EX(&g_events, connect_to_battle_switch_timely_idx, reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 10);
		}
	}
	return 0;
}

int connect_to_trade_switch_timely(void* owner, void* data)
{
	if (!data) {
		ADD_TIMER_EVENT_EX(&g_events, connect_to_trade_switch_timely_idx, reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 10);
	} else if (trade_switch_fd == -1) {
		connect_to_trade_switch();
		if (trade_switch_fd == -1)
		{
			ADD_TIMER_EVENT_EX(&g_events, connect_to_trade_switch_timely_idx, reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 10);
		}
	}
	return 0;
}


int send_to_battle_switch(const player_t* p, uint16_t cmd, uint32_t body_len, void* body_buf, userid_t sender_id)
{
	static uint8_t bsw_pkg[bswproto_max_len] = {0};
	if (battle_switch_fd == -1) {
		connect_to_battle_switch();
	}
	int len = sizeof(battle_switch_proto_t) + body_len;
    if ((battle_switch_fd == -1) || (body_len > static_cast<int>(sizeof(bsw_pkg) - sizeof(battle_switch_proto_t)))) {
		ERROR_LOG("FAILED SENDING PACKAGE TO SWITCH\t[switch_fd=%d]", battle_switch_fd);
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
		KDEBUG_LOG(p->id, "SEND REQ TO SWITCH\t[uid=%u cmd=%u waitcmd=%u fd=%u]", p->id, cmd, p->waitcmd, battle_switch_fd);
	}
	return net_send(battle_switch_fd, bsw_pkg, len);
}

int send_to_trade_switch(const player_t* p, uint16_t cmd, uint32_t body_len, void* body_buf, userid_t sender_id)
{
	static uint8_t bsw_pkg[bswproto_max_len] = {0};
	if (trade_switch_fd == -1) {
		connect_to_trade_switch();
	}
	int len = sizeof(battle_switch_proto_t) + body_len;
    if ((trade_switch_fd == -1) || (body_len > static_cast<int>(sizeof(bsw_pkg) - sizeof(battle_switch_proto_t)))) {
		ERROR_LOG("FAILED SENDING PACKAGE TO TRADE SWITCH\t[switch_fd=%d]", trade_switch_fd);
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
		KDEBUG_LOG(p->id, "SEND REQ TO TRADE SWITCH\t[uid=%u cmd=%u waitcmd=%u fd=%u]", p->id, cmd, p->waitcmd, trade_switch_fd);
	}
	return net_send(trade_switch_fd, bsw_pkg, len);
}



void handle_battle_switch_return(battle_switch_proto_t* data, uint32_t len)
{
	uint16_t waitcmd = data->seq & 0xFFFF; // lower 16 bytes
	int   connfd  = data->seq >> 16; //higher 16 bytes

	player_t * p = NULL;
	if (data->seq) { 	
		p = get_player_by_fd(connfd);
		if (!p || p->waitcmd != waitcmd) {
			ERROR_LOG("process connection to switch error\t[uid=%u cmd=%u seq=%X]",
					data->sender_id, waitcmd, data->seq);
			return; 							
		}
	} else {
		p = get_player(data->sender_id);
		if (!p) {
			return;
		}
	}

//	uint32_t bodylen = data->len - sizeof(battle_switch_proto_t);
	TRACE_LOG("%u %u %u %u", p->id, p->waitcmd, data->cmd, data->ret);
	switch (data->cmd) {
		case bsw_cmd_auto_join_room:
			btlsw_auto_join_room_callback(p, data, len);
			break;
		case bsw_cmd_auto_join_pvp_room:
			btlsw_auto_join_pvp_room_callback(p, data, len);
			break;
		case bsw_cmd_recommend_market:
			trade_sw_recommend_market_callback(p, data, len);
			break;
		case bsw_cmd_change_market:
			trade_sw_change_market_callback(p, data, len);
			break;

		case bsw_cmd_get_ap_toplist:
			btlsw_get_ap_toplist_callback(p, data, len);
			break;			
		case bsw_cmd_get_contest_list:
			btlsw_get_contest_list_callback(p, data, len);
			break;
		case bsw_cmd_auto_join_contest_group:
			btlsw_auto_join_contest_group_callback(p, data, len);
			break;
		case bsw_cmd_list_contest_group_info:
			btlsw_list_contest_group_callback(p, data, len);
			break;
		case bsw_cmd_leave_contest_group:
			btlsw_leave_contest_group_callback(p, data, len);
			break;
		case bsw_cmd_contest_group_auto_msg:
			btlsw_contest_group_auto_msg_callback(p, data, len);
			break;
		case bsw_cmd_contest_group_enter_room:
			btlsw_contest_group_enter_room_callback(p, data, len);
			break;
		case bsw_cmd_contest_btl:
			btlsw_contest_btl_callback(p, data, len);
			break;
		case bsw_cmd_contest_all_over:
			btlsw_contest_all_over_callback(p, data, len);
			break;
		case bsw_cmd_player_win_contest:
			btlsw_player_win_contest_callback(p, data, len);
			break;
		case bsw_cmd_contest_guess_champion:
			btlsw_contest_guess_champion_callback(p, data, len);
			break;
		case bsw_cmd_list_all_contest_player:
			btlsw_list_all_contest_player_callback(p, data, len);
			break;
		case bsw_cmd_list_passed_contest_info:
			btlsw_list_passed_contest_info_callback(p, data, len);
			break;
		case bsw_cmd_team_contest_auto_join:
			btlsw_team_contest_auto_join_callback(p, data, len);
			break;
		case bsw_cmd_team_contest_get_server_id:
			btlsw_team_contest_get_server_id_callback(p, data, len);
			break;

		default:
			WARN_LOG("btl sw cmdid not existed: %u", data->cmd);
			break;
	}
}

int get_pvp_room_info(player_t *p, uint32_t pvp_mode)
{
//	ERROR_LOG("SEND BSW PVP ROOM LIST %u", p->id);
	int idx = 0;
	uint8_t buf[4] = {0};
	pack_h(buf, pvp_mode, idx);
	return send_to_battle_switch(p, bsw_cmd_get_contest_list, idx, buf, p->id);
}

int auto_join_contest_group(player_t * p)
{
	int idx = 0;
	uint8_t buf[32] = {0};
	pack_h(buf, p->id, idx);
	pack_h(buf, p->role_tm, idx);
	pack_h(buf, p->role_type, idx);
	pack(buf, p->nick, max_nick_size, idx); 
	pack_h(buf, (uint32_t)(p->lv), idx);
	pack_h(buf, p->exploit, idx);
//	uint32_t out_flag = 0;
//	pack_h(buf, out_flag, idx);
	return send_to_battle_switch(p, bsw_cmd_auto_join_contest_group, idx, buf, p->id);
}

int list_contest_group_info(player_t * p)
{
	return send_to_battle_switch(p, bsw_cmd_list_contest_group_info, 0, NULL, p->id);
}

int leave_contest_group(player_t * p)
{
	return send_to_battle_switch(p, bsw_cmd_leave_contest_group, 0, NULL, p->id);
}

int btsw_enter_contest_btl(player_t * p)
{
	return send_to_battle_switch(p, bsw_cmd_contest_btl, 0, NULL, p->id);
}

int player_guess_contest_champion(player_t * p, uint32_t guess_id, uint32_t guess_tm)
{
	int idx = 0;
	uint8_t buf[8] = {0};
	pack_h(buf, guess_id, idx);
	pack_h(buf, guess_tm, idx);
	return send_to_battle_switch(p, bsw_cmd_contest_guess_champion, idx, buf, p->id);
}

int btlsw_team_contest_auto_join(player_t * p)
{
	int idx = 0;
	uint8_t buf[4] = {0};
	pack_h(buf, p->team_info.team_id, idx);
	return send_to_battle_switch(p, bsw_cmd_team_contest_auto_join, idx, buf, p->id);
}


int btlsw_team_contest_get_server_id(player_t * p)
{
	int idx = 0;
	uint8_t buf[4] = {0};
	pack_h(buf, p->team_info.team_id, idx);
	return send_to_battle_switch(p, bsw_cmd_team_contest_get_server_id, idx, buf, p->id);
}

int list_all_contest_player(player_t * p)
{
	return send_to_battle_switch(p, bsw_cmd_list_all_contest_player, 0, NULL, p->id);
}


int list_all_passed_contest(player_t * p)
{
	
	return send_to_battle_switch(p, bsw_cmd_list_passed_contest_info, 0, NULL, p->id);
}

bool init_btl_switch_proto_handles()
{
	return true;
}

bool final_btl_switch_proto_handles()
{
	return true;
}



//temp team pvp


int  temp_team_pvp_match(player_t* p)
{
    return auto_join_pvp_room(p, get_tmp_team_pvp_match_value(p), pvp_temporary_team);
}
