/** 
 * ========================================================================
 * @file room.hpp
 * @brief 
 * @version 1.0
 * @date 2012-02-09
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_ROOM_H_2012_02_09
#define H_ROOM_H_2012_02_09


#include "proto.hpp"
#include "player.hpp"

enum room_mode_t
{
    // 自由pvp
    ROOM_MODE_PVP_FREE = 1,
    // 竞技pvp
    ROOM_MODE_PVP_COMPETE = 2,
};

// 房间状态
enum room_status_t
{
    // 房间状态未知
    ROOM_STATUS_NO       = 0,
    // 房间已建立，等待开始
    ROOM_STATUS_WAIT     = 1,
    // 房间对战进行中
    ROOM_STATUS_GO       = 2,
    // 房间正在匹配
    ROOM_STATUS_MATCH    = 3,
};

// 房间操作
enum room_action_t
{
    ROOM_ACTION_START_MATCH = 2,
    ROOM_ACTION_STOP_MATCH = 1,
};

/* TODO(zog): 座位能不能坐: 是状态, 座位有没有人: 用是否关联了 player 来判断;
 * 当然, 如果出现 '座位不能坐人, 但此时上面有人', 则应该是BUG,
 * 产生的原因可能是:
 * 	(1) 当人坐上去的时候, 没有注意座位不能做人;
 * 	(2) 或是在设置座位权限的时候没有注意上面有没有坐人;
 *
 */
// 房间里座位的状态
enum
{
    // 位置留空
    SEAT_STATUS_BLANK = 1,
    // 位置关闭
    SEAT_STATUS_CLOSE = 2,
    // 位置有人
    SEAT_STATUS_TAKEN = 3,
};




// 房间里的座位
struct room_seat_t
{
    // 座位状态
    uint8_t status;

    // 玩家
    c_player * player;

    room_seat_t()
    {
        init();
    }

    void init()
    {
        status = SEAT_STATUS_BLANK;
        player = NULL;
    }

    int is_blank()
    {
        return (SEAT_STATUS_BLANK == this->status);
    }

    int is_close()
    {
        return (SEAT_STATUS_CLOSE == this->status);
    }

    int is_taken()
    {
        return (SEAT_STATUS_TAKEN == this->status && NULL != player);
    }

    void set_player(c_player * p_player)
    {
        this->player = p_player;
        this->status = SEAT_STATUS_TAKEN;

        p_player->set_team(1);
        p_player->set_right(ROOM_RIGHT_NO);
        p_player->set_status(PLAYER_STATUS_READY);
    }

    void set_close()
    {
        init();
        this->status = SEAT_STATUS_CLOSE;
    }

    void set_blank()
    {
        init();
        this->status = SEAT_STATUS_BLANK;
    }

    uint8_t get_status()
    {
        return status;
    }
};

class c_room
{
	public:
		static bool is_valid_room_status(uint8_t status)
		{
			return (status == ROOM_STATUS_WAIT
					|| status == ROOM_STATUS_GO
					|| status == ROOM_STATUS_MATCH);
		}

    public:

        c_room(c_online *p_online, uint32_t index);

        virtual ~c_room() { }

        int init();

        void destroy()
        {
            room_seat_t *p_seat = NULL;
            c_player *p_player = NULL;
            for (uint8_t i = 0; i < m_max_seat_count; i++) {
                p_seat = get_seat(i);
                if (!p_seat->is_taken()) {
                    continue;
                }

                p_player = p_seat->player;
                p_player->leave_room();
            }

            init();
        }

        void create(uint32_t uid)
        {
            m_create_time = get_now_tv()->tv_sec;
            set_status(ROOM_STATUS_WAIT);
            m_creator_uid = uid;
        }

        c_online *get_online()
        {
            return m_online;
        }

        uint32_t get_online_id();

        void set_battle(c_battle *p_battle)
        {
            m_battle = p_battle;
        }

        c_battle *get_battle()
        {
            return m_battle;
        }

        const uint32_t get_room_id()
        {
            return m_room_id;
        }

        void set_status(uint8_t status)
        {
            m_status = status;
        }

        uint8_t get_status()
        {
            return m_status;
        }

        inline int check_status(uint8_t status)
        {
            return (m_status == status);
        }

        void set_name(const char *p_name)
        {
			/* TODO(zog): 脏词检测, '\0' 保护 */
            if (p_name) {
                snprintf(m_name, sizeof(m_name), "%s", p_name);
            }
        }

        const char * get_name()
        {
            return m_name;
        }

        const char * get_password()
        {
            return m_passwd;
        }

        bool need_passwd(void)
        {
            return (m_passwd[0] != '\0');
        }

        void set_passwd(const char *passwd)
        {
            if (passwd) {
                snprintf(m_passwd, sizeof(m_passwd), "%s", passwd);
            }
        }

