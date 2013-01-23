#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__


#include "common.hpp"
#include "cli_proto.hpp"



extern player_map_t				player_map;
extern cached_player_map_t		cached_player_map;




void del_player(player_t *p);
void cache_a_pkg(player_t *p, const void *buf, uint32_t buflen);
void proc_cached_pkg();
void player_leave_seat(player_t *p, bool is_kicked=false);
bool chg_room_player_status(player_t *p, uint8_t to_status);
bool player_sit_down(player_t *p, seat_t *seat);
bool player_sit_down(player_t *p, room_t *room, uint8_t team_no, uint8_t seat_no);



inline uint64_t make_player_key(uint32_t uid, int32_t role_tm)
{
	uint64_t key = uid;
	return (key << 32 | role_tm);
}

inline uint64_t make_player_key(player_t *p)
{
	return make_player_key(p->id, p->role_tm);
}

inline player_t *get_player(uint32_t uid, int32_t role_tm)
{
	player_map_iter_t iter = player_map.find(make_player_key(uid, role_tm));
	if (iter == player_map.end()) {
		return 0;
	}
	return iter->second;
}

inline player_t *alloc_player(online_t *online, uint32_t uid, int32_t role_tm)
{
	player_t *p = reinterpret_cast<player_t *>(g_slice_alloc0(sizeof(player_t)));
	if (unlikely(!p)) {
		WARN_TLOG("ISSUE: nomem_for_player: u=%u, role_tm=%d, olid=%u",
				uid, role_tm, online->id);
		return 0;
	}

	p->id = uid;
	p->role_tm = role_tm;
	p->alloc_time = get_now_tv()->tv_sec;
	p->waitcmd = 0;
	p->seq = 0;
	p->status = player_status_idle;
	p->seat = 0;
	p->online = online;
	p->cache_pkg_queue = new cache_pkg_queue_t();

	DEBUG_TLOG("alloc_player, u=%u, role_tm=%d, alloc_time=%d, olid=%u",
			p->id, p->role_tm, p->alloc_time, online->id);
	return p;
}

inline void dealloc_player(player_t *p)
{
	DEBUG_TLOG("dealloc_player, u=%u, role_tm=%d, alloc_time=%d, olid=%u",
			p->id, p->role_tm, p->alloc_time, p->online->id);
	g_slice_free1(sizeof(player_t), p);
}

/* 建立直接关系链 (不对内部关系链做任何修改) */
inline void reg_player(player_t *p)
{
	online_t *online = p->online;
	uint64_t key = make_player_key(p->id, p->role_tm);

	player_map.insert(make_pair(key, p));
	online->player_map->insert(make_pair(key, p));

	DEBUG_TLOG("reg_player, u=%u, role_tm=%d, olid=%u", p->id, p->role_tm, online->id);
}

/* 解除直接关系链 (不对内部关系链做任何修改) */
inline void unreg_player(player_t *p)
{
	online_t *online = p->online;
	uint64_t key = make_player_key(p->id, p->role_tm);

	player_map.erase(key);
	online->player_map->erase(p->id);

	DEBUG_TLOG("unreg_player, u=%u, role_tm=%d, olid=%u", p->id, p->role_tm, online->id);
}

inline player_t *add_player(online_t *online, uint32_t uid, int32_t role_tm)
{
	/* 自身数据 (无下级关系链, 以后有工会, 不过应该存储在 player_t 中的 online_player_copy) */
	player_t *p = alloc_player(online, uid, role_tm);
	if (unlikely(!p)) {
		ERROR_TLOG("failed to alloc_player, u=%u, role_tm=%d, olid=%u",
				uid, role_tm, online->id);
		return 0;
	}

	/* 直接关系链 */
	reg_player(p);

	return p;
}

inline bool is_in_cached_player_map(player_t *p)
{
	uint64_t key = make_player_key(p);
	cached_player_map_iter_t iter = cached_player_map.find(key);
	return (iter != cached_player_map.end());
}

inline void reg_cached_player_queue(player_t *p)
{
	uint64_t key = make_player_key(p);
	cached_player_map.insert(make_pair(key, p));
}

inline void unreg_cached_player_queue(player_t *p)
{
	uint64_t key = make_player_key(p);
	cached_player_map.erase(key);
}

inline bool is_npc_player(player_t *p)
{
	return !is_valid_uid(p->id);
}

inline bool is_player_in_room(player_t *p)
{
	return (p->seat != 0);
}

#define PLAYER_MUST_INROOM(_player) \
	do { \
		if (!is_player_in_room(_player)) { \
			return send_header_to_player(_player, BTLSW_ERR_NOT_INROOM); \
		} \
	} while(0)

#define PLAYER_MUST_NOT_INROOM(_player) \
	do { \
		if (is_player_in_room(_player)) { \
			return send_header_to_player(_player, BTLSW_ERR_ALREADY_INROOM); \
		} \
	} while(0)

#define PLAYER_MUST_ROOM_OWNER(_room, _player) \
	do { \
		if (!is_room_owner(_room, _player)) { \
			return send_header_to_player(_player, BTLSW_ERR_NOT_ROOM_OWNER); \
		} \
	} while(0)

#define PLAYER_MUST_NOT_ROOM_OWNER(_room, _player) \
	do { \
		if (is_room_owner(_room, _player)) { \
			return send_header_to_player(_player, BTLSW_ERR_CANT_BE_ROOM_OWNER); \
		} \
	} while(0)

/* TODO(zog): 对战模块还没实现..., 实现后, 要把这些判断加到所有的房间协议里去 */
#define PLAYER_MUST_IN_BATTLE(_player) \
	do { \
		if (!is_room_player_in_battle(_player)) { \
			return send_header_to_player(_player, BTLSW_ERR_PLAYER_NOT_IN_BATTLE); \
		} \
	} while(0)

#define PLAYER_MUST_NOT_IN_BATTLE(_player) \
	do { \
		if (is_room_player_in_battle(_player)) { \
			return send_header_to_player(_player, BTLSW_ERR_PLAYER_IN_BATTLE); \
		} \
	} while(0)


#endif // __PLAYER_HPP__
