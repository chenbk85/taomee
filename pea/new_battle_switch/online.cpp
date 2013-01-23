#include "common.hpp"
#include "room.hpp"
#include "online.hpp"


online_fd_map_t		online_fd_map;
online_olid_map_t	online_olid_map;



bool online_reg_able(fdsession_t *fdsess, uint32_t olid)
{
	/*
	 * online 启动时过来注册, 先做2个检查, 以确认:
	 * 	(1) 没有同一个online注册了多个id;
	 * 	(2) 也没有多个online注册了相同的id,
	 */
	int fd = fdsess->fd;
	online_t *online_by_fd = get_online_by_fd(fd);
	online_t *online_by_olid = get_online_by_olid(olid);
	if (likely(!online_by_fd && !online_by_olid)) {
		return true;
	}

	/* (1) 确认不是同一个 online 来注册 */
	if (online_by_fd) {
		if (online_by_fd->id == olid) {
			/* 相同online 注册 相同 id */
			ERROR_TLOG("olregerr: dupol_dupid, olip=0x%X, fd=%d, olid=%u",
					fdsess->remote_ip, fd, olid);
		} else {
			/* 相同online 注册 不同id */
			ERROR_TLOG("olregerr: dupol_diffid, olip=0x%X, fd=%d, old_olid=%u, new_olid=%u",
					fdsess->remote_ip, fd, online_by_fd->id, olid);
		}

		/* (3) 顺便检查下 id_map 和 fd_map 是否一致 */
		online_t *online = get_online_by_olid(online_by_fd->id);
		if (!online) {
			ERROR_TLOG("olregerr: olid_map(no) != fd_map(has), olip=0x%X, fd=%d, olid=%u",
					fdsess->remote_ip, fd, olid);
		}
	}

	/* (2) 确认不是 多个online 来注册同一个olid */
	if (online_by_olid) {
		if (online_by_olid->fdsess->fd == fd) {
			/* 相同online 注册 相同id */
			ERROR_TLOG("olregerr: dupol_dupid, olip=0x%X, fd=%d, olid=%u",
					fdsess->remote_ip, fd, olid);
		} else {
			/* 不同 online 注册相同 id */
			ERROR_TLOG("olregerr: diffol_dupid (new-cfg err), "
					"olip=0x%X, fd=%d, old_olid=%u, new_olid=%u",
					fdsess->remote_ip, fd, online_by_olid->id, olid);
		}

		/* (3) 顺便检查下 id_map 和 fd_map 是否一致 */
		online_t *online = get_online_by_fd(online_by_olid->fdsess->fd);
		if (!online) {
			ERROR_TLOG("olregerr: olid_map(has) != fd_map(no), olip=0x%X, fd=%d, olid=%u",
					fdsess->remote_ip, fd, olid);
		}
	}
	
	return false;
}


/* 添加 online (初始化: 直接关系链 + 下级关系链 + 自身数据;) */
online_t *add_online(fdsession_t *fdsess, uint32_t olid)
{
	if (!online_reg_able(fdsess, olid)) {
		return 0;
	}

	/* 下级关系链 + 自身数据 */
	online_t *online = alloc_online(fdsess, olid);
	if (unlikely(!online)) {
		ERROR_TLOG("failed to alloc_online, olip=0x%X, fd=%d, olid=%u",
				fdsess->remote_ip, fdsess->fd, olid);
		return 0;
	}

	/* 直接关系链 */
	reg_online(online);

	DEBUG_TLOG("add_online, olid=%u", online->id);
	return online;
}

void release_room_map(room_map_t *room_map)
{
	room_t *room = 0;
	room_map_iter_t iter = room_map->begin();
	while (iter != room_map->end()) {
		room = iter->second;
		iter++;
		del_room(room);
	}
	room_map->clear();
	delete room_map;
}

/* 删除 online (释放: 直接关系链 + 下级关系链 + 自身数据;) */
void del_online(online_t *online)
{
	/* 下级关系链 */
	release_room_map(online->fighting_map);
	online->fighting_map = 0;

	release_room_map(online->asking_map);
	online->asking_map= 0;

	release_room_map(online->idle_map);
	online->idle_map = 0;

	release_room_map(online->pve_fighting_map);
	online->pve_fighting_map = 0;

	release_room_map(online->pve_asking_map);
	online->pve_asking_map = 0;

	release_room_map(online->pve_idle_map);
	online->pve_idle_map = 0;

	release_room_map(online->closed_map);
	online->closed_map = 0;

	/* 直接关系链 */
	unreg_online(online);

	DEBUG_TLOG("del_online, olid=%u", online->id);

	/* 自身数据 */
	dealloc_online(online);
}
