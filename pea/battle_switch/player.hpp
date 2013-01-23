/** 
 * ========================================================================
 * @file player.hpp
 * @brief 
 * @version 1.0
 * @date 2012-02-08
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_PLAYER_H_2012_02_08
#define H_PLAYER_H_2012_02_08


#include <boost/intrusive/list.hpp>


#include <libtaomee++/memory/mempool.hpp>
#include "common.hpp"
#include "proto.hpp"



typedef boost::intrusive::list_member_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink> > object_hook_t;



// 房间里玩家的权力
enum {
    // 无特权
    ROOM_RIGHT_NO       = 0,
    // 踢人
    ROOM_RIGHT_KICK     = 1<<0,
    // 邀请玩家
    ROOM_RIGHT_INVITE   = 1<<1,
    // 赋予其他玩家权力
    ROOM_RIGHT_GRANT    = 1<<2,
    // 修改房间属性，名字、密码、地图等
    ROOM_RIGHT_SET_ATTR = 1<<3,
    // 开关房间座位
    ROOM_RIGHT_SET_SEAT = 1<<4,


    ROOM_RIGHT_END
};

// 房主的权力
#define ROOM_RIGHT_HOST     (ROOM_RIGHT_KICK | ROOM_RIGHT_INVITE | ROOM_RIGHT_GRANT | ROOM_RIGHT_SET_ATTR | ROOM_RIGHT_SET_SEAT)

// 玩家的阵营
/*
enum
{
    TEAM_NO    = 0,
    TEAM_ONE   = 1,
    TEAM_TWO   = 2,
};
*/

// 玩家的状态
enum {
    PLAYER_STATUS_UNREADY = 0,
    PLAYER_STATUS_READY = 1,
};


class c_player : public taomee::MemPool
{
    public:

        // user_id
        const uint32_t m_id;
        const uint32_t m_role_tm;
        const uint32_t m_server_id;

        // 包头的一些信息
        uint32_t m_ret;
        uint32_t m_waitcmd;
        uint32_t m_seq;

        // 缓存的报文序列
        GQueue * m_pkg_queue;
        object_hook_t m_wait_cmd_hook;


        // 上一次收到报文的时间戳
        uint32_t m_last_pkg_time;


    public:
		c_player(c_online *p_online, uint32_t uid, uint32_t svrid, uint32_t role_tm)
			: m_id(uid), m_role_tm(role_tm), m_server_id(svrid), m_online(p_online)
        {
            m_ret = 0;
            m_waitcmd = 0;
            m_seq = 0;
            m_last_pkg_time = 0;
            m_pkg_queue = g_queue_new();

            set_match_value(0);
            leave_room();
        }

        ~c_player()
        {
            cached_pkg_t *pkg = 0;
			while (1) {
                pkg = reinterpret_cast<cached_pkg_t *>(g_queue_pop_head(m_pkg_queue));
				if (!pkg) {
					break;
				}
                g_slice_free1(pkg->len, pkg);
			}
            g_queue_free(m_pkg_queue);

			/* 注意: m_wait_cmd_hook 是一个神奇的 boost hook,
			 * 这个 hook 会在宿主被 delete 的时候, 自动脱链,
			 * 所以此处没有显示的脱链操作; */
		}

        inline c_online * get_online()
        {
            return m_online;
        }

        inline void set_room(c_room * p_room)
        {
            m_room = p_room;
        }

        inline c_room * get_room()
        {
            return m_room;
        }

        inline void set_right(uint32_t right)
        {
            m_right = right;
        }

        inline uint32_t get_right()
        {
            return m_right;
        }

        inline void set_host()
        {
            set_status(PLAYER_STATUS_READY);
            set_right(ROOM_RIGHT_HOST);
        }

        inline int check_right(uint32_t right)
        {
            return (m_right & right);
        }

        inline void set_team(uint32_t team)
        {
            m_team = team;
        }

        inline uint32_t get_team()
        {
            return m_team;
        }

        inline uint32_t get_match_value()
        {
            return m_match_value;
        }

        inline void set_match_value(uint32_t match_value)
        {
            m_match_value = match_value;
        }

        inline int check_status(uint8_t status)
        {
            return (m_status == status);
        }

        inline void set_status(uint8_t status)
        {
            m_status = status;
        }

        inline uint8_t get_status()
        {
            return m_status;
        }

        inline void set_seat(uint8_t seat)
        {
            m_seat = seat;
        }

        inline uint8_t get_seat()
        {
            return m_seat;
        }

        inline void enter_room(c_room * p_room, uint8_t seat, uint32_t team)
        {
            set_room(p_room);
            set_seat(seat);
            set_right(ROOM_RIGHT_NO);
            set_status(PLAYER_STATUS_UNREADY);
            set_team(team);
        }

        inline void leave_room()
        {
            set_room(NULL);
            set_seat(0);
            set_right(ROOM_RIGHT_NO);
            set_status(PLAYER_STATUS_UNREADY);
            set_team(TEAM_NO);
        }

		inline void clear_waitcmd()
		{
			m_waitcmd = 0;
		}

        inline bool is_has_room()
        {
            return (NULL != m_room);
        }

    private:

        // 匹配值
        uint32_t m_match_value;

        // 所在的online
        c_online * const m_online;

        ////// 房间中的信息

        // 所处的房间
        c_room * m_room;
        // 房间中的座位编号
        uint8_t m_seat;
        // 权力
        uint32_t m_right;
        // 状态
        uint8_t m_status;
        // 阵营
        uint32_t m_team;
};


typedef std::map<uint32_t, c_player *> player_map_t;
typedef int (* player_func_t)(c_player * p_player, void * param);


void init_proto_head(void * buf, c_player * p, uint16_t cmd = 0);
int send_to_player(c_player * p, Cmessage * p_out = NULL, bool complete = true);
int send_to_player(c_player * p, btlsw_proto_t * p_header, Cmessage * p_out = NULL, bool complete =  true);
// 用指定的cmd，发包体为p_out的报文给p
int send_to_player(c_player * p, uint16_t cmd, Cmessage * p_out = NULL, bool complete = true);
c_player * alloc_player(c_online * p_online, uint32_t user_id, uint32_t online_id, uint32_t role_tm);
int destroy_player(c_player * p);
c_player* get_player_from_online(int fd, uint32_t uid);


#endif
