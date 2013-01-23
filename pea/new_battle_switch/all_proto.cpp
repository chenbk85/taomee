#include "all_proto.hpp"



//------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------
void pack_pvp_room_list_info(room_t *room, room_list_info_t *room_list_info)
{
	room_list_info->init();

	room_list_info->room_id = room->id;
	room_list_info->map_id = room->transcript ? room->transcript->mapid : 0;
	memcpy(room_list_info->room_name, room->name, sizeof(room_list_info->room_name));
	room_list_info->room_type= room->type; // pvp / pve
	room_list_info->room_mode = room->mode; // free / compete
	room_list_info->encrypted_flag = (room->pass[0] == '\0') ? 0 : 1; // 0: 没加密, 1: 加密
	room_list_info->room_status = room->status; // 不会有 fighting 的;
	room_list_info->player_count = count_room_players(room);
	room_list_info->max_count = count_room_unlock_seats(room);
}

uint8_t get_room_list_info_turn_lower(room_map_t *room_map, uint8_t mode,
		uint32_t bound_roomid, uint8_t nr, btlsw_pvp_room_list_out *p_out)
{
	room_t *room = 0;
	room_list_info_t room_list_info;
	room_map_iter_t iter = room_map->lower_bound(bound_roomid);
	if (iter == room_map->end()) {
		return nr;
	}

	int loops = 0;
	room_map_riter_t riter(iter);
	for (; riter != room_map->rend(); riter++) {
		room = riter->second;
		if (room->id == bound_roomid) continue;
		if (mode != 0xFF && mode != room->mode) continue;
		pack_pvp_room_list_info(room, &room_list_info);
		p_out->room_list.push_back(room_list_info);
		--nr;
		if (nr == 0) {
			break;
		}
		if (loops++ >= MAX_ROOM_LIST_BATCH_LOOPS) {
			break;
		}
	}

	return nr;
}

uint8_t get_room_list_info_turn_upper(room_map_t *room_map, uint8_t mode,
		uint32_t bound_roomid, uint8_t nr, btlsw_pvp_room_list_out *p_out)
{
	room_t *room = 0;
    room_list_info_t room_list_info;
	room_map_iter_t iter = room_map->upper_bound(bound_roomid);

	int loops = 0;
	for (; iter != room_map->end(); iter++) {
		room = iter->second;
		if (room->id == bound_roomid) continue;
		if (mode != 0xFF && mode != room->mode) continue;
		pack_pvp_room_list_info(room, &room_list_info);
		p_out->room_list.push_back(room_list_info);
		--nr;
		if (nr == 0) {
			break;
		}
		if (loops++ >= MAX_ROOM_LIST_BATCH_LOOPS) {
			break;
		}
	}

	return nr;
}

void init_player_show_info(player_t *p, room_player_show_info_t *info)
{
	memcpy(p->nick, info->nick, sizeof(p->nick));
	p->nick[sizeof(p->nick) - 1] = '\0';

	player_show_info_t *pi = &(p->show_info);
	pmodel_info_t *pmodel_info = &(pi->model_info);
	proom_pet_info_t *ppet_info = &(pi->pet_info);
	psimple_equip_info_t *pequip_info = pi->equip_info;

	model_info_t *model_info = &(info->model_info);
	room_pet_info_t *pet_info = &(info->pet_info);


	/* model_info */
	pmodel_info->eye_model = model_info->eye_model;
	pmodel_info->resource_id = model_info->resource_id;

	/* pet_info */
	ppet_info->pet_id = pet_info->pet_id;
	ppet_info->pet_level = pet_info->pet_level;
	ppet_info->skill_1 = pet_info->skill_1;
	ppet_info->skill_2 = pet_info->skill_2;
	ppet_info->skill_3 = pet_info->skill_3;
	ppet_info->uni_skill = pet_info->uni_skill;

	/* equip_info */
	for (uint32_t i = 0; i < MAX_EQUIP_ON_PLAYER && i < info->equip_info.size(); i++) {
		pequip_info[i].equip_id = info->equip_info[i].equip_id;
	}
}

