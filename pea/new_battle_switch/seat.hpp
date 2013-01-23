#ifndef __SEAT_HPP__
#define __SEAT_HPP__


#include "cli_proto.hpp"
#include "player.hpp"
#include "common.hpp"


seat_t *alloc_seat(team_t *team, uint8_t seat_no);
seat_t *add_seat(team_t *team, uint8_t seat_no, uint8_t lock);
void del_seat(seat_t *seat);
void notify_seat_player(seat_t *seat, player_t *notifier, uint16_t cmd,
		Cmessage *p_out, bool skip_notifier=DONT_SKIP_NOTIFIER, bool complete=false);




inline void dealloc_seat(seat_t *seat)
{
	room_t *room = seat->team->room;
	DEBUG_TLOG("dealloc_seat, seat=%hhu, team=%hhu, olid=%u, roomid=%u",
			seat->id, seat->team->id, room->online->id, room->id);
	g_slice_free1(sizeof(seat_t), seat);
}

inline seat_t *find_seat(team_t *team, uint8_t seat_no)
{
	seat_map_iter_t iter = team->seat_map->find(seat_no);
	if (iter == team->seat_map->end()) {
		return 0;
	}
	return iter->second;
}

inline void reg_seat(team_t *team, seat_t *seat)
{
	team->seat_map->insert(make_pair(seat->id, seat));
	room_t *room = team->room;
	online_t *online = room->online;
	DEBUG_TLOG("reg_seat, seat=%hhu, team=%hhu, roomid=%u, olid=%u",
			seat->id, team->id, room->id, online->id);
}

inline void unreg_seat(seat_t *seat)
{
	team_t *team = seat->team;
	room_t *room = team->room;
	online_t *online = room->online;
	DEBUG_TLOG("unreg_seat, seat=%hhu, team=%hhu, roomid=%u, olid=%u",
			seat->id, team->id, room->id, online->id);
	team->seat_map->erase(seat->id);
}

inline void set_seat_locked(seat_t *seat)
{
	seat->lock = seat_lock_on;
}

inline void set_seat_unlocked(seat_t *seat)
{
	seat->lock = seat_lock_off;
}

/* 注意: 调用者要保证 onoff 的合法性 */
inline void set_seat_onoff(seat_t *seat, uint8_t onoff)
{
	seat->lock = onoff;
}

/* return: true: seat是上锁的, false: seat没有上锁 */
inline bool is_seat_locked(seat_t *seat)
{
	return (seat->lock == seat_lock_on);
}

/* return: true: seat空了, false: seat不空 */
inline bool is_seat_empty(seat_t *seat)
{
	return (seat->player ? false: true);
}


#define SEAT_MUST_EMPTY(_seat) \
	do { \
		if (!is_seat_empty(seat)) { \
			return send_header_to_player(p, BTLSW_ERR_SEAT_DONT_EMPTY); \
		} \
	} while (0)

#define SEAT_MUST_NOT_EMPTY(_seat) \
	do { \
		if (is_seat_empty(seat)) { \
			return send_header_to_player(p, BTLSW_ERR_SEAT_IS_EMPTY); \
		} \
	} while (0)


#endif // __SEAT_HPP__
