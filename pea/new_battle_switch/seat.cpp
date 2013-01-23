#include "seat.hpp"



seat_t *alloc_seat(team_t *team, uint8_t seat_no)
{
	room_t *room = team->room;
	online_t *online = room->online;

	seat_t *seat = find_seat(team, seat_no);
	if (unlikely(seat)) {
		WARN_LOG("BUG: alloc_dup_seat, seat=%hhu, team=%hhu, roomid=%u, olid=%u",
				seat_no, team->id, room->id, online->id);
		return 0;
	}

	seat = reinterpret_cast<seat_t *>(g_slice_alloc0(sizeof(seat_t)));
	if (unlikely(!seat)) {
		WARN_TLOG("ISSUE: nomem_for_seat: seat=%hhu, team=%hhu, roomid=%u, olid=%u",
				seat_no, team->id, room->id, online->id);
		return 0;
	}

	seat->id = seat_no;
	seat->team = team;

	DEBUG_TLOG("alloc_seat, seat=%hhu, team=%hhu, roomid=%u, olid=%u",
			seat->id, team->id, room->id, online->id);
	return seat;
}

seat_t *add_seat(team_t *team, uint8_t seat_no, uint8_t lock)
{
	room_t *room = team->room;
	online_t *online = room->online;

	/* 下级关系链 + 自身数据 */
	seat_t *seat = alloc_seat(team, seat_no);
	if (unlikely(!seat)) {
		ERROR_TLOG("fail_alloc_seat, team=%hhu, roomid=%u, olid=%u",
				team->id, room->id, online->id);
		return 0;
	}

	/* 初始锁状态 */
	seat->lock = lock;

	/* 直接关系链 */
	reg_seat(team, seat);
	return seat;
}

void del_seat(seat_t *seat)
{
	/* 下级关系链 (player) */
	/* 注意: player 虽是 seat 的下级对象,
	 * 但 player 同时又是独立对象, 即: player不需要依附seat而存在,
	 * player 仅依附 online 而存在, 销毁 online 时才需要销毁 player */
	if (seat->player) {
		if (likely(seat->player->seat)) {
			player_leave_seat(seat->player);

		} else {
			WARN_TLOG("BUG: seat->player=%p, seat->player->seat=%p",
					seat->player, seat->player->seat);
			seat->player = 0;
		}
	}

	/* 自身数据 */

	/* 直接关系链 */
	unreg_seat(seat);

	DEBUG_TLOG("del_seat, seat_id=%u", seat->id);

	/* 注意: 不管位子有没有人, 销毁位子时, 上面的玩家对象(有的话)不会被销毁,
	 *
	 * 其实, 玩家对象是否需要被销毁取决于3个因素:
	 * 1 玩家自己主动关浏览器;
	 * 2 battle_switch 与 online 的连接断开;
	 * 3 玩家重新进入大厅, 而导致之前的player(有的话), 被销毁; (通常online挂掉被拉起后出现这种情况)
	 *
	 * 注意: 玩家对象中所指向的 seat/online 等关系链的调整也不关本函数的事,
	 * 需要调用者根据情况自行解决;
	 */
	dealloc_seat(seat);
}

void notify_seat_player(seat_t *seat, player_t *notifier, uint16_t cmd,
		Cmessage *p_out, bool skip_notifier, bool complete)
{
	player_t *p = 0;
	if (!seat->player) {
		goto end;
	}

	p = seat->player;
	if ((skip_notifier == SKIP_NOTIFIER) && (p == notifier)) {
		goto end;
	}

/* TODO(zog): 为方便用 1,2 这些小号测试, 暂时注释掉 */
#if 0
	if (is_npc_player(p)) {
DEBUG_TLOG("notify_seat_player-3");
		goto end;
	}
#endif

	/* notify 不改变接收者的 waitcmd */
	send_to_player(p, cmd, p_out, false);

end:
	if (complete) {
		notifier->waitcmd = 0;
	}
}
