#include "common.hpp"
#include "btlsw_debug.hpp"
#include "room.hpp"

static const char *room_type_str[] = {
	"room_type_pvp", 
	"room_type_pve",
};

static const char *room_status_str[] = {
	"room_status_idle", 
	"room_status_asking",
	"room_status_fighting",
	"room_status_closed",
};

void tracelog_list_room_players(room_t *room, const char *file, int caller)
{
	TRACE_TLOG("list_room_players(%s:%d)[olid=%u, roomid=%u, owner=%u]:",
			file, caller, room->online->id, room->id, room->owner->id);
	TRACE_TLOG("------------------------------------------------------------");
	if (!room) return ;

	team_map_iter_t iter = room->team_map->begin();
	for (; iter != room->team_map->end(); iter++) {
		team_t *team = iter->second;
		seat_map_iter_t iter2 = team->seat_map->begin();
		for (; iter2 != team->seat_map->end(); iter2++) {
			seat_t *seat = iter2->second;
			if (!seat->player) continue;
			TRACE_TLOG("team=%hhu, seat=%hhu, u=%u",
					seat->team->id, seat->id, seat->player->id);
		}
	}
	TRACE_TLOG("------------------------------------------------------------");
}


void tracelog_list_online_rooms(online_t *online, const char *file, int caller)
{
	TRACE_TLOG("list_online_rooms(%s:%d)[olid=%u]:", file, caller, online->id);
	TRACE_TLOG("------------------------------------------------------------");

	room_map_t *room_map = 0;
	room_map_iter_t iter;
	for (uint32_t tp = room_type_pvp; tp < max_room_type; tp++) {
		for (uint32_t s = room_status_idle; s < room_status_max; s++) {
			room_map = get_room_map(online, tp, s);
			if (!room_map) continue;
			TRACE_TLOG("-->(%s:%s) ...", room_type_str[tp], room_status_str[s]);
			iter = room_map->begin();
			for (; iter != room_map->end(); iter++) {
				room_t *room = iter->second;
				TRACE_TLOG("---->roomid=%u, player_count=%d",
						room->id, count_room_players(room));
			}
		}
	}
}
