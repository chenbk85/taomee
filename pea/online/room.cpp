#include "cli_proto.hpp"
#include "player.hpp"
#include "battle.hpp"
#include "battle_switch.hpp"
#include "utils.hpp"

#include "room.hpp"



/* return: true: 在大厅, false: 不在大厅 */
bool is_player_in_hall(player_t *p)
{
	return (p->room.in_hall);
}

/* return: true: 在房间里, false: 不在房间里 */
bool is_player_in_room(player_t *p)
{
	return (is_player_in_hall(p) && p->room.room_id);
}

int pvp_room_tell_players_start_battle(player_t* p, uint32_t btl_id, uint32_t fd_idx)
{
	btlsw_pvp_room_start_battle_in in;

	in.btl_id = btl_id;
	in.fd_idx = fd_idx;
	return send_to_battle_switch(p, btlsw_pvp_room_start_battle_cmd, &in);
}


//------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------

int pack(player_t *p_player, room_pet_info_t *p_pet)
{
	assert(p_player);
	assert(p_pet);

    if (p_player->check_module(MODULE_PET) && p_player->fight_pet) {
        p_pet->pet_id = p_player->fight_pet->m_id;
        p_pet->pet_level = p_player->fight_pet->m_level;
        p_pet->skill_1 = p_player->fight_pet->m_skill.m_skill_1;
        p_pet->skill_2 = p_player->fight_pet->m_skill.m_skill_2;
        p_pet->skill_3 = p_player->fight_pet->m_skill.m_skill_3;
        p_pet->uni_skill = p_player->fight_pet->m_skill.m_uni_skill;
    }

    return 0;
}
int pack(player_t *p_player, vector<simple_equip_info_t> &simple_equip_info_vec)
{
	assert(p_player);

    if (p_player->check_module(MODULE_EQUIP)) {
        item *equips[] = {
			p_player->head,
			p_player->eye,
			p_player->glass,
			p_player->body,
			p_player->tail,
			p_player->suit,
		};

        for (uint32_t i = 0; i < sizeof(equips) / sizeof(equips[0]); i++) {
            item *p_item = equips[i];
            if (!p_item) {
                continue;
            }
            simple_equip_info_t e;
            e.equip_id = p_item->item_id();

            simple_equip_info_vec.push_back(e);
        }
    }
    return 0;
}


//------------------------------------------------------------------
// implement functions
//------------------------------------------------------------------
void pack_room_player_show_info(player_t *p, room_player_show_info_t *info)
{
	info->uid = p->id;
	info->role_tm = p->role_tm;
	info->team_id = 0; // 共用结构体, 兼容
	info->seat_id = 0; // 共用结构体, 兼容
	info->player_status = 0; // 共用结构体, 兼容
	memcpy(info->nick, p->nick, MAX_NICK_SIZE);
	info->model_info.eye_model = p->eye_model;
	info->model_info.resource_id = p->resource_id;

	c_pet *fight_pet = p->fight_pet;
	if (fight_pet) {
		info->pet_info.pet_id = fight_pet->m_id;
		info->pet_info.pet_level = fight_pet->m_level;
		info->pet_info.skill_1 = fight_pet->m_skill.m_skill_1;
		info->pet_info.skill_2 = fight_pet->m_skill.m_skill_2;
		info->pet_info.skill_3 = fight_pet->m_skill.m_skill_3;
		info->pet_info.uni_skill = fight_pet->m_skill.m_uni_skill;
	}

	item *tmp_equips[] = {
		p->head,
		p->eye,
		p->glass,
		p->body,
		p->tail,
		p->suit,
	};

	for (uint32_t i = 0; i < sizeof(tmp_equips) / sizeof(tmp_equips[0]); i++) {
		item *tmp = tmp_equips[i];
		if (!tmp) {
			continue;
		}
		simple_equip_info_t e;
		e.equip_id = tmp->item_id();
		info->equip_info.push_back(e);
	}
}

/* 更新房间属性 */
void pvp_room_update_attr(room_t *room, room_info_t *info)
{
	room->map_id = info->map_id;
	room->map_lv = info->map_lv;
}