void pack_room_player_show_info(player_t *p, room_player_show_info_t *info)
{
	info->uid = p->id;
	info->role_tm = p->role_tm;
	info->team_id = p->seat ? p->seat->team->id : 0;
	info->seat_id = p->seat ? p->seat->id : 0;;
	info->player_status = p->status;
	memcpy(info->nick, p->nick, MAX_NICK_SIZE);
	info->model_info.eye_model = p->show_info.model_info.eye_model;
	info->model_info.resource_id = p->show_info.model_info.resource_id;

	proom_pet_info_t *fight_pet = &(p->show_info.pet_info);
	info->pet_info.pet_id = fight_pet->pet_id;
	info->pet_info.pet_level = fight_pet->pet_level;
	info->pet_info.skill_1 = fight_pet->skill_1;
	info->pet_info.skill_2 = fight_pet->skill_2;
	info->pet_info.skill_3 = fight_pet->skill_3;
	info->pet_info.uni_skill = fight_pet->uni_skill;

	for (int i = 0; i < MAX_EQUIP_ON_PLAYER; i++) {
		if (p->show_info.equip_info[i].equip_id == 0) continue;
		simple_equip_info_t e;
		e.equip_id = p->show_info.equip_info[i].equip_id;
		info->equip_info.push_back(e);
	}
}

void pvp_room_update_attr(room_t *room, room_info_t *info)
{
	info->room_name[sizeof(info->room_name)-1] = '\0';
	info->password[sizeof(info->password)-1] = '\0';

	memcpy(room->name, info->room_name, sizeof(room->name));
	memcpy(room->pass, info->password, sizeof(room->pass));
	room->mapid = info->map_id; // TODO(zog): transcript 弄好后, 这些信息用来切换副本;
	room->maplv = info->map_lv; // ditto
}


//------------------------------------------------------------------
// proto_XXX functions
// DEFAULT_ARG = player_t * p, Cmessage * c_in, Cmessage * c_out, void * param
//------------------------------------------------------------------
//------------------------------------------------------------------
// 房间聊天中转
//------------------------------------------------------------------
int btlsw_chat_msg_transfer(DEFAULT_ARG)
{
	btlsw_chat_msg_transfer_in *p_in = P_IN;

	PLAYER_MUST_INROOM(p);
	room_t *room = p->seat->team->room;
	notify_room_player(room, p, btlsw_chat_msg_transfer_cmd, p_in, DONT_SKIP_NOTIFIER, true);

    TRACE_TLOG("send_msg_to_room, u=%u, roomid=%u", p->id, room->id);
	return 0;
}

//------------------------------------------------------------------
// 进入大厅 (pvp/pve)
//------------------------------------------------------------------
int btlsw_player_enter_hall(DEFAULT_ARG)
{
	btlsw_player_enter_hall_in *p_in = P_IN;
	
	init_player_show_info(p, &(p_in->player_info));
	DEBUG_LOG("player_enter_hall, u=%u, role_tm=%u, olid=%u",
			p->id, p->role_tm, p->online->id);

	return send_header_to_player(p, BTLSW_ERR_NOERROR);
}

//------------------------------------------------------------------
// 离开大厅 (pvp/pve)
//------------------------------------------------------------------
int btlsw_player_exit_hall(DEFAULT_ARG)
{
	DEBUG_TLOG("player_exit_hall, u=%u, role_tm=%u, olid=%u",
			p->id, p->role_tm, p->online->id);
	send_header_to_player(p, BTLSW_ERR_NOERROR);

	del_player(p);
	return 0;
}

