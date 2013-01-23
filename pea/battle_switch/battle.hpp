/** 
 * ========================================================================
 * @file battle.hpp
 * @brief 
 * @version 1.0
 * @date 2012-02-08
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_BATTLE_H_2012_02_08
#define H_BATTLE_H_2012_02_08


#include "common.hpp"
#include "room.hpp"


class c_battle
{
    public:


        c_battle(c_room * p_host)
        {
            m_host = p_host;
            m_room[0] = p_host;
            m_room_count = 1;
            p_host->set_battle(this);
        }

        inline void set_battle(uint32_t btl_id, int fd_idx)
        {
            m_btl_id = btl_id;
            m_fd_idx = fd_idx;
        }

        inline uint32_t get_btl_id()
        {
            return m_btl_id;
        }

        inline int get_fd_idx()
        {
            return m_fd_idx;
        }

        int add_room(c_room * p_room)
        {
            if (p_room == m_host)
            {
                return 0;
            }

            if (m_room_count >= MAX_ROOM_PER_BATTLE)
            {
                return -1;
            }

            m_room[m_room_count] = p_room;
            m_room_count++;
            p_room->set_battle(this);
            
            return 0;
        }

        int del_room(c_room * p_room)
        {
            return 0;
        }

        int traverse_room(room_func_t cb, void * param, uint8_t ignore_flag = 1)
        {
            for (uint8_t i = 0; i < m_room_count; i++) {
                if (m_room[i]) {
                    int ret = cb(m_room[i], param);
                    if (!ignore_flag && ret) {
						return ret;
                    }

                }
            }

            return 0;
        }

    private:

        // 战斗服务器id
        uint32_t m_btl_id;
        int m_fd_idx;


    public:
        // 房主所在房间
        c_room * m_host;
        // 战斗中的房间信息
        uint8_t m_room_count;
        c_room * m_room[MAX_ROOM_PER_BATTLE];
};


typedef std::list<c_battle *> battle_list_t;
typedef std::map<uint32_t, c_battle *> battle_map_t;

int send_to_battle(c_battle * p_battle, btlsw_proto_t * p_header, Cmessage * p_out = NULL);
int send_to_battle(c_battle * p_battle, c_player * p, Cmessage * p_out = NULL);

int new_battle(c_room * p_host, c_battle ** pp_battle);

int del_battle(c_battle * p_battle);



// class c_battle_list
// {
    // public:

        // int add_battle(c_room * p_host, c_battle ** pp_battle)
        // {
            // if (NULL == p_host)
            // {
                // return -1;
            // }

            // m_battle_list.push_back(p_battle);
            // return 0;
        // }

        // int find_battle(c_room * p_host, c_battle ** pp_battle)
        // {
            // if (NULL == p_host)
            // {
                // return -1;
            // }

            // battle_list_t::iterator it = m_battle_list.begin();
            // for ( ; it != m_battle_list.end(); it++)
            // {
                // c_battle * p_battle = *it;
                // if (p_battle->p_host == p_host)
                // {
                    // *pp_battle = p_battle;
                    // return 0;
                // }
            // }

            // return -1;

        // }

        // int del_battle(c_room * p_host)
        // {

            // battle_list_t::iterator it = m_battle_list.begin();
            // for ( ; it != m_battle_list.end(); it++)
            // {
                // c_battle * p_battle = *it;
                // if (p_battle->p_host == p_host)
                // {
                    // delete p_battle;
                    // m_battle_list.erase(it);
                    // break;
                // }
            // }

            // return 0;
        // }

        // int uninit()
        // {

            // battle_list_t::iterator it = m_battle_list.begin();
            // for ( ; it != m_battle_list.end(); it++)
            // {
                // c_battle * p_battle = *it;
            
                // delete p_battle;
            // }

            // m_battle_list.clear();

            // return 0;
            // // return ((c_room_map *)this)->uninit(p_room_func, param);
        // }

    // private:

        // battle_list_t m_battle_list;

// };

#endif

