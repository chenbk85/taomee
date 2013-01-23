#include "team.hpp"
#include "room.hpp"


uint32_t room_id_generator;

/* TODO(zog): transcript 用配置默认;
 * 注意: 对name的脏词检测在online就做过了;
 */
room_t *alloc_room(online_t *online, uint8_t type, uint8_t mode,
		const char *name, int namelen, const char *pass, int passlen)
{
	uint32_t new_roomid = get_next_room_id();
	/* 确保没有重复的 roomid */
	room_t *room = find_room(online, new_roomid);
	if (unlikely(room)) {
		WARN_TLOG("BUG: dup_new_roomid: %u", new_roomid);
		return 0;
	}

	room = reinterpret_cast<room_t *>(g_slice_alloc0(sizeof(room_t)));
	if (unlikely(!room)) {
		WARN_TLOG("ISSUE: nomem_for_room: %u", new_roomid);
		return 0;
	}

	room->id = new_roomid;
	room->create_time = get_now_tv()->tv_sec;
	memcpy(room->name, name, MIN(namelen, int(sizeof(room->name))));
	memcpy(room->pass, pass, MIN(passlen, int(sizeof(room->pass))));
	room->type = type;
	room->mode = mode;
	room->status = room_status_idle;
	room->team_map = new team_map_t();
	room->online = online;
	//room->transcript = transcript; // TODO(zog): 副本在什么时候与 room 挂上钩呢? 相关业务流程写到的时候自然会清楚;

	DEBUG_TLOG("alloc_room, olid=%u, roomid=%u, type=%hhu, mode=%hhu",
			online->id, room->id, type, mode);
	return room;
}

/* 添加 room (初始化: 直接关系链 + 下级关系链 + 自身数据;) */
room_t *add_room(online_t *online, player_t *owner, uint8_t type, uint8_t mode,
		const char *name, int namelen, const char *pass, int passlen)
{
	/* 下级关系链 + 自身数据 */
	room_t *room = alloc_room(online, type, mode, name, namelen, pass, passlen);
	if (unlikely(!room)) {
		ERROR_TLOG("failed to alloc_room_when_add_room, olip=0x%X, olid=%u",
				online->fdsess->remote_ip, online->id);
		return 0;
	}
	/* 随后要立刻建立下级设施: team 和 seat(在add_team中被创建) */
	team_t *blue_team = add_team(room, team_no_blue, team_max_seat[type]);
	if (unlikely(!blue_team)) {
		ERROR_TLOG("failed_to_add_blue_team_when_add_room, olip=0x%X, olid=%u",
				online->fdsess->remote_ip, online->id);

		TRACELOG_PRE_DEL_ROOM(room->id, room);
		del_room(room);
		return 0;
	}

	/* 根据房间对战模式决定是否需要红方 */
	if (is_two_side_room_mode(room->mode)) {
		team_t *red_team = add_team(room, team_no_red, team_max_seat[type]);
		if (unlikely(!red_team)) {
			ERROR_TLOG("failed_to_add_red_team_when_add_room, olip=0x%X, olid=%u",
					online->fdsess->remote_ip, online->id);

			TRACELOG_PRE_DEL_ROOM(room->id, room);
			del_room(room);
			return 0;
		}
	}

	/* 直接关系链 */
	room_map_t *room_map = get_room_map(online, type, room_status_idle);
	reg_room(room_map, room);

	/* 间接关系链: 设定 owner, 并把 owner 坐在第一个seat */
	room->owner = owner;
	bool sit_ret = player_sit_down(owner, room, team_no_blue, 0);
	if (unlikely(!sit_ret)) {
		ERROR_TLOG("failed_to_sit_down_when_add_room, olip=0x%X, olid=%u, u=%u",
				online->fdsess->remote_ip, online->id, owner->id);

		TRACELOG_PRE_DEL_ROOM(room->id, room);
		del_room(room);
		return 0;	
	}

	/* 关联数据 (TODO(zog): eg: 副本地图配置) */

	DEBUG_TLOG("add_room, olid=%u, roomid=%u, type=%hhu, mode=%hhu, owner=%u",
			online->id, room->id, type, mode, owner->id);
	return room;
}

bool close_room(room_t *room)
{
	online_t *online = room->online;
	if (room->status == room_status_closed) return true;

	/* 直接关系链: 迁移到 closed_map 中 */
	room_map_t *from_room_map = get_room_map(online, room->type, room->status);
	if (unlikely(!from_room_map)) return false;
	from_room_map->erase(room->id);
	online->closed_map->insert(make_pair(room->id, room));

	/* 修改状态 */
	room->status = room_status_closed;

	/* 去掉 owner */
	room->owner = 0;

	/* TODO(zog): btlgrp/transcript 怎么处理, 或许应该放到 del_room 里处理 ??? */

	return true;
}

