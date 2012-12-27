/*
 * =====================================================================================
 *
 *       Filename:  work_proc.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年12月07日 16时13分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_WORK_PROC_H_20111207
#define H_WORK_PROC_H_20111207

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/list.h>
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
}

#include <stdint.h>
#include <map>
#include <vector>
#include <set>

#include "../../common/message.h"
#include "../../common/constant.h"
#include "../../common/data_structure.h"
#include "../../common/utility/utility.h"

using namespace std;

#pragma pack(push)
#pragma pack(1)


/**
 * 匹配成功的未点击start按钮的用户信息
 * */
typedef struct
{
    uint32_t uid;               /**<@brief 用户自己的米米号 */
    uint32_t opponent_uid;      /**<@brief 对手的米米号，未匹配成功时取值0 */
    uint32_t enter_timestamp;  /**<@brief 匹配成功后，为匹配成功时的时间戳 */
    uint8_t is_start;           /**<@brief 是否点击了start按钮(0:未点击 1：已点击) */
    uint8_t waiting_to_delete;  /**<@brief 等待被删除(0:不删除 1：删除) */
}match_gamer_info_t;

/**
 * 在比赛中的用户信息
 * */
typedef struct
{
    uint32_t uid;               /**<@brief 用户自己的米米号 */
    uint32_t opponent_uid;      /**<@brief 对手的米米号 */
    uint32_t begin_timestamp;   /**<@brief 比赛开始时间 */
    uint8_t is_finished;        /**<@brief 用户答题是否结束(0:未结束 1：已结束) */
    uint8_t is_in_game;         /**<@brief 用户是否还在游戏中(0:不在 1：再) */
    uint16_t remain_time;       /**<@brief 比赛剩余时间 */
    uint16_t cur_score;         /**<@brief 用户当前得分 */
    uint8_t waiting_to_delete;  /**<@brief 等待从列表中清除 0:暂不清除 1：清除 */
}gaming_gamer_info_t;

    typedef struct
    {
        uint32_t seq_id;
        quest_type_t quest;
    }puzzle_quest_t;


    typedef struct
    {
        uint16_t next_time;
        uint16_t score;
        uint16_t remain_time;
    }imitate_answer_t;

    typedef struct
    {
        uint32_t self_uid;
        uint32_t robot_uid;
        uint32_t matched_timestamp;
    }match_robot_info_t;

    typedef struct
    {
        uint32_t self_uid;
        uint32_t robot_uid;
        uint16_t imitate_index;
        uint32_t begin_timestamp;
        uint8_t is_end;
        uint8_t is_robot_end;
        uint16_t remain_time;
        uint16_t cur_score;
        uint32_t trigger_robot_timestamp;
        uint16_t step_index;
    }gaming_with_robot_info_t;

/**
 * 用于检测匹配用户是否超时的定时器
 **/
typedef struct
{
    list_head_t timer_list;
}match_opp_timer_t;

/**
 * 用于检测是否点击start按钮的超时
 * */
typedef struct
{
    list_head_t timer_list;
}click_start_timer_t;


/**
 * 进入比赛后，检测是否结束的超时
 * */
typedef struct
{
    list_head_t timer_list;
}finished_game_timer_t;

typedef struct
{
    list_head_t timer_list;
}imitate_answer_timer_t;

typedef struct
{
    timer_struct_t *tmr;
    list_head_t timer_list;
}online_keepalive_timer_t;



#pragma pack(pop)


