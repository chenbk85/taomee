#ifndef __ROOM_HPP__
#define __ROOM_HPP__


#include "btlsw_debug.hpp"
#include "team.hpp"
#include "common.hpp"


/* 一次最多拉取 32 个房间信息 */
#define MAX_BATCH_ROOM_LIST_NUM		(32)

/* 每次 onidle 清理的房间数有个限度, 保证清理出足够的内存, 又不占用太多时间;
 * QA(zog): 这个数量合理吗? */
#define MAX_BATCH_PROC_CLOSED_ROOM	(400)

#ifdef ENABLE_TRACE_LOG
#define TRACELOG_PRE_DEL_ROOM(_roomid, _room) \
	do { \
		TRACE_TLOG("pre_del_room(%s:%d): roomid=%u, room=%p", \
				__FILE__, __LINE__, _roomid, _room); \
	} while (0)
#else
#define TRACELOG_PRE_DEL_ROOM(_roomid, _room) \
	do {} while (0)
#endif

extern uint32_t room_id_generator;


room_t *alloc_room(online_t *online, uint8_t type, uint8_t mode,
		const char *name, int namelen, const char *pass, int passlen);
room_t *add_room(online_t *online, player_t *owner, uint8_t type, uint8_t mode,
		const char *name, int namelen, const char *pass, int passlen);
void del_room(room_t *room);
bool is_room_empty(room_t *room);
int count_room_unlock_seats(room_t *room, uint8_t team_no=0xFF);
int count_room_seats(room_t *room, uint8_t team_no=0xFF);
void notify_room_player(room_t *room, player_t *notifier, uint16_t cmd,
		Cmessage *p_out, bool skip_notifier=DONT_SKIP_NOTIFIER, bool complete=false);
player_t *gen_new_room_owner(room_t *room, player_t *skip_p=0);
bool close_room(room_t *room);
void proc_closed_rooms(void);





inline uint32_t get_next_room_id()
{
	++room_id_generator;
	if (unlikely(room_id_generator == 0)) {
		++room_id_generator;
	}
	return room_id_generator;
}

inline bool is_valid_room_mode(uint8_t mode)
{
	return (mode < max_room_mode);
}

/* return: true: 需要两边, false: 只需要一边 */
inline bool is_two_side_room_mode(uint8_t mode)
{
	return (mode == room_mode_free) ? true : false;
}

inline bool is_room_owner(room_t *room, player_t *p)
{
	return (room->owner == p);
}

inline room_map_t *get_room_map(online_t *online, uint8_t type, uint8_t status)
{
	if (type == room_type_pvp) {
		switch (status) {
		case room_status_idle:
			return online->idle_map;

		case room_status_asking:
			return online->asking_map;

		case room_status_fighting:
			return online->fighting_map;

		case room_status_closed:
			return online->closed_map;
		}
	} else if (type == room_type_pve) {
		switch (status) {
		case room_status_idle:
			return online->pve_idle_map;

		case room_status_asking:
			return online->pve_asking_map;

		case room_status_fighting:
			return online->pve_fighting_map;

		case room_status_closed:
			return online->closed_map;
		}
	}
	return 0;
}

inline room_t *find_status_room(online_t *online, uint8_t status, uint32_t roomid)
{
	room_map_t *room_map = 0;
	for (uint32_t tp = room_type_pvp; tp < max_room_type; tp++) {
		room_map = get_room_map(online, tp, status);
		if (unlikely(!room_map)) continue;

		room_map_iter_t iter = room_map->find(roomid);
		if (iter != room_map->end()) {
			return iter->second;
		}
	}

	return 0;
}

inline room_t *find_room(online_t *online, uint32_t roomid)
{
	for (int8_t s = room_status_idle; s < room_status_max; s++) {
		room_t *room = find_status_room(online, s, roomid);
		if (room) {
			return room;
		}
	}
	return 0;
}

/* 在房间中找人 (你妈妈喊你回家吃饭) */
inline player_t *find_room_player(room_t *room, uint32_t uid)
{
	team_map_iter_t iter = room->team_map->begin();
	for (; iter != room->team_map->end(); iter++) {
		team_t *team = iter->second;
		seat_map_iter_t iter2 = team->seat_map->begin();
		for (; iter2 != team->seat_map->end(); iter2++) {
			seat_t *seat = iter2->second;
			if (seat->player && seat->player->id == uid) {
				return seat->player;
			}
		}
	}

	/* 没找到 (这小子溜了) */
	return 0;
}