void del_room(room_t *room)
{
	/* 下级关系链 */
	team_t *team = 0;
	team_map_iter_t iter = room->team_map->begin();
	while (iter != room->team_map->end()) {
		team = iter->second;
		iter++;
		del_team(team);
	}
	room->team_map->clear();
	delete room->team_map;
	room->team_map = 0;

	/* 直接关系链 */
	unreg_room(room);

	DEBUG_TLOG("del_room, room_id=%u", room->id);

	/* 自身数据 */
	dealloc_room(room);
}

/* return: true: 房间空了, false: 房间不空 */
bool is_room_empty(room_t *room)
{
	team_t *team = 0;
	team_map_iter_t iter = room->team_map->begin();
	for (; iter != room->team_map->end(); iter++) {
		team = iter->second;
		if (!is_team_empty(team)) {
			return false;
		}
	}

	return true;
}

/*
 * @brief 返回 room 有多少座位;
 * @team 特指哪一队的座位: 0xFF: all, 0: 蓝队, 1: 红队, 其它值: 无效, 返回0;
 * return: room 中的 seat 数量;
 */
int count_room_seats(room_t *room, uint8_t team_no)
{
	if (team_no != 0xFF) { // 放过 0xFF, 表示取所有 team 的位子数量;
		if (unlikely(!is_valid_team_no(team_no))) {
			return 0;
		}
	}

	int count = 0;
	team_map_iter_t iter = room->team_map->begin();
	for (; iter != room->team_map->end(); iter++) {
		team_t *team = iter->second;
		if (team_no == team->id) {
			return team->seat_map->size();
		}
		count += team->seat_map->size();
	}

	return count;
}

int count_room_unlock_seats(room_t *room, uint8_t team_no)
{
	if (team_no != 0xFF) { // 放过 0xFF, 表示取所有 team 的位子数量;
		if (unlikely(!is_valid_team_no(team_no))) {
			return 0;
		}
	}

	int count = 0;
	team_map_iter_t iter = room->team_map->begin();
	for (; iter != room->team_map->end(); iter++) {
		team_t *team = iter->second;
		seat_map_iter_t iter2 = team->seat_map->begin();
		for (; iter2 != team->seat_map->end(); iter2++) {
			seat_t *seat = iter2->second;
			if (seat->lock == seat_lock_on) continue;
			++count;
		}
	}

	return count;
}

void notify_room_player(room_t *room, player_t *notifier, uint16_t cmd,
		Cmessage *p_out, bool skip_notifier, bool complete)
{
	team_t *team = 0;
	team_map_iter_t iter = room->team_map->begin();
	for (; iter != room->team_map->end(); iter++) {
		team = iter->second;
		notify_team_player(team, notifier, cmd, p_out, skip_notifier, false);
	}

	if (complete) {
		notifier->waitcmd = 0;
	}
}

player_t *gen_new_room_owner(room_t *room, player_t *skip_p)
{
	player_t *new_owner = 0;
	team_map_iter_t iter = room->team_map->begin();
	for (; iter != room->team_map->end(); iter++) {
		team_t *team = iter->second;
		seat_map_iter_t iter2 = team->seat_map->begin();
		for (; iter2 != team->seat_map->end(); iter2++) {
			seat_t *seat = iter2->second;
			if (!seat->player) continue;
			if (skip_p && seat->player == skip_p) continue;
			new_owner = seat->player;
			break;
		}
	}

	return new_owner;
}

void proc_closed_rooms(void)
{
	room_t *room = 0;
	online_t *online = 0;
	online_fd_map_iter_t iter = online_fd_map.begin();

	int count = 0;
	for (; iter != online_fd_map.end(); iter++) {
		online = iter->second;
		room_map_iter_t closed_room_iter = online->closed_map->begin();
		while (closed_room_iter != online->closed_map->end()) {
			room = closed_room_iter->second;
			closed_room_iter++;

			/* 检查一下是否有BUG */
			if (unlikely(room->status != room_status_closed)) {
				WARN_TLOG("BUG: closed_room_not_closed_status, "
						"roomid=%u, type=%hhu, mode=%hhu",
						room->id, room->type, room->mode);
			}
			if (unlikely(room->owner)) {
				WARN_TLOG("BUG: closed_room_has_owner, "
						"roomid=%u, type=%hhu, mode=%hhu, owner=%u",
						room->id, room->type, room->mode, room->owner->id);
			}

			TRACELOG_PRE_DEL_ROOM(closed_room_iter->first, room);
			del_room(room);
			++count;
			if (count >= MAX_BATCH_PROC_CLOSED_ROOM) {
				WARN_TLOG("ISSUE: too_many_closed_room, olid=%u", online->id);
				break;
			}

		}
	}
}