//------------------------------------------------------------------
// 拉取pvp房间列表 (pvp)
//------------------------------------------------------------------
int btlsw_pvp_room_list(DEFAULT_ARG)
{
	btlsw_pvp_room_list_in *p_in = P_IN;
    btlsw_pvp_room_list_out out, *p_out = &out;

	uint8_t mode = p_in->room_mode;
	uint8_t count = p_in->count;
	uint8_t turn = p_in->turn;
	uint32_t bound_roomid = p_in->bound_roomid;


	if (mode != 0xFF && !is_valid_room_mode(mode)) {
		return send_header_to_player(p, BTLSW_ERR_INV_ROOM_MODE);
	}

	if (turn != 0 && turn != 1) {
		return send_header_to_player(p, BTLSW_ERR_INV_ROOM_LIST_TURN);
	}
	if (count == 0 || count > MAX_BATCH_ROOM_LIST_NUM) {
		return send_header_to_player(p, BTLSW_ERR_INV_ROOM_LIST_COUNT);
	}
	uint8_t left = count;

	/* 同服, ID从小到大, 仅pvp-玩家自己配对房间, 未对战房间, 数量: ??? */
	if (turn == 0) { // 向前翻
		left = get_room_list_info_turn_lower(p->online->idle_map, mode, bound_roomid, left, p_out);
		if (left > 0) { // idle 还不够的话, 就找 asking 的;
			get_room_list_info_turn_lower(p->online->asking_map, mode, bound_roomid, left, p_out);
		}
	} else { // 向后翻
		left = get_room_list_info_turn_upper(p->online->idle_map, mode, bound_roomid, left, p_out);
		if (left > 0) { // idle 还不够的话, 就找 asking 的;
			get_room_list_info_turn_upper(p->online->asking_map, mode, bound_roomid, left, p_out);
		}
	}

	return send_to_player(p, p_out);
}

//------------------------------------------------------------------
// 建立pvp房间 (pvp)
//------------------------------------------------------------------
int btlsw_pvp_create_room(DEFAULT_ARG)
{
    if (is_player_in_room(p)) {
        ERROR_LOG("fail_pvp_create_room: already_in_room, u=%u, roomid=%u",
				p->id, p->seat->team->room->id);
		return send_header_to_player(p, BTLSW_ERR_ALREADY_INROOM);
    }

	btlsw_pvp_create_room_in *p_in = P_IN;

    char *room_name = p_in->room_name;
	room_name[sizeof(p_in->room_name)-1] = '\0';
    char *passwd = p_in->password;
	passwd[sizeof(p_in->password)-1] = '\0';
    uint8_t mode = p_in->room_mode;
    uint32_t mapid = p_in->map_id;
    uint8_t maplv = p_in->map_lv;

	if (!is_valid_room_mode(mode)) {
		return send_header_to_player(p, BTLSW_ERR_INV_ROOM_MODE);
	}

	if (!is_valid_pvp_mapid(mapid)) {
		return send_header_to_player(p, BTLSW_ERR_INV_PVP_MAPID);
	}

	online_t *online = p->online;
	room_t *room = add_room(online, p, room_type_pvp, mode,
			room_name, MAX_ROOM_NAME_LEN, passwd, MAX_ROOM_PWD_LEN);
	if (unlikely(!room)) {
		return send_header_to_player(p, BTLSW_ERR_SYSTEM_FAULT);
	}
	assert(p->seat);
	assert(p->seat->team);
	assert(p->seat->team->room);

#ifdef ENABLE_TRACE_LOG
	tracelog_list_online_rooms(online, __FILE__, __LINE__);
	tracelog_list_room_players(room, __FILE__, __LINE__);
#endif

	btlsw_pvp_create_room_out out, *p_out = &out;

	/* misc_info */
	p_out->team_id = p->seat->team->id;
	p_out->seat_id = p->seat->id;

	/* room_info */
/* TODO(zog): transcript完成后 删除这个 #if */
#if 1
	room->mapid = mapid;
	room->maplv = maplv;
#endif
    p_out->room_info.room_id = room->id;
    memcpy(p_out->room_info.room_name, room_name, MAX_ROOM_NAME_LEN);
    memcpy(p_out->room_info.password, passwd, MAX_ROOM_PWD_LEN);
    p_out->room_info.room_type = room_type_pvp;
    p_out->room_info.room_mode = mode;
    p_out->room_info.map_id = mapid; // TODO(zog): 副本地图配置
    p_out->room_info.map_lv = maplv; // TODO(zog): 副本地图配置

	/* pet_info */
	proom_pet_info_t *pet_info = &(p->show_info.pet_info);
    p_out->pet_info.pet_id = pet_info->pet_id;
    p_out->pet_info.pet_level = pet_info->pet_level;
    p_out->pet_info.skill_1 = pet_info->skill_1;
    p_out->pet_info.skill_2 = pet_info->skill_2;
    p_out->pet_info.skill_3 = pet_info->skill_3;
    p_out->pet_info.uni_skill = pet_info->uni_skill;

    DEBUG_TLOG("pvp_create_room, u=%u, olid=%u, roomid=%u, mode=%u, name=[%s], "
			"pass=[%s]", p->id, online->id, room->id, mode, room_name, passwd);
    return send_to_player(p, p_out);
}

