#include "dispatch.hpp"
#include "room.hpp"
#include "player.hpp"


player_map_t  player_map;
cached_player_map_t  cached_player_map;



void cache_a_pkg(player_t *p, const void *buf, uint32_t buflen)
{   
	uint32_t len = sizeof(cached_pkg_t) + buflen;
	cached_pkg_t *cache = reinterpret_cast<cached_pkg_t *>(g_slice_alloc0(len));
	cache->len = len;
	memcpy(cache->pkg, buf, buflen);

	p->cache_pkg_queue->push(cache);
	if (!is_in_cached_player_map(p)) {
		reg_cached_player_queue(p);
	}
}

void proc_cached_pkg()
{
	cached_pkg_t *cache = 0;

	cached_player_map_iter_t iter = cached_player_map.begin();
	while (iter != cached_player_map.end()) {
		cached_player_map_iter_t tmp_iter = iter;
		++iter;

		player_t *p = tmp_iter->second;
		int fd = p->online->fdsess->fd;
		while(p->waitcmd == 0) {
			cache = p->cache_pkg_queue->front();
			int err = dispatch(cache->pkg, p->online->fdsess, false);
			g_slice_free1(cache->len, cache);
			p->cache_pkg_queue->pop();
			if (p->cache_pkg_queue->empty()) {
				cached_player_map.erase(tmp_iter);
			}
			if (err) {
				close_client_conn(fd);
				break;
			}
			if (p->cache_pkg_queue->empty()) {
				break;
			}
		}
	}
}

void release_player_linked_data(player_t *p)
{
	/* cached_pkg_queue */
	cached_pkg_t *cache = 0;
	while (!p->cache_pkg_queue->empty()) {
		cache = p->cache_pkg_queue->front();
		g_slice_free1(cache->len, cache);
		p->cache_pkg_queue->pop();
	}
	delete p->cache_pkg_queue;
	p->cache_pkg_queue = 0;

	if (is_in_cached_player_map(p)) {
		unreg_cached_player_queue(p);
	}

	/* TODO(zog): 对战资源??? */
}

void del_player(player_t *p)
{
	/* 直接关系链 (player_map 和 online->player_map) */
	unreg_player(p);


	/* TODO(zog): 离开对战组 */


	/* 间接关系链 (依次检查是否需要释放: seat->team->room->房主?) */
	if (is_player_in_room(p)) {
		player_leave_seat(p);
	}

	/* 无下级关系链 */

	/* 自身关联数据 (指针指向的数据, eg: cached_pkg_queue) */
	release_player_linked_data(p);

	/* 自身数据 */
	dealloc_player(p);
}

bool player_sit_down(player_t *p, seat_t *to_seat)
{
	assert(to_seat);

	/* to_seat 上必须没有人 */
	if (to_seat->player) {
		return false;
	}

	seat_t *from_seat = p->seat; // NULL 表示是刚进房间, 第一次坐下
	if (from_seat) {
		from_seat->player = 0;
	}

	p->seat = to_seat;
	to_seat->player = p;

	room_t *room = to_seat->team->room;

	btlsw_player_sitdown_notify_out out;
	out.sitdown_uid = p->id;
	out.sitdown_role_tm = p->role_tm;
	out.from_team_id = from_seat ? from_seat->id : 0xFF;
	out.from_seat_id = from_seat ? from_seat->id : 0xFF;
	out.to_team_id = to_seat->team->id;
	out.to_seat_id = to_seat->id;
	notify_room_player(room, p, btlsw_player_sitdown_notify_cmd, &out, DONT_SKIP_NOTIFIER, false);

	return true;
}

bool player_sit_down(player_t *p, room_t *room, uint8_t team_no, uint8_t seat_no)
{
	if (p->seat) {
		return false;
	}

	team_t *team = find_team(room, team_no);
	if (unlikely(!team)) {
		return false;
	}
	seat_t *seat = find_seat(team, seat_no);
	if (unlikely(!seat)) {
		return false;
	}

	return player_sit_down(p, seat);
}

void player_leave_seat(player_t *p, bool is_kicked)
{
	seat_t *seat = p->seat;
	team_t *team = seat->team;
	room_t *room = team->room;
	online_t *online = room->online;
	uint32_t room_id = room->id;
	uint8_t team_id = team->id;
	uint8_t seat_id = seat->id;


	seat->player = 0;
	p->seat = 0;
	p->status = player_status_idle;

	if (count_room_players(room) == 0) {
		close_room(room);
		room = 0; team = 0; seat = 0;
#ifdef ENABLE_TRACE_LOG
		tracelog_list_online_rooms(online, __FILE__, __LINE__);
#endif
	} else {
		room->owner = gen_new_room_owner(room);
		assert(room->owner);
		DEBUG_TLOG("gen_new_room_owner: u=%u, roomid=%u, olid=%u",
				room->owner->id, room->id, room->online->id);
	}

	btlsw_player_leave_notify_out out;
	out.leaver_team = team_id;
	out.leaver_seat = seat_id;
	out.leaver_uid = p->id;
	out.leaver_role_tm = p->id;
	out.is_kicked = is_kicked ? 1 : 0;
	if (room) { // 初始化成 0 的, 如果房间销毁了的话, 就不用显示的赋值0了
		player_t *owner = room->owner;
		out.owner_team = owner->seat->team->id;
		out.owner_seat = owner->seat->id;
		out.owner_uid = owner->id;
		out.owner_role_tm = owner->role_tm;
	}

	if (room) {
		notify_room_player(room, p, btlsw_player_leave_notify_cmd, &out, SKIP_NOTIFIER, false);
	}
	if (is_kicked) {
		/* 被踢者已不在房间, 要单独发送通知 */
		send_to_player(p, btlsw_player_leave_notify_cmd, &out, false);
	}

	DEBUG_TLOG("player_leave_seat(kick=%d), u=%u, role_tm=%d, seat=%hhu, team=%hhu, "
			"owner=%u, oseat=%hhu, oteam=%hhu, roomid=%u, olid=%u",
			is_kicked ? 1 : 0, p->id, p->role_tm, seat_id, team_id,
			out.owner_uid, out.owner_seat, out.owner_team, room_id, p->online->id);
}

bool chg_room_player_status(player_t *p, uint8_t to_status)
{
	uint8_t from_status = p->status;
	p->status = to_status;

	room_t *room = p->seat->team->room;

	btlsw_chg_player_status_notify_out out;
	out.chg_uid = p->id;
	out.chg_role_tm = p->role_tm;
	out.from_status = from_status;
	out.to_status = to_status;
	notify_room_player(room, p, btlsw_chg_player_status_notify_cmd, &out, DONT_SKIP_NOTIFIER, false);

	return true;
}