        bool verify_passwd(const char *passwd)
        {
            TRACE_TLOG("verify_passwd, pass=[%s], try=[%s]", m_passwd, passwd);
            if (0 == strncmp(m_passwd, passwd, MAX_ROOM_PWD_LEN)) {
                return true;
            } else {
                return false;
            }
        }

        uint32_t get_creator_uid()
        {
            return m_creator_uid;
        }

        void set_map(uint32_t map_id)
        {
            m_map_id = map_id;
        }

        const uint32_t get_map_id()
        {
            return m_map_id;
        }

        virtual int set_mode(uint8_t mode)
        {
            m_mode = mode;
            return 0;
        }

        uint8_t get_mode()
        {
            return m_mode;
        }
        
        uint32_t get_create_time()
        {
            return m_create_time;
        }

        int diff_match_value(uint32_t match_value)
        {
            return abs((int)match_value - (int)this->m_match_value);
        }

        uint32_t get_match_value()
        {
            return m_match_value;
        }

        void set_match_value(uint32_t match_value)
        {
            m_match_value = match_value;
        }

        // 房间是否空
        bool is_empty()
        {
            return (0 == m_player_count);
        }

        // 房间是否满了
        bool is_full()
        {
            return (m_seat_count == m_player_count);
        }

        uint8_t get_player_count()
        {
            return m_player_count;
        }

        uint8_t get_max_seat_count()
        {
            return m_max_seat_count;
        }

        uint8_t get_seat_count()
        {
            return m_seat_count;
        }

        room_seat_t *get_seat(uint8_t seat)
        {
			/* QA(zog): 怎么会有 m_max_seat_count == 0 的情况? */
            if (0 == m_max_seat_count) {
                return m_seat;
            } else {
                seat %= m_max_seat_count;
                return (m_seat + seat);
            }
        }

        uint32_t get_host_id()
        {
            room_seat_t * host = get_seat(m_host_seat);

            if (!host->is_taken()) {
                return 0;
            }
            
            return host->player->m_id;
        }

		/* 房主称号是跟着 player 跑的, 不是跟着座位跑的, 这个接口完全搞错了 */
        virtual int set_host(uint8_t seat)
        {
            room_seat_t * p_seat = get_seat(seat);

            if (!p_seat->is_taken()) {
                return -1;
            }

            m_host_seat = seat;
            p_seat->player->set_host();

            return 0;
        }

		/* 房主称号是跟着 player 跑的, 不是跟着座位跑的, 这个接口完全搞错了 */
        uint8_t get_host_seat()
        {
            return m_host_seat;
        }


        int query_player(uint32_t user_id, uint8_t * p_seat, c_player ** pp_player);

        int traverse_player(player_func_t cb, void * param, uint8_t ignore_flag = 1);

        virtual int del_player(uint8_t seat, c_player ** pp_player = NULL);
        virtual int del_player(uint32_t user_id, c_player ** pp_player = NULL);

        // 添加到指定座位
        virtual int add_player(uint8_t seat, c_player * p_player, uint32_t team);
        // 不指定添加的座位，系统自动分配
        virtual int add_player(c_player * p_player, uint8_t * p_seat);

        virtual int change_seat(uint8_t old_seat, uint8_t new_seat);

        virtual int change_team(uint8_t old_seat, uint32_t team);

        virtual int kick_player(uint8_t kicker_seat, uint8_t kicked_seat);

        int close_seat(uint8_t seat);

        int open_seat(uint8_t seat);


        // 计算房间是否需要匹配
        virtual int calc_match_flag(uint8_t * p_flag)
        {
            if (p_flag) {
                *p_flag = m_match_flag;
            }
            return 0;
        }

        // 计算房间是否可以开始
        virtual int calc_start_flag(uint8_t * p_flag)
        {
            if (p_flag) {
                *p_flag = m_start_flag;
            }
            return 0;
        }

        // 检查房间的正确性
		/* TODO(zog): 这是个接口是干什么的 ??? */
        virtual bool check_validity(uint32_t cur_time);

    protected:

		/* TODO(zog): 据说没有匹配值... */
        // 重新计算房间的匹配值
        virtual void calc_match_value();

		/* TODO(zog): BUG - 当一边把座位都锁上后, 再有玩家想进来, 就提示错误码了 */
        // start_seat为搜索的起始位置，默认为0
        // end_seat为搜索的结束位置，默认为-1
        int find_first_blank(uint8_t * p_seat, uint8_t start_seat = 0, uint8_t end_seat = -1);

        // 找座位最靠前的玩家 (用于接替做房主的)
        int find_first_player(uint8_t * p_seat);

        // 统计指定队伍的玩家数
        uint8_t get_team_player_count(uint32_t team);