//------------------------------------------------------------------
// proto functions
// DEFAULT_ARG = player_t * p, Cmessage * c_in, Cmessage * c_out, void * param
//------------------------------------------------------------------
//------------------------------------------------------------------
// 进入大厅
//------------------------------------------------------------------
int cli_proto_player_enter_hall(DEFAULT_ARG)
{
	btlsw_player_enter_hall_in out;
	room_player_show_info_t *info = &(out.player_info);

	pack_room_player_show_info(p, info);
	return send_to_battle_switch(p, btlsw_player_enter_hall_cmd, &out);
}

int btlsw_player_enter_hall(DEFAULT_ARG)
{
	memset(&(p->room), 0, sizeof(p->room));
	p->room.in_hall = player_in_hall;
	DEBUG_TLOG("player_enter_hall, u=%u, olid=%u", p->id, get_server_id());
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

//------------------------------------------------------------------
// 离开大厅
//------------------------------------------------------------------

int cli_proto_player_exit_hall(DEFAULT_ARG)
{
	return send_to_battle_switch(p, btlsw_player_exit_hall_cmd, 0);
}

int btlsw_player_exit_hall(DEFAULT_ARG)
{
	p->room.in_hall = player_not_in_hall;
	p->room.room_id = 0;
	DEBUG_TLOG("player_exit_hall, u=%u, olid=%u", p->id, get_server_id());
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

//------------------------------------------------------------------
// 拉取房间列表
//------------------------------------------------------------------
int cli_proto_pvp_room_list(DEFAULT_ARG)
{
	cli_proto_pvp_room_list_in *p_in = P_IN;
	btlsw_pvp_room_list_in out, *p_out = &out;
	p_out->room_mode = p_in->room_mode;
	p_out->count = p_in->count;
	p_out->turn = p_in->turn;
	p_out->bound_roomid = p_in->bound_roomid;
    return send_to_battle_switch(p, btlsw_pvp_room_list_cmd, &out);
}

int btlsw_pvp_room_list(DEFAULT_ARG)
{
    btlsw_pvp_room_list_out *p_in = P_IN;
    cli_proto_pvp_room_list_out out, *p_out = &out;

	p_out->room_list = p_in->room_list;

    return send_to_player(p, p_out, p->waitcmd, 1);
}

//------------------------------------------------------------------
// 按房间编号拉取房间信息
//------------------------------------------------------------------
int cli_proto_pvp_query_room_info(DEFAULT_ARG)
{
	cli_proto_pvp_query_room_info_in *p_in = P_IN;
    btlsw_pvp_query_room_info_in out, *p_out = &out;

	p_out->room_id = p_in->room_id;
    return send_to_battle_switch(p, btlsw_pvp_query_room_info_cmd, &out);
}

int btlsw_pvp_query_room_info(DEFAULT_ARG)
{
    btlsw_pvp_query_room_info_out *p_in = P_IN;
	cli_proto_pvp_query_room_info_out out, *p_out = &out;

	p_out->room_info = p_in->room_info;

    return send_to_player(p, &out, p->waitcmd, 1);
}


//------------------------------------------------------------------
// pvp 创建房间
//------------------------------------------------------------------
int cli_proto_pvp_create_room(DEFAULT_ARG)
{
	cli_proto_pvp_create_room_in *p_in = P_IN;

    btlsw_pvp_create_room_in out, *p_out = &(out);

	int i = 0;
	int found = 0;
	for (; i < MAX_ROOM_NAME_LEN; i++) {
		p_out->room_name[i] = p_in->room_name[i];
		if (p_out->room_name[i] == '\0') {
			found = 1;
			break;
		}
	}
	if (!found) {
		p_out->room_name[sizeof(p_out->room_name)-1] = '\0';
	}

    int ret = tm_dirty_check(0, p_out->room_name);
    if (ret > 0) {
        ERROR_TLOG("dirty_room_name, u=%u", p->id);
        return send_error_to_player(p, ONLINE_ERR_DIRTY_ROOM_NAME);
    }

    memcpy(p_out->password, p_in->password, sizeof(p_out->password));
	p_out->password[sizeof(p_out->password)-1] = '\0';
    p_out->room_mode = p_in->room_mode;
    p_out->map_id = p_in->map_id;
    p_out->map_lv = p_in->map_lv;

    return send_to_battle_switch(p, btlsw_pvp_create_room_cmd, p_out);
}

int btlsw_pvp_create_room(DEFAULT_ARG)
{
	btlsw_pvp_create_room_out *p_in = P_IN;

	p->room.owner_id  = p->id;
	p->room.team_id = p_in->team_id;
	p->room.seat_id = p_in->seat_id;

	p->room.room_id = p_in->room_info.room_id;
	p->room.type = p_in->room_info.room_type;
	p->room.mode = p_in->room_info.room_mode;
	p->room.map_id = p_in->room_info.map_id;
	p->room.map_lv  = p_in->room_info.map_lv;

	cli_proto_pvp_create_room_out out, *p_out = &out;
	p_out->room_info = p_in->room_info;
	p_out->pet_info = p_in->pet_info;

    return send_to_player(p, &out, p->waitcmd, 1);
}


//------------------------------------------------------------------
// 退出房间（房间广播）
//------------------------------------------------------------------
int cli_proto_leave_room(DEFAULT_ARG)
{
	return send_to_battle_switch(p, btlsw_leave_room_cmd, 0);
}

int btlsw_leave_room(DEFAULT_ARG)
{
	p->room.room_id = 0;
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

int btlsw_player_leave_notify(DEFAULT_ARG)
{
	btlsw_player_leave_notify_out *p_in = P_IN;
	/* 注意:
	 * (1) 通知消息不改变 waitcmd;
	 * (2) 踢人的通知也是这个(其中有表示是自己走的还是被踢的)
	 */
	if (p->id == p_in->leaver_uid) {
		p->room.room_id = 0;
	}
    return send_to_player(p, p_in, cli_proto_player_leave_notify_cmd, 0);
}


//------------------------------------------------------------------
// 踢人
//------------------------------------------------------------------
int cli_proto_kick_room_player(DEFAULT_ARG)
{
	cli_proto_kick_room_player_in * p_in = P_IN;
    btlsw_kick_room_player_in out, *p_out = &out;

	if (p_in->kickee_uid == p->id) {
		return send_header_to_player(p, p->waitcmd, ONLINE_ERR_CANT_KICK_SELF, 1);
	}

    p_out->kickee_uid = p_in->kickee_uid;
    p_out->kickee_role_tm = p_in->kickee_role_tm;

	return send_to_battle_switch(p, btlsw_kick_room_player_cmd, &out);
}

int btlsw_kick_room_player(DEFAULT_ARG)
{
	return send_header_to_player(p, p->waitcmd, 0, 1);
}


//------------------------------------------------------------------
// 开关房间座位
//------------------------------------------------------------------
int cli_proto_set_room_seat_onoff(DEFAULT_ARG)
{
	cli_proto_set_room_seat_onoff_in * p_in = P_IN;

    btlsw_set_room_seat_onoff_in out, *p_out = &out;

    p_out->team_id = p_in->team_id;
    p_out->seat_id = p_in->seat_id;
    p_out->onoff = p_in->onoff;

	return send_to_battle_switch(p, btlsw_set_room_seat_onoff_cmd, &out);
}

int btlsw_set_room_seat_onoff(DEFAULT_ARG)
{
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

int btlsw_notify_room_seat_onoff(DEFAULT_ARG)
{
	btlsw_notify_room_seat_onoff_out *p_in = P_IN;
	cli_proto_notify_room_seat_onoff_out out, *p_out = &out;
	p_out->team_id = p_in->team_id;
	p_out->seat_id = p_in->seat_id;
	p_out->onoff = p_in->onoff;

    return send_to_player(p, &out, cli_proto_notify_room_seat_onoff_cmd, 0);
}


//------------------------------------------------------------------
// 房间中的玩家: 设置状态(准备/取消准备), 更换座位
//------------------------------------------------------------------
int cli_proto_room_player_set_attr(DEFAULT_ARG)
{
	cli_proto_room_player_set_attr_in *p_in = P_IN;
	
	btlsw_room_player_set_attr_in out, *p_out = &out;
	p_out->which = p_in->which;
	p_out->new_team_id = p_in->new_team_id;
	p_out->new_seat_id = p_in->new_seat_id;
	p_out->new_status = p_in->new_status;
	return send_to_battle_switch(p, btlsw_room_player_set_attr_cmd, &out);
}

int btlsw_room_player_set_attr(DEFAULT_ARG)
{
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

// 换座位通知
int btlsw_player_sitdown_notify(DEFAULT_ARG)
{
	btlsw_player_sitdown_notify_out *p_in = P_IN;

	if (p_in->sitdown_uid == p->id
		&& p_in->sitdown_role_tm == (int32_t)p->role_tm) {
		// 是自己, 更新自己的team/seat
		p->room.team_id = p_in->to_team_id;
		p->room.seat_id = p_in->to_seat_id;
	}

	cli_proto_player_sitdown_notify_out out, *p_out = &out;
	p_out->sitdown_uid = p_in->sitdown_uid;
	p_out->sitdown_role_tm = p_in->sitdown_role_tm;
	p_out->from_team_id = p_in->from_team_id;
	p_out->from_seat_id = p_in->from_seat_id;
	p_out->to_team_id = p_in->to_team_id;
	p_out->to_seat_id = p_in->to_seat_id;

    return send_to_player(p, &out, cli_proto_player_sitdown_notify_cmd, 0);
}

// 切换准备状态通知
int btlsw_chg_player_status_notify(DEFAULT_ARG)
{
	btlsw_chg_player_status_notify_out *p_in = P_IN;

	cli_proto_chg_player_status_notify_out out, *p_out = &out;
	p_out->chg_uid = p_in->chg_uid;
	p_out->chg_role_tm = p_in->chg_role_tm;
	p_out->from_status = p_in->from_status;
	p_out->to_status = p_in->to_status;

    return send_to_player(p, &out, cli_proto_chg_player_status_notify_cmd, 0);
}


//------------------------------------------------------------------
// 拉取房间玩家显示信息 (装备, 宠物 之类的)
//------------------------------------------------------------------
int cli_proto_get_room_player_show_info(DEFAULT_ARG)
{
	cli_proto_get_room_player_show_info_in *p_in = P_IN;

	btlsw_get_room_player_show_info_in out, *p_out = &out;
	p_out->uid = p_in->uid;
	p_out->role_tm = p_in->role_tm;

    return send_to_battle_switch(p, btlsw_get_room_player_show_info_cmd, &out);
}

int btlsw_get_room_player_show_info(DEFAULT_ARG)
{
	btlsw_get_room_player_show_info_out *p_in = P_IN;
	cli_proto_get_room_player_show_info_out out, *p_out = &out;

	p_out->show_info = p_in->show_info;
    return send_to_player(p, p_out, p->waitcmd, 1);
}


//------------------------------------------------------------------
// 加入房间
//------------------------------------------------------------------
int cli_proto_join_room(DEFAULT_ARG)
{
	cli_proto_join_room_in *p_in = P_IN;

    btlsw_pvp_join_room_in out, *p_out = &out;

    p_out->room_id = p_in->room_id;
    memcpy(p_out->password, p_in->password, sizeof(p_in->password));

    return send_to_battle_switch(p, btlsw_pvp_join_room_cmd, p_out);
}

int btlsw_pvp_join_room(DEFAULT_ARG)
{
	btlsw_pvp_join_room_out *p_in = P_IN;
	cli_proto_join_room_out out, *p_out = &out;

	p->room.team_id = p_in->team_id;
	p->room.seat_id = p_in->seat_id;

	p_out->room_id = p_in->room_id;
	return send_to_player(p, p_out, p->waitcmd, 1);
}


//------------------------------------------------------------------
// 获取房间信息 (加入者, 房间类型 等)
//------------------------------------------------------------------
int cli_proto_get_room_full_info(DEFAULT_ARG)
{
    return send_to_battle_switch(p, btlsw_get_room_full_info_cmd, 0);
}

int btlsw_get_room_full_info(DEFAULT_ARG)
{
    btlsw_get_room_full_info_out * p_in = P_IN;
	cli_proto_get_room_full_info_out out, *p_out = &out;

	p_out->room_full_info = p_in->room_full_info;
    return send_to_player(p, p_out, p->waitcmd, 1);

}


//------------------------------------------------------------------
// 房主点击 '开始战斗' 按钮
//------------------------------------------------------------------
int cli_proto_pvp_room_start(DEFAULT_ARG)
{
    return send_to_battle_switch(p, btlsw_pvp_room_start_cmd, 0);
}

/*
 * 改变pvp房间状态为开始战斗;
 * btlsw会自动判断是否需要匹配，并根据需要进行匹配;
 * 只有匹配到了合适的房间, 才会返回应答包给房主所在的online;
 */
int btlsw_pvp_room_start(DEFAULT_ARG)
{
    btlsw_pvp_room_start_out *p_in = P_IN;

	/* TODO(zog): 这个函数没什么用, 干掉 */
    //room_start(p, p_in);

    if(!init_player_btl_grp(p)) {
        return send_error_to_player(p, ONLINE_ERR_SYSTEM_FAULT);
    }

    return btlsvr_init_battle(p, p_in->map_id, p_in->player_count,
			online_proto_create_btl_cmd);
}

/*
 * 注意: 每个房间的人都收到这个包;
 * 由房主发出，通知房间中的每个玩家，加入相应的战斗服务器，正式开始战斗
 * 调用者是 btlsvr_create_btl_callback->pvp_room_tell_players_start_battle
 */
int btlsw_pvp_room_start_battle(DEFAULT_ARG)
{
    btlsw_pvp_room_start_battle_out* p_in = P_IN;

    btlsvr_syn_player_info(p, p_in->btl_id, p_in->fd_idx);

	/* 同时离开地图: TODO(zog): 离开地图早在进入大厅的时候就应该做了 */
	if (p->check_in_map()) {
		broadcast_player_leave_map(p, p->cur_map, p->cur_map_line_index);
		leave_map(p);
	}
	return 0;
}


//------------------------------------------------------------------
// 设置房间属性 (密码, 房间名称, 副本地图id, 副本地图难度)
// 注意: 回包不能包含密码, 也不允许修改 room_type 和 room_mode
//------------------------------------------------------------------
int cli_proto_pvp_room_set_attr(DEFAULT_ARG)
{
	cli_proto_pvp_room_set_attr_in *p_in = P_IN;

	/* 对房间名做脏词检测 */
    int ret = tm_dirty_check(0, p_in->room_info.room_name);
    if (ret > 0) {
        ERROR_TLOG("dirty_room_name, u=%u", p->id);
        return send_error_to_player(p, ONLINE_ERR_DIRTY_ROOM_NAME);
    }

    btlsw_pvp_room_set_attr_in out, *p_out = &out;
	p_out->room_info = p_in->room_info;

	return send_to_battle_switch(p, btlsw_pvp_room_set_attr_cmd, &out);
}

int btlsw_pvp_room_set_attr(DEFAULT_ARG)
{
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

int btlsw_pvp_room_attr_notify(DEFAULT_ARG)
{
	btlsw_pvp_room_attr_notify_out *p_in = P_IN;

	pvp_room_update_attr(&(p->room), &(p_in->room_info));

	cli_proto_pvp_room_attr_notify_out out, *p_out = &out;
	p_out->room_info = p_in->room_info;

	TRACE_TLOG("btlsw_pvp_room_attr_notify: in:[%s|%s], out:[%s|%s]",
			p_in->room_info.room_name, p_in->room_info.password,
			p_out->room_info.room_name, p_out->room_info.password);

	return send_to_player(p, p_out, cli_proto_pvp_room_attr_notify_cmd, 0);
}


//------------------------------------------------------------------
// btlsw 回发房间聊天消息
//------------------------------------------------------------------
int btlsw_chat_msg_transfer(DEFAULT_ARG)
{
	btlsw_chat_msg_transfer_out *p_in = P_IN;

	uint64_t key = gen_key_by_uid_role(p_in->sender.user_id, p_in->sender.role_tm);
	if (p->blacklist.find(key) != p->blacklist.end()) {
		return 0;//如果发送者在我黑名单中，则我不收该消息
	}
	return send_to_player(p, p_in, cli_proto_chat_public_msg_cmd, 0);
}
