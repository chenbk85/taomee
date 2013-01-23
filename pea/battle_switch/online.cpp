/** 
 * ========================================================================
 * @file online.cpp
 * @brief 
 * @version 1.0
 * @date 2011-12-12
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

extern "C" {
#include <libtaomee/timer.h>
}

#include "online.hpp"
#include "proto.hpp"
#include "dispatch.hpp"



typedef std::map<int, c_online *> fd_online_map_t;
typedef std::map<uint32_t, c_online *> id_online_map_t;

fd_online_map_t m_fd_online_map;
id_online_map_t m_id_online_map;


c_online *get_online_by_fd(int fd)
{
    fd_online_map_t::iterator it = m_fd_online_map.find(fd);
    if (it == m_fd_online_map.end()) {
        return 0;
    } else {
        return it->second;
    }
}

c_online *get_online(uint32_t id)
{
    id_online_map_t::iterator it = m_id_online_map.find(id);
    if (it == m_id_online_map.end()) {
        return 0;
    } else {
        return it->second;
    }
}

#if 0
c_online *get_online(c_online *p, uint32_t id)
{
    if (id == p->m_id) {
        *pp_online = p;
        return 0;
    }

    return get_online(id);
}
#endif

void add_online(c_online * p_online)
{
    int fd = p_online->m_fdsess->fd;
    uint32_t online_id = p_online->m_id;

    m_fd_online_map.insert(pair<int, c_online *>(fd, p_online));
    m_id_online_map.insert(pair<uint32_t, c_online *>(online_id, p_online));
}

int del_online_by_fd(int online_fd)
{
    fd_online_map_t::iterator it = m_fd_online_map.find(online_fd);
    if (it != m_fd_online_map.end()) {
        c_online * p_online = it->second;
        uint32_t id = p_online->m_id;
        DEBUG_TLOG("DEL_ONLINE1, fd=%u, olid=%u, olip=%u", online_fd, id,
				p_online->m_fdsess ? p_online->m_fdsess->remote_ip : 0);
        delete p_online;
        m_id_online_map.erase(id);
        m_fd_online_map.erase(it);
    }
    return 0;
}

int del_online(uint32_t online_id)
{
    id_online_map_t::iterator it = m_id_online_map.find(online_id);
    if (it != m_id_online_map.end()) {
        c_online *p_online = it->second;
        int fd = p_online->m_fdsess->fd;
        INFO_TLOG("DEL_ONLINE2, fd=%u, olid=%u, olip=%u", fd, online_id,
				p_online->m_fdsess ? p_online->m_fdsess->remote_ip : 0);
        delete p_online;
        m_fd_online_map.erase(fd);
        m_id_online_map.erase(it);
    }

    return 0;
}

int clear_online()
{
    fd_online_map_t::iterator it = m_fd_online_map.begin();
    while (it != m_fd_online_map.end()) {
        c_online *p_online = it->second;
        delete p_online;
        it++;
    }

    m_fd_online_map.clear();
    m_id_online_map.clear();
    return 0;
}

void route_online_timer(void)
{
	static time_t last_time = 0;
    time_t cur_time = get_now_tv()->tv_sec;

	if (cur_time - last_time < ROUTE_ONLINE_TIMER_DUR) {
		return ;
	}
	last_time = cur_time;

    fd_online_map_t::iterator it = m_fd_online_map.begin();
    while (it != m_fd_online_map.end()) {
        c_online *p_online = it->second;
        p_online->clean_room(cur_time);
        p_online->clean_player(cur_time);
        it++;
    }
}

int traverse_online(online_func_t cb, void * param, uint8_t ignore_flag)
{
    fd_online_map_t::iterator it = m_fd_online_map.begin();
    while (it != m_fd_online_map.end()) {
        c_online * p_online = it->second;
		int ret = cb(p_online, param);
		if (!ignore_flag) {
			if (ret) {
				return ret;
			}
		}
		it++;
	}

    return 0;
}

c_online::~c_online()
{
    DEBUG_TLOG("ONLINE_UNINITED_1_INIT, olid=%u", m_id);

    m_wait.uninit();
    DEBUG_TLOG("ONLINE_UNINITED_2_WAIT, olid=%u", m_id);

    m_match.uninit();
    DEBUG_TLOG("ONLINE_UNINITED_3_MATCH, olid=%u", m_id);

    m_go.uninit();
    DEBUG_TLOG("ONLINE_UNINITED_4_GO, olid=%u", m_id);

    m_room_pool.uninit();
    DEBUG_TLOG("ONLINE_UNINITED_5_ROOMPOOL, olid=%u", m_id);

    // 通知在本online，但进入的房间在其他online的玩家，非正常退出
    map_for_each(m_player_map, it) {

        c_player * p_player = it->second;

        c_online * p_online = p_player->get_online();
        c_room * p_room = p_player->get_room();

        if (p_online == this) {
            delete p_player;
            continue;
        }

        destroy_player(p_player);

        if (p_room->is_empty()) {
            p_online->destroy_room(p_room);

            DEBUG_TLOG("DESTROY_ROOM, olid=%u, roomid=%u",
					p_online->m_id, p_room->get_room_id());
        }
    }

    DEBUG_TLOG("ONLINE_UNINITED_6_FIN, olid=%u", m_id);
}

int c_online::clean_room(uint32_t cur_time)
{
    return 0;
}

int c_online::clean_player(uint32_t cur_time)
{
    return 0;
}