//------------------------------------------------------------------
// 玩家离开房间 (pvp/pve)
//------------------------------------------------------------------
int btlsw_leave_room(DEFAULT_ARG)
{
	if (!is_player_in_room(p)) {
		return send_header_to_player(p, BTLSW_ERR_NOT_INROOM);
	}

	player_leave_seat(p);

	return send_header_to_player(p, BTLSW_ERR_NOERROR);
}

//------------------------------------------------------------------
// 按房间编号搜索房间信息 (包括fighting列表的房间)
//------------------------------------------------------------------
int btlsw_pvp_query_room_info(DEFAULT_ARG)
{
	btlsw_pvp_query_room_info_in *p_in = P_IN;
	uint32_t room_id = p_in->room_id;

	room_t *room = find_room(p->online, room_id);
	if (!room) {
		return send_header_to_player(p, BTLSW_ERR_NO_ROOM_FOUND);
	}

	btlsw_pvp_query_room_info_out out;
	pack_pvp_room_list_info(room, &(out.room_info));
    return send_to_player(p, &out);
}

//------------------------------------------------------------------
// 踢人 (pvp/pve)
//------------------------------------------------------------------
int btlsw_kick_room_player(DEFAULT_ARG)
{
    btlsw_kick_room_player_in *p_in = P_IN;

	uint32_t kickee_uid = p_in->kickee_uid;
	int32_t kickee_role_tm = p_in->kickee_role_tm;

	/* 防止自宫 */
	if (kickee_uid == p->id) { // 注意: 暂时不对 role_tm 做检查
		return send_header_to_player(p, BTLSW_ERR_CANT_KICK_SELF);
	}
	
	/* 检查 p(kicker) 是不是房主 */
	if (!is_player_in_room(p)) {
		return send_header_to_player(p, BTLSW_ERR_NOT_INROOM);
	}
	room_t *room = p->seat->team->room;

	/* 对战中, 不允许踢人 */
	ROOM_MUST_NOT_IN_BATTLE(room);

	if (!is_room_owner(room, p)) {
		return send_header_to_player(p, BTLSW_ERR_NOT_ROOM_OWNER);
	}

	/* 踢人 */
	player_t *kickee = get_player(kickee_uid, kickee_role_tm);
	if (!kickee
		|| !is_player_in_room(kickee)
		|| kickee->seat->team->room != room) {
		return send_header_to_player(p, BTLSW_ERR_NOPLAYER_INROOM);
	}
	player_leave_seat(kickee, true);

	return send_header_to_player(p, BTLSW_ERR_NOERROR);
}