        // 房间里的玩家数
        uint8_t m_player_count;
        // 房间里座位数的上限
        uint8_t m_max_seat_count;
        // 房间里打开的座位数
        uint8_t m_seat_count;


        // 房间是否可以开始的标志
        uint8_t m_start_flag;
        // 房间是否需要匹配
        uint8_t m_match_flag;
        // 房主的座位号
        uint8_t m_host_seat;

        // 座位信息
        room_seat_t * m_seat;


    private:

        // 房间id
        uint32_t m_room_id;

        // 建立房间的online
        c_online * m_online;
        // 房间加入的battle
        c_battle * m_battle;

        // 房间创建的时间
        uint32_t m_create_time;

        // 房间名字
        char m_name[MAX_ROOM_NAME_LEN];

        // 房间密码
        char m_passwd[MAX_ROOM_PWD_LEN];

        // 房间状态，room_status_t
        uint8_t m_status;

        // 房间当前选定的地图id
		/* TODO(zog): 地图也要用个结构来表示 */
        uint32_t m_map_id;

		/* TODO(zog): 创建者用 (uid + role_tm + svrid) 三元组表示 */
        uint32_t m_creator_uid;

        // 战斗模式
        uint8_t m_mode;

        // 房间的匹配值
        uint32_t m_match_value;
};


typedef std::map<uint32_t, c_room *> room_map_t;
typedef std::list<c_room *> room_list_t;

typedef int (* room_func_t)(c_room * p_room, void * param);

// 默认complete为true，表示p的waitcmd会被清除
int send_to_room(c_room * p_room, c_player * p, Cmessage * p_out = NULL, bool complete = true);

// 房间里所有人的waitcmd都不会改变
int send_to_room(c_room * p_room, btlsw_proto_t * p_header, Cmessage * p_out = NULL);



class c_room_map
{
    public:
		c_room_map()
		{
			m_inited = 0;
		}

		int init(uint8_t status)
		{
            if (m_inited) {
                return 0;
            }

            m_map.clear();
            m_status = status;
            m_inited = 1;

            return 0;
        }

        int uninit(room_func_t p_room_func = NULL, void * param = NULL)
        {
            if (!m_inited) {
                return 0;
            }

            if (p_room_func) {
                room_map_t::iterator it = m_map.begin();
                for ( ; it != m_map.end(); it++) {
                    c_room * p_room = it->second;
                    p_room_func(p_room, param);
                }
            }

            m_map.clear();
            m_inited = 0;
            return 0;
        }

        int add_room(c_room *p_room)
        {
            if (!p_room) {
                return -1;
            }

            p_room->set_status(m_status);
            m_map.insert(pair<uint32_t, c_room *>(p_room->get_room_id(), p_room));

            return 0;
        }

        int del_room(c_room *p_room)
        {
			assert(p_room);

            p_room->set_status(ROOM_STATUS_NO);
            m_map.erase(p_room->get_room_id());

            return 0;
        }

        int get_room(uint32_t room_id, c_room **pp_room)
        {
            room_map_t::iterator it = m_map.find(room_id);
            if (it != m_map.end()) {
                *pp_room = it->second;
                return 0;
            }

            return -1;
        }

        int del_room(uint32_t room_id, c_room **pp_room)
        {
            c_room *p_room = NULL;
            if (get_room(room_id, &p_room)) {
                // 没有目标房间
                *pp_room = NULL;
                return -1;
            }

            *pp_room = p_room;
            del_room(p_room);

            return 0;
        }

        int find_room(room_func_t cb, void *param, c_room **pp_room)
        {
            room_map_t::iterator it = m_map.begin();
            c_room * p_room = NULL;
            c_room * least_room = NULL;
            int value = 0;
            int32_t least_value = INT_MAX;

            for ( ; it != m_map.end(); it++) {
                p_room = it->second;

                // cb返回-1，表示不匹配
                value = cb(p_room, param);
                if (value >= 0 && value < least_value) {
                    least_room = p_room;
                    least_value = value;
                }
            }

            if (!least_room) {
                *pp_room = NULL;
                return -1;
            } else {
                *pp_room = least_room;
                return 0;
            }
        }

        int traverse_room(room_func_t cb, void * param, bool ignore = true)
        {
            room_map_t::iterator it = m_map.begin();
            c_room * p_room = NULL;

            int ret = 0;
            while (it != m_map.end()) {
                p_room = it->second;
                ret = cb(p_room, param);
                if (!ignore && ret) {
					return ret;
                }

                it++;
            }

            return 0;
        }

        int clean_room(uint32_t cur_time)
        {
            return 0;
        }

    private:
        /* 表示是那种状态类型的 room_map */
        uint8_t m_status;

        /* room_map 是否已经被初始化 */
        uint8_t m_inited;

        /* room_map 具体数据 */
        room_map_t m_map;
};

#endif
