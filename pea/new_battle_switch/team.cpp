#include "team.hpp"
#include "seat.hpp"



/* 注意: 数组的下标就是 room_type_t 中对应的类型, 所以设定位子数量要按照顺序来 */
uint8_t team_max_seat[max_room_type] = {
	team_max_seat_pvp, /* room_type_pvp */
	team_max_seat_pve, /* room_type_pve */
};

/* 添加 room (初始化: 直接关系链 + 下级关系链 + 自身数据;) */
team_t *add_team(room_t *room, uint8_t team_no, uint8_t max_seat)
{
	online_t *online = room->online;

	/* 下级关系链 + 自身数据 */
	team_t *team = alloc_team(room, team_no, max_seat);
	if (unlikely(!team)) {
		ERROR_TLOG("failed to alloc_team, olid=%u, roomid=%u, "
				"team=%hhu, max_seat=%hhu", online->id, room->id, team_no, max_seat);
		return 0;
	}

	for (uint8_t i = 0; i < max_seat; i++) {
		seat_t *seat = add_seat(team, i, seat_lock_off);
		if (unlikely(!seat)) {
			ERROR_TLOG("failed_to_add_team_when_add_room, "
					"olid=%u, roomid=%u, seat=%hhu", online->id, room->id, i);
			del_team(team);
			return 0;
		}
	}

	/* 直接关系链 */
	reg_team(team);

	return team;
}

void del_team(team_t *team)
{
	/* 下级关系链 */
	seat_t *seat = 0;
	seat_map_iter_t iter = team->seat_map->begin();
	while (iter != team->seat_map->end()) {
		seat = iter->second;
		iter++;
		del_seat(seat);
	}
	team->seat_map->clear();
	delete team->seat_map;
	team->seat_map = 0;

	/* 直接关系链 */
	unreg_team(team);

	DEBUG_TLOG("del_team, team_id=%u", team->id);

	/* 自身数据 */
	dealloc_team(team);
}

/* return: true: team空了, false: team不空 */
bool is_team_empty(team_t *team)
{
	seat_t *seat = 0;
	seat_map_iter_t iter = team->seat_map->begin();
	for (; iter != team->seat_map->end(); iter++) {
		seat = iter->second;
		if (!is_seat_empty(seat)) {
			return false;
		}
	}

	return true;
}

void notify_team_player(team_t *team, player_t *notifier, uint16_t cmd,
		Cmessage *p_out, bool skip_notifier, bool complete)
{
	seat_t *seat = 0;
	seat_map_iter_t iter = team->seat_map->begin();
	for (; iter != team->seat_map->end(); iter++) {
		seat = iter->second;
		notify_seat_player(seat, notifier, cmd, p_out, skip_notifier, false);
	}

	if (complete) {
		notifier->waitcmd = 0;
	}
}