class c_work_proc
{
public:
    c_work_proc();
    ~c_work_proc();
    int init();
    int uninit();

public:
    int enter_two_puzzle(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    int start_two_puzzle(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    int answer_two_puzzle(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    int user_logout_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);

    int make_online_keepalive(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    int online_exit(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    int register_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    int online_closed(int fd);

private:
    /**
     * 三个用户定时器的回调函数
     * */
    static int wrap_check_waiting_user(void *owner, void *p_data);
    int check_waiting_user();

    static int wrap_check_starting_user(void *owner, void *p_data);
    int check_starting_user();

    static int wrap_check_finished_user(void *owner, void *p_data);
    int check_finished_user();

    static int wrap_imitate_user(void *owner, void *p_data);
    int imitate_user();

    static int online_keepalive_check(void *owner, void *p_data);

private:
    /* *
     *  * @brief check_val_len 检查长度是否正确
     *   *
     *    * @param real_len  实际应该的长度
     *     * @param recved_len 收到的数据长度
     *      *
     *       * @return
     *        */
    inline int check_val_len(uint32_t real_len, uint32_t recved_len)
    {
        if(real_len != recved_len)
        {
            ERROR_LOG("[msg_type:%u]recved_len(%u) is not equal to real_len(%u).", m_msg_type, recved_len, real_len);
            return ERR_MSG_LEN;
        }
        return 0;
    }


    int set_real_coins(uint32_t user_id, two_puzzle_result_t *p_result);
    int send_to_client(fdsession_t *fdsess, uint32_t seq_id, uint16_t msg_type);
    int pack(const void *val, uint32_t val_len);


    /**
     * @brief load_puzzle 载入题库
     *
     * @param puzzle_config_file
     *
     * @return
     */
    int load_puzzle(const char *puzzle_config_file);

    int load_robot(const char *robot_file);
    int load_imitate(const char *imitate_file);

    /**
     * @brief set_question 设置比赛题目
     *
     * @return
     */
    int set_question();

    fdsession_t* find_fdsess(uint32_t user_id)
    {
        map<uint32_t, int>::iterator iter = m_uid_id_map.find(user_id);
        if(iter == m_uid_id_map.end())
        {
            return NULL;
        }

        map<int, fdsession_t*>::iterator fd_iter = m_id_fdsess_map.find(iter->second);
        if(fd_iter == m_id_fdsess_map.end())
        {
            return NULL;
        }
        else
        {
            return fd_iter->second;
        }

        //map<uint32_t, fdsession_t*>::iterator iter = m_uid_fdsess_map.find(user_id);
        //if(iter == m_uid_fdsess_map.end())
        //{
        //    return NULL;
        //}
        //else
        //{
        //    return iter->second;
        //}
    }

private:

    uint8_t m_inited;

    two_puzzle_opponent_info_t m_waiting_match_user;                /**<@brief 正在等待二人益智游戏的用户 */
    uint32_t m_begin_waiting_timestamp;                     /**<@brief 开始等待的时间 */
    //map<uint32_t, fdsession_t*> m_uid_fdsess_map;           /**<@brief 存放用户连接信息的map(key:uid) value:online_id */
    map<uint32_t, int> m_uid_id_map;//存放uid对应的online_id信息
    map<int, fdsession_t*> m_id_fdsess_map; //存放online_id对应的连接信息
    map<int, int> m_fd_id_map;//记录fd对应的id信息

    map<uint32_t, match_gamer_info_t> m_matched_gamer_map;  /**<@brief 匹配成功的用户列表(key:uid)*/
    typedef map<uint32_t, match_gamer_info_t>::iterator m_matched_gamer_iterator;

    map<uint32_t, gaming_gamer_info_t> m_gaming_gamer_map;  /**<@brief 比赛中的用户列表(key:uid) */
    typedef map<uint32_t, gaming_gamer_info_t>::iterator m_gaming_gamer_iterator;

    match_opp_timer_t m_match_opp_timer;
    int m_match_opp_time;                                   /**<@brief 等待匹配用户的超时时间 */

    click_start_timer_t m_click_start_timer;
    int m_click_start_time;                                 /**<@brief 等待点击start按钮的超时时间 */

    finished_game_timer_t m_finished_game_timer;
    int m_finished_game_time;                               /**<@brief 等待比赛结束的超时时间 */

    map<uint32_t, uint16_t> m_day_restrict_map;             /**<@brief 记录二人益智游戏每日奖励的限制, 这里如果程序重启就会导致限制失效 */
    uint16_t m_day_restrict;                                /**<@brief 益智游戏每日奖励的上限 */
    int m_last_day;

    char m_send_buffer[MAX_SEND_PKG_SIZE];                  /**<@brief 发送缓冲 */
    uint32_t m_send_buffer_len;                             /**<@brief 发送缓冲中数据的长度 */
    svr_msg_header_t *m_p_send_header;                      /**<@brief 指向发送缓冲的指针 */


    uint32_t m_user_id;
    uint32_t m_errno;
    uint16_t m_msg_type;

    vector<puzzle_quest_t> m_puzzle_vec;               /**<@brief 题库 */
    int m_puzzle_total;                                     /**<@brief 题库中题目总量 */

    uint16_t m_quest_num;                                   /**<@brief 每次生成的比赛题目数量 */
    uint16_t m_init_total_time;                             /**<@brief 每场比赛的初始时间 */

//    map<uint32_t, quest_type_t> m_question_tmp_map;
    vector<quest_type_t> m_question_tmp_vec;                /**<@brief 临时存放每次比赛生成的比赛题目 */
    char m_question_buffer[MAX_SEND_PKG_SIZE];              /**<@brief 存放比赛题目的缓存 */
    uint32_t m_question_buffer_len;                         /**<@brief 比赛题目缓存的实际长度 */

    uint16_t m_win_reward_coins;                            /**<@brief 二人益智比赛胜者奖励的咕咚果 */
    uint16_t m_lose_reward_coins;                           /**<@brief 二人益智比赛失败者奖励的咕咚果 */
    uint16_t m_tie_reward_coins;                            /**<@brief 二人益智比赛平局奖励的咕咚果 */



    vector<two_puzzle_opponent_info_t> m_robot_info_vec;     /**<@brief 机器人的个人信息 */
    int m_robot_num;                                   /**<@brief 机器人的数量 */


    vector<vector<imitate_answer_t> > m_imitate_answer_vec;
    int m_imitate_num;                                  /**<@brief 模拟行为的个数 */


    map<uint32_t, match_robot_info_t> m_match_to_robot_map;

    map<uint32_t, gaming_with_robot_info_t> m_gaming_with_robot_map;

    imitate_answer_timer_t m_imitate_timer;
    int m_imitate_time;                               /**<@brief 模拟答题的时间时间 */


    online_keepalive_timer_t m_online_keepalive_tmr[MAX_ONLINE_NUM];           //检测各个online是否活跃的定时器

};

#endif//H_WORK_PROC_H_20111207
