/** 
 * ========================================================================
 * @file pvp_room.hpp
 * @brief 
 * @version 1.0
 * @date 2011-11-17
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_PVP_ROOM_H_2011_11_17
#define H_PVP_ROOM_H_2011_11_17


#include "proto.hpp"
#include "room.hpp"
#include "object_pool.hpp"


class c_pvp_room : public c_room
{
    public:
        c_pvp_room(c_online * p_online, uint32_t index)
			: c_room(p_online, index)
        {
            m_max_seat_count = MAX_PVP_ROOM_SEAT_NUM;
            m_seat_count = m_max_seat_count;
            m_seat = m_pvp_seat;
            init();
        }


        // 计算房间是否需要匹配
        int calc_match_flag(uint8_t * p_flag);

        int calc_start_flag(uint8_t * p_flag);

        int set_mode(uint8_t mode);

        int add_player(uint8_t seat, c_player * p_player, uint32_t team);

        // 不指定位置，自动安排
        int add_player(c_player * p_player, uint8_t * p_seat);

        int change_team(uint8_t old_seat, uint32_t new_team);

        bool check_validity(uint32_t cur_time);

    protected:


        // 位置和队伍的对应
        uint32_t calc_team(uint8_t seat);

        // 队伍座位区的起始号
        uint8_t team_start_seat(uint32_t team);

    private:

        room_seat_t m_pvp_seat[MAX_PVP_ROOM_SEAT_NUM];
};


typedef std::map<uint32_t, c_pvp_room *> pvp_room_map_t;
typedef std::list<c_pvp_room *> pvp_room_list_t;

typedef int (* pvp_room_func_t)(c_pvp_room * p_room, void * param);


class c_pvp_room_pool : public c_object_pool<c_pvp_room>
{
public:
	int init(c_online * p_online, uint32_t init_count)
	{
		m_online = p_online;
		if (0 != malloc_object(init_count)) {
			return -1;
		}

		return 0;
	}

	int uninit()
	{
		return 0;
	}


protected:
	int malloc_object(uint32_t count)
	{
		// 打算分配到的下标
		uint32_t new_index = m_index + count;
		while (m_index < new_index) {
			c_pvp_room * p_room = new c_pvp_room(m_online, m_index);
			if (p_room) {
				m_index++;
				m_free_list.push_back(p_room);
			}
		}

		return 0;
	}

private:
	c_online * m_online;
};


#endif