//------------------------------------------------------------------
// 开关房间座位
//------------------------------------------------------------------
int btlsw_set_room_seat_onoff(DEFAULT_ARG)
{
	btlsw_set_room_seat_onoff_in *p_in = P_IN;
	uint8_t team_id = p_in->team_id;
	uint8_t seat_id = p_in->seat_id;
	uint8_t onoff = p_in->onoff;

	if (onoff != 0 && onoff != 1) {
		return send_header_to_player(p, BTLSW_ERR_INV_SEAT_ONOFF);
	}

	PLAYER_MUST_INROOM(p);
	room_t *room = p->seat->team->room;

	PLAYER_MUST_ROOM_OWNER(room, p);
	team_t *team = find_team(room, team_id);
	if (!team) {
		return send_header_to_player(p, BTLSW_ERR_INV_TEAM_ID);
	}
	seat_t *seat = find_seat(team, seat_id);
	if (!seat) {
		return send_header_to_player(p, BTLSW_ERR_INV_SEAT_ID);
	}
	SEAT_MUST_EMPTY(seat);

	set_seat_onoff(seat, onoff);

	btlsw_notify_room_seat_onoff_out out;
	out.team_id = team_id;
	out.seat_id = seat_id;
	out.onoff = seat->lock;
	notify_room_player(room, p, btlsw_notify_room_seat_onoff_cmd, &out);

	return send_header_to_player(p, BTLSW_ERR_NOERROR);
}

//------------------------------------------------------------------
// 房间中的玩家: 设置状态(准备/取消准备), 更换座位
//------------------------------------------------------------------
int btlsw_room_player_set_attr(DEFAULT_ARG)
{
	btlsw_room_player_set_attr_in *p_in = P_IN;
	uint8_t which = p_in->which;
	uint8_t new_team_id = p_in->new_team_id;
	uint8_t new_seat_id = p_in->new_seat_id;
	uint8_t new_status = p_in->new_status;

	if (which != 0 && which != 1) {
		return send_header_to_player(p, BTLSW_ERR_INV_PLAYER_SET_WHICH);
	}

	PLAYER_MUST_INROOM(p);
	room_t *room = p->seat->team->room;

	team_t *team = 0;
	seat_t *seat = 0;
	if (which == 0) { // 换位子
		goto chg_seat;
	} else { // 准备状态变更
		goto chg_player_status;
	}

chg_seat:
	team = find_team(room, new_team_id);
	if (!team) {
		return send_header_to_player(p, BTLSW_ERR_INV_TEAM_ID);
	}
	seat = find_seat(team, new_seat_id);
	if (!seat) {
		return send_header_to_player(p, BTLSW_ERR_INV_SEAT_ID);
	}
	SEAT_MUST_EMPTY(seat); // 同时保证了不会切换到自己位子上

	player_sit_down(p, seat);

	return send_header_to_player(p, BTLSW_ERR_NOERROR);

chg_player_status:
	if ((new_status != player_status_idle)
		&& (new_status != player_status_ready)) {
		return send_header_to_player(p, BTLSW_ERR_INV_PLAYER_STATUS);
	}
	if (p->status == new_status) {
		return send_header_to_player(p, BTLSW_ERR_NOERROR);
	}

	chg_room_player_status(p, new_status);

	return send_header_to_player(p, BTLSW_ERR_NOERROR);
}

//------------------------------------------------------------------
// 获取房间玩家显示信息 (装备, 宠物 之类的)
//------------------------------------------------------------------
int btlsw_get_room_player_show_info(DEFAULT_ARG)
{
	btlsw_get_room_player_show_info_in *p_in = P_IN;

	uint32_t uid = p_in->uid;
	int32_t role_tm = p_in->role_tm;

	player_t *player = p;
	if (uid != p->id || role_tm != p->role_tm) {
		player = get_player(uid, role_tm);
	}

	if (!player) {
		return send_header_to_player(p, BTLSW_ERR_NOPLAYER_INROOM);
	}

	btlsw_get_room_player_show_info_out out, *p_out = &out;
	room_player_show_info_t *info = &(p_out->show_info);
	pack_room_player_show_info(player, info);

    return send_to_player(p, &out);
}

