/** 
 * ========================================================================
 * @file online.hpp
 * @brief 
 * @version 1.0
 * @date 2011-12-08
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_ONLINE_H_2011_12_08
#define H_ONLINE_H_2011_12_08


#include "room.hpp"
#include "pvp_room.hpp"

/* 每隔多久回收一下资源, 比如关掉的房间, 或走掉的人 */
#define ROUTE_ONLINE_TIMER_DUR		(10)

class c_online
{
    public:
        c_online(fdsession_t * fdsess, uint32_t online_id)
			: m_fdsess(fdsess), m_id(online_id)
        {
            uint32_t init_count = config_get_intval("init_pvp_room_num", 20000);
            if (0 != m_room_pool.init(this, init_count)) {
                ERROR_TLOG("init pvp room pool failed (%s)",
						m_room_pool.get_last_errstr());
                return;
            }

            if (0 != m_wait.init(ROOM_STATUS_WAIT)) {
                ERROR_TLOG("pvp wait init failed");
                return;
            }

            if (0 != m_match.init(ROOM_STATUS_MATCH)) {
                ERROR_TLOG("pvp match init failed");
                return;
            }

            if (0 != m_go.init(ROOM_STATUS_GO)) {
                ERROR_TLOG("pvp go init failed");
                return;
            }
        }

        ~c_online();

        c_pvp_room_pool *get_room_pool()
        {
            return &m_room_pool;
        }

        c_room_map * get_wait_map()
        {
            return &m_wait;
        }

        c_room_map * get_match_map()
        {
            return &m_match;
        }

        c_room_map *get_go_map()
        {
            return &m_go;
        }

        c_room_map *get_status_map(uint8_t status)
		{
            switch (status) {
			case ROOM_STATUS_WAIT:
				return get_wait_map();

			case ROOM_STATUS_GO:
				return get_go_map();

			case ROOM_STATUS_MATCH:
				return get_match_map();

			default:
				return 0;
            }
		}

        int get_room(uint32_t room_id, c_room **pp_room, c_room_map **pp_map)
        {
            c_room_map *p_map = &m_wait;
            if (0 != p_map->get_room(room_id, pp_room)) {
                p_map = &m_match;
                if (0 != p_map->get_room(room_id, pp_room)) {
                    p_map = &m_go;
                    if (0 != p_map->get_room(room_id, pp_room)) {
                        return -1;
                    }
                }
            }

            if (pp_map) {
                *pp_map = p_map;
            }

            return 0;
        }

        int set_room_status(c_room * p_room, uint8_t status)
        {
			if (!c_room::is_valid_room_status(status)) {
				WARN_TLOG("BUG! set_room_status invalid status: %hhu", status);
				return -1;
			}

            uint8_t old_status = p_room->get_status();
			if (old_status == status) {
				return 0;
			}

            c_room_map *p_old_map = get_status_map(old_status);
            c_room_map *p_map = get_status_map(status);

            p_old_map->del_room(p_room);
            p_map->add_room(p_room);

            return 0;
        }

        c_player *get_player(uint32_t uid)
        {
            player_map_t::iterator it = m_player_map.find(uid);
            if (it == m_player_map.end()) {
                return 0;
			}
			return it->second;
        }

        void add_player(c_player * p_player)
        {
            uint32_t player_id = p_player->m_id;
            m_player_map.insert(pair<uint32_t, c_player *>(player_id, p_player));
        }


        void del_player(c_player * p_player)
        {
            uint32_t player_id = p_player->m_id;
            m_player_map.erase(player_id);
        }

        int del_player(uint32_t player_id, c_player ** pp_player)
        {
            player_map_t::iterator it = m_player_map.find(player_id);
            if (it == m_player_map.end()) {
                return -1;
            }

            *pp_player = it->second;
            m_player_map.erase(it);
            return 0;
        }

        int clear_player()
        {
            player_map_t::iterator it = m_player_map.begin();
            while (it != m_player_map.end()) {
                c_player * p_player = it->second;
                delete p_player;
                it++;
            }

            m_player_map.clear();
            return 0;
        }

        int traverse_player(player_func_t cb, void * param, uint8_t ignore_flag = 1)
        {
            player_map_t::iterator it = m_player_map.begin();
            while (it != m_player_map.end()) {
                c_player *p_player = it->second;
                int ret = cb(p_player, param);
                if (!ignore_flag && ret) {
					return ret;
                }
                it++;
            }

            return 0;
        }

        // TODO(zog): 清理因不可控的原因造成的垃圾房间
        int clean_room(uint32_t time);

        // TODO(zog): 清理因不可控的原因造成的死用户
        int clean_player(uint32_t time);

        void destroy_room(c_room *p_room)
        {
            m_wait.del_room(p_room);
            m_match.del_room(p_room);
            m_go.del_room(p_room);

            c_pvp_room *p_pvp_room = dynamic_cast<c_pvp_room *>(p_room);
            m_room_pool.destroy_obj(p_pvp_room);

            p_room->destroy();
        }

    public:
        fdsession_t * const m_fdsess;
        const uint32_t m_id;

    private:
        // 等待中的房间列表
        c_room_map m_wait;
        // 匹配中的房间列表
        c_room_map m_match;
        // 游戏进行中的房间列表
        c_room_map m_go;

        // 房间
        c_pvp_room_pool m_room_pool;

        // 用户
        player_map_t m_player_map;
};


typedef int (* online_func_t)(c_online * p_online, void * param);
c_online *get_online_by_fd(int fd);
c_online *get_online(uint32_t id);
c_online *get_online(c_online * p, uint32_t id);
void add_online(c_online * p_online);
int del_online_by_fd(int online_fd);
int del_online(uint32_t online_id);
int clear_online();
//int route_online_timer(void * owner, void * data);
void route_online_timer(void);
int traverse_online(online_func_t cb, void * param, uint8_t ignore_flag = 1);

#endif
