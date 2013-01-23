#ifndef __ONLINE_HPP__
#define __ONLINE_HPP__


#include "common.hpp"



void del_online(online_t *online);
bool online_reg_able(fdsession_t *fdsess, uint32_t olid);
online_t *add_online(fdsession_t *fdsess, uint32_t olid);


inline online_t *get_online_by_fd(uint32_t fd)
{
	online_fd_map_iter_t iter = online_fd_map.find(fd);
	if (iter == online_fd_map.end()) {
		return 0;
	}

	return iter->second;
}

inline online_t *get_online_by_olid(uint32_t olid)
{
	online_olid_map_iter_t iter = online_olid_map.find(olid);
	if (iter == online_olid_map.end()) {
		return 0;
	}

	return iter->second;
}

inline online_t *alloc_online(fdsession_t *fdsess, uint32_t olid)
{
	online_t *online = reinterpret_cast<online_t *>(g_slice_alloc0(sizeof(online_t)));
	if (unlikely(!online)) return 0;

	online->fdsess = fdsess;
	online->id = olid;

	online->idle_map = new room_map_t();
	online->asking_map = new room_map_t();
	online->fighting_map = new room_map_t();

	online->pve_idle_map = new room_map_t();
	online->pve_asking_map = new room_map_t();
	online->pve_fighting_map = new room_map_t();

	online->closed_map = new room_map_t();

	online->player_map = new player_map_t();

	DEBUG_TLOG("alloc_online, olid=%u, fd=%u", online->id, online->fdsess->fd);
	return online;
}

inline void dealloc_online(online_t *online)
{
	DEBUG_TLOG("dealloc_online, olid=%u, fd=%u", online->id, online->fdsess->fd);
	g_slice_free1(sizeof(online_t), online);
}

/* 建立直接关系链 (不对内部关系链做任何修改) */
inline void reg_online(online_t *online)
{
	online_fd_map.insert(make_pair(online->fdsess->fd, online));
	online_olid_map.insert(make_pair(online->id, online));

	fdsession_t *fdsess = online->fdsess;
	DEBUG_TLOG("reg_online, olip=0x%X, fd=%d, olid=%u",
			fdsess->remote_ip, fdsess->fd, online->id);
}

/* 解除直接关系链 (不对内部关系链做任何修改) */
inline void unreg_online(online_t *online)
{
	fdsession_t *fdsess = online->fdsess;
	DEBUG_TLOG("unreg_online, olip=0x%X, fd=%d, olid=%u",
			fdsess->remote_ip, fdsess->fd, online->id);

	online_fd_map.erase(online->fdsess->fd);
	online_olid_map.erase(online->id);
}




#endif // __ONLINE_HPP__