//------------------------------------------------------------------
// 玩家加入pvp房间
//------------------------------------------------------------------
int btlsw_pvp_join_room(DEFAULT_ARG)
{
	btlsw_pvp_join_room_in *p_in = P_IN;
    uint32_t room_id = p_in->room_id;
    char passwd[MAX_ROOM_PWD_LEN] = { 0 };
	snprintf(passwd, sizeof(passwd), "%s", p_in->password);

	PLAYER_MUST_NOT_INROOM(p); // 同时排除进入相同的房间

	online_t *online = p->online;
	/* 找不到要进入的房间 (撤销了) */
	room_t *room = find_room(online, room_id);
	if (!room) {
		return send_header_to_player(p, BTLSW_ERR_NO_ROOM_FOUND);
	}

	/* 密码不对 */
	if (room->pass[0] != '\0'
		&& strncmp(passwd, room->pass, sizeof(passwd))) {
		return send_header_to_player(p, BTLSW_ERR_PASSWORD);
	}

	/* 房间满 */
	seat_t *blue_seat = find_room_first_empty_seat(room, team_no_blue);
	seat_t *red_seat = find_room_first_empty_seat(room, team_no_red);
	seat_t *select_seat = 0;
	if (!blue_seat && !red_seat) {
		return send_header_to_player(p, BTLSW_ERR_ROOM_FULL);
	} else if (blue_seat && !red_seat) {
		select_seat = blue_seat;
	} else if (!blue_seat && red_seat) {
		select_seat = red_seat;
	} else {
		int blue_seat_count = count_room_players(room, 0, team_no_blue);
		int red_seat_count = count_room_players(room, 0, team_no_red);
		if (blue_seat_count <= red_seat_count) {
			select_seat = blue_seat;
		} else {
			select_seat = red_seat;
		}
	}
	player_sit_down(p, select_seat);

	/* 过滤条件都通过了, 坐下 */
	btlsw_pvp_join_room_out out, *p_out = &out;
	p_out->room_id = room->id;
	p_out->team_id = select_seat->team->id;
	p_out->seat_id = select_seat->id;

#ifdef ENABLE_TRACE_LOG
	tracelog_list_online_rooms(online, __FILE__, __LINE__);
	tracelog_list_room_players(room, __FILE__, __LINE__);
#endif

	DEBUG_TLOG("player_join_room, u=%u, role_tm=%d, "
			"room_id=%u, team=%hhu, seat=%hhu, owner=%u", p->id, p->role_tm,
			room->id, select_seat->team->id, select_seat->id, room->owner->id);
	return send_to_player(p, p_out);
}

//------------------------------------------------------------------
// 拉取房间内信息
//------------------------------------------------------------------
int btlsw_get_room_full_info(DEFAULT_ARG)
{
	btlsw_get_room_full_info_out out, *p_out = &out;

	PLAYER_MUST_INROOM(p);
	room_t *room = p->seat->team->room;

	room_full_info_t *info = &(p_out->room_full_info);

	/* owner */
	info->owner_uid = room->owner->id;
	info->owner_role_tm = room->owner->role_tm;

	/* room_info */
	room_info_t *room_info = &(info->room_info);
    room_info->room_id = room->id;
    memcpy(room_info->room_name, room->name, MAX_ROOM_NAME_LEN);
	/* 注意: 不要给 passwd, 否则密码就无效了 */
    room_info->room_type = room->type;
    room_info->room_mode = room->mode;
    room_info->map_id = room->maplv; // TODO(zog): 副本地图配置, 用 transcript 代替
    room_info->map_lv = room->mapid; // TODO(zog): 副本地图配置, 用 transcript 代替

	/* player_show_info */
	room_player_show_info_t player_info_st, *player_info = &player_info_st;
	team_map_iter_t iter = room->team_map->begin();
	for (; iter != room->team_map->end(); iter++) {
		team_t *team = iter->second;
		seat_map_iter_t iter2 = team->seat_map->begin();
		for (; iter2 != team->seat_map->end(); iter2++) {
			seat_t *seat = iter2->second;
			if (!seat->player) continue;
			player_info->init();
			pack_room_player_show_info(seat->player, player_info);
			info->player_info.push_back(player_info_st);
		}
	}

	return send_to_player(p, p_out);
}


