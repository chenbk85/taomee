#ifndef __TEAM_HPP__
#define __TEAM_HPP__


#include "seat.hpp"
#include "common.hpp"


void del_team(team_t *team);
team_t *add_team(room_t *room, uint8_t team_no, uint8_t max_seat);
bool is_team_empty(team_t *team);
void notify_team_player(team_t *team, player_t *notifier, uint16_t cmd,
		Cmessage *p_out, bool skip_notifier=DONT_SKIP_NOTIFIER, bool complete=false);



inline bool is_valid_team_no(uint32_t team_no)
{
	return (team_no < max_team_no);
}

inline team_t *find_team(room_t *room, uint8_t team_no)
{
	team_map_iter_t iter = room->team_map->find(team_no);
	if (iter == room->team_map->end()) {
		return 0;
	}
	return iter->second;
}

inline team_t *alloc_team(room_t *room, uint8_t team_no, uint8_t max_seat)
{
	team_t *team = find_team(room, team_no);
	if (unlikely(team)) {
		WARN_TLOG("BUG dup_new_team: roomid=%u, team=%hhu", room->id, team_no);
		return 0;
	}

	team = reinterpret_cast<team_t *>(g_slice_alloc0(sizeof(team_t)));
	if (unlikely(!team)) {
		WARN_TLOG("nomem_for_team, roomid=%u, team=%hhu", room->id, team_no);
		return 0;
	}

	team->id = team_no;
	team->max_seat = max_seat;
	team->seat_map = new seat_map_t();
	team->room = room;

	DEBUG_TLOG("alloc_team, team=%hhu, max_seat=%hhu, olid=%u, roomid=%u",
			team->id, team->max_seat, room->online->id, room->id);
	return team;
}

inline void dealloc_team(team_t *team)
{
	room_t *room = team->room;
	DEBUG_TLOG("dealloc_team, team=%hhu, olid=%u, roomid=%u",
			team->id, room->online->id, room->id);
	g_slice_free1(sizeof(team_t), team);
}

/* 建立直接关系链 (不对内部关系链做任何修改) */
inline void reg_team(team_t *team)
{
	room_t *room = team->room;
	room->team_map->insert(make_pair(team->id, team));

	online_t *online = room->online;
	DEBUG_TLOG("reg_team, team=%hhu, roomid=%u, olid=%u", team->id, room->id, online->id);
}

/* 解除直接关系链 (不对内部关系链做任何修改) */
inline void unreg_team(team_t *team)
{
	room_t *room = team->room;
	online_t *online = room->online;
	room->team_map->erase(team->id);

	DEBUG_TLOG("unreg_team, team=%hhu, roomid=%u, olid=%u", team->id, room->id, online->id);
}



#endif //__TEAM_HPP__