inline void dealloc_room(room_t *room)
{
	DEBUG_TLOG("dealloc_room, olid=%u, roomid=%u, "
			"type=%hhu, mode=%hhu, status=%hhu, owner=%u",
			room->online->id, room->id, room->type, room->mode, room->status,
			room->owner ? room->owner->id : 0);
	g_slice_free1(sizeof(room_t), room);
}

/* 建立直接关系链 (不对内部关系链做任何修改) */
inline void reg_room(room_map_t *room_map, room_t *room)
{
	online_t *online = room->online;
	room_map->insert(make_pair(room->id, room));
	DEBUG_TLOG("reg_room, olid=%u, roomid=%u", online->id, room->id);
}

/* 解除直接关系链 (不对内部关系链做任何修改) */
inline void unreg_room(room_t *room)
{
	online_t *online = room->online;
	room_map_t *room_map = get_room_map(online, room->type, room->status);
	if (unlikely(!room_map)) {
		WARN_TLOG("BUG: ill_room_status, olid=%u, roomid=%u, status=%hhu",
				online->id, room->id, room->status);
		return ;
	}

	DEBUG_TLOG("unreg_room, olid=%u, roomid=%u", online->id, room->id);
	room_map->erase(room->id);
}

/*
 * @brief 返回 room 还有多少人;
 * @skip_p 需要过滤的人, 0 表示不需要过滤任何人;
 * @skip_team 需要选定某个队伍, 0xFF 表示不特指任何队伍;
 * return: true: p 是 room 的最后一个成员, false: p 不是 room 最后一个成员
 */
inline int count_room_players(room_t *room, player_t *skip_p=0, uint8_t spec_team=0xFF)
{
	int count = 0;
	team_map_iter_t iter = room->team_map->begin();
	for (; iter != room->team_map->end(); iter++) {
		team_t *team = iter->second;
		if (spec_team != 0xFF && team->id != spec_team) continue;

		seat_map_iter_t iter2 = team->seat_map->begin();
		for (; iter2 != team->seat_map->end(); iter2++) {
			seat_t *seat = iter2->second;
			if (!seat->player) continue;
			if (skip_p && seat->player == skip_p) continue;
			++count;
		}
	}

	return count;
}

/* return: 0: 房间满了, !0: 找到的 seat */
inline seat_t *find_room_first_empty_seat(room_t *room, uint8_t spec_team=0xFF)
{
	team_map_iter_t iter = room->team_map->begin();
	for (; iter != room->team_map->end(); iter++) {
		team_t *team = iter->second;
		/* 仅对某个队伍的座位感兴趣 */
		if (spec_team != 0xFF && team->id != spec_team) continue;

		seat_map_iter_t iter2 = team->seat_map->begin();
		for (; iter2 != team->seat_map->end(); iter2++) {
			seat_t *seat = iter2->second;
			if (!seat->player) return seat;
		}
	}

	return 0;
}

/* return: true: 房间满, false: 房间不满 */
inline bool is_room_full(room_t *room)
{
	seat_t *seat = find_room_first_empty_seat(room);
	return (seat == 0);
}

inline bool is_room_last_player(room_t *room, player_t *p)
{
	return (count_room_players(room, p) > 0) ? false : true;
}


inline bool is_room_in_battle(room_t *room)
{
	return (room->status == room_status_fighting);
}

/* return: true: room 已经成功从原来的 room_map 切换到 to_status 的 room_map 上了;
 * false: room 还在原 room_map; */
inline bool pvp_room_switch_status(room_t *room, uint8_t to_status)
{
	if (room->status == to_status) return true;

	online_t *online = room->online;
	room_map_t *from_room_map = get_room_map(online, room->type, room->status);
	room_map_t *to_room_map = get_room_map(online, room->type, room->status);
	if (!from_room_map || !to_room_map) {
		return false;
	}
	from_room_map->erase(room->id);
	to_room_map->insert(make_pair(room->id, room));
	room->status = to_status;
	return true;
}



#define ROOM_MUST_IN_BATTLE(_room) \
	do { \
		if (is_room_in_battle(_room)) { \
			 return send_header_to_player(p, BTLSW_ERR_NOT_IN_BATTLE); \
		} \
	} while (0)

#define ROOM_MUST_NOT_IN_BATTLE(_room) \
	do { \
		if (is_room_in_battle(_room)) { \
			 return send_header_to_player(p, BTLSW_ERR_ALREADY_IN_BATTLE); \
		} \
	} while (0)



#endif // __ROOM_HPP__