//------------------------------------------------------------------
// 修改房间属性 (房间名,密码, 副本地图id, 副本难度)
//------------------------------------------------------------------
int btlsw_pvp_room_set_attr(DEFAULT_ARG)
{
	btlsw_pvp_room_set_attr_in *p_in = P_IN;

	PLAYER_MUST_INROOM(p);
	room_t *room = p->seat->team->room;

	PLAYER_MUST_ROOM_OWNER(room, p);

	/* 允许修改的房间属性: room_name, password, map_id, map_lv */
	pvp_room_update_attr(room, &(p_in->room_info));

	btlsw_pvp_room_attr_notify_out out, *p_out = &out;
	p_out->room_info = p_in->room_info;
	notify_room_player(room, p, btlsw_pvp_room_attr_notify_cmd, p_out, DONT_SKIP_NOTIFIER, false);

	return send_header_to_player(p, BTLSW_ERR_NOERROR);
}


//------------------------------------------------------------------
// 房主点 "开始对战": 通知房间中其它玩家，加入相应的战斗服务器
//------------------------------------------------------------------
int btlsw_pvp_room_start(DEFAULT_ARG)
{
    btlsw_pvp_room_start_out out, *p_out = &out;

	PLAYER_MUST_INROOM(p);
	room_t *room = p->seat->team->room;

	p_out->map_id = room->mapid; // TODO: 临时数据, 后改成从 transcript 中读;
	p_out->player_count = count_room_players(room);

	return send_to_player(p, p_out);
}

/* 修改所在房间的状态到对战状态, 登记对战参数(fd, btlid,...), 通知其它房间成员加入对战 */
int btlsw_pvp_room_start_battle(DEFAULT_ARG)
{
    btlsw_pvp_room_start_battle_in * p_in = P_IN;


	PLAYER_MUST_INROOM(p);
	room_t *room = p->seat->team->room;

	PLAYER_MUST_ROOM_OWNER(room, p);

	/* TODO(zog): 这里要根据对战模式判断是否:
	 * (1) 需要两边都准备好(有两边-自由模式);
	 * (2) 还是需要svr帮忙配对(仅有一边, 竞技模式);
	 *
	 * 最好还是分成另外一个协议;
	 */

	/* 全都要准备好才能开始 */
	team_map_iter_t iter = room->team_map->begin();
	for (; iter != room->team_map->end(); iter++) {
		team_t *team = iter->second;
		int count = count_room_players(room, 0, team->id);
		if (count == 0) {
			/* 发现有一边一个人都没有, 此时哪怕另一边全部准备好了, 也不能开始对战 */
			return send_header_to_player(p, BTLSW_ERR_SOMETEAM_UNREADY);
		}
		seat_map_iter_t iter2 = team->seat_map->begin();
		for (; iter2 != team->seat_map->end(); iter2++) {
			seat_t *seat = iter2->second;
			if (!seat->player) continue;
			if (seat->player != p
				&& seat->player->status != player_status_ready) {
				return send_header_to_player(p, BTLSW_ERR_SOMEONE_UNREADY);
			}
		}
	}

	/* room 的状态变化 */
	if (!pvp_room_switch_status(room, room_status_fighting)) {
		return send_header_to_player(p, BTLSW_ERR_ROOM_CANT_IN_FIGHTING);
	}

    btlsw_pvp_room_start_battle_out out, *p_out = &out;
	p_out->btl_id = p_in->btl_id;
	p_out->fd_idx = p_in->fd_idx;

#ifdef ENABLE_TRACE_LOG
	tracelog_list_room_players(room, __FILE__, __LINE__);
#endif

	DEBUG_TLOG("notify_room_start_battle, u=%u, role_tm=%d, room_id=%u, owner=%u",
			p->id, p->role_tm, room->id, room->owner->id);
	notify_room_player(room, p, btlsw_pvp_room_start_battle_cmd, &out, DONT_SKIP_NOTIFIER, true);
	return 0;
}

//------------------------------------------------------------------
// 战斗结束
//------------------------------------------------------------------
int btlsw_pvp_end_battle(DEFAULT_ARG)
{
	return 0;
}
