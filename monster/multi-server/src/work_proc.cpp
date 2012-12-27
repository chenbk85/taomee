/*
 * =====================================================================================
 *
 *       Filename:  work_proc.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年12月08日 09时53分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <string.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <errno.h>
#include "work_proc.h"

c_work_proc g_work_proc;

c_work_proc::c_work_proc()
{
    m_inited = 0;
}

c_work_proc::~c_work_proc()
{
    uninit();
}

int c_work_proc::init()
{
    if(m_inited)
    {//已经初始化
        return -1;
    }

    memset(&m_waiting_match_user, 0, sizeof(m_waiting_match_user));

   // m_uid_fdsess_map.clear();
    m_uid_id_map.clear();
    m_id_fdsess_map.clear();
    m_matched_gamer_map.clear();
    m_gaming_gamer_map.clear();


    m_match_to_robot_map.clear();
    m_gaming_with_robot_map.clear();

    m_user_id = 0;
    m_errno = ERR_NO_ERR;
    m_msg_type = 0;

    memset(m_send_buffer, 0, sizeof(m_send_buffer));
    m_send_buffer_len = 0;
    m_p_send_header = (svr_msg_header_t*)m_send_buffer;

    m_match_opp_time = config_get_intval("match_opp_time", 10);
    if(m_match_opp_time <= 0)
    {
        ERROR_LOG("reading match_opp_time from config failed.");
        return -1;
    }

    m_click_start_time = config_get_intval("click_start_time", 30);
    if(m_click_start_time <= 0)
    {
        ERROR_LOG("reading click_start_time from config failed.");
        return -1;
    }

    m_finished_game_time = config_get_intval("finished_game_time", 60);
    if(m_finished_game_time <= 0)
    {
        ERROR_LOG("reading finished_game_time from config failed.");
        return -1;
    }

    m_imitate_time = config_get_intval("imitate_time", 1);
    if(m_imitate_time <= 0)
    {
        ERROR_LOG("reading imitate_time from config failed.");
        return -1;
    }

    //读取益智游戏题库
    char *puzzle_file = config_get_strval("puzzle_config_file");
    if(puzzle_file == NULL)
    {
        ERROR_LOG("reading puzzle_config_file failed.");
        return -1;
    }

    if(load_puzzle(puzzle_file) != 0)
    {
        ERROR_LOG("load puzzle failed.");
        return -1;
    }

    char *robot_file = config_get_strval("robot_file");
    if(robot_file == NULL)
    {
        ERROR_LOG("reading robot_file failed");
        return -1;
    }

    if(load_robot(robot_file) != 0)
    {
        ERROR_LOG("load robot file failed");
        return -1;
    }


    char *imitate_file = config_get_strval("imitate_file");
    if(imitate_file == NULL)
    {
        ERROR_LOG("reading imitate_file failed");
        return -1;
    }

    if(load_imitate(imitate_file) != 0)
    {
        ERROR_LOG("load imitate file failed");
        return -1;
    }


    //每场比赛生成的题目数量
    m_quest_num = config_get_intval("question_num", 0);

    if(m_quest_num < 0 || m_quest_num > m_puzzle_total)
    {
        ERROR_LOG("question num must between 0 and m_puzzle_total(%u).", m_puzzle_total);
        return -1;
    }

    // 没场比赛的初始总时间
    m_init_total_time = config_get_intval("init_total_time", 60);
    if(m_init_total_time <= 0)
    {
        ERROR_LOG("reading init_total_time failed.");
        return -1;
    }

    //读取比赛结果奖励
    m_win_reward_coins = config_get_intval("win_reward_coins", 10);
    if(m_win_reward_coins <= 0)
    {
        ERROR_LOG("reading win_reward_coins failed.");
        return -1;
    }


    m_lose_reward_coins = config_get_intval("lose_reward_coins", 6);
    if(m_lose_reward_coins <= 0)
    {
        ERROR_LOG("reading lose_reward_coins failed.");
        return -1;
    }

    m_tie_reward_coins = config_get_intval("tie_reward_coins", 8);
    if(m_tie_reward_coins <= 0)
    {
        ERROR_LOG("read tie_reward_coins failed.");
        return -1;
    }

    m_day_restrict = config_get_intval("two_puzzle_day_restrict", 50);
    if(m_day_restrict <= 0)
    {
        ERROR_LOG("read two_puzzle_day_restrict failed.");
        return -1;
    }

    m_last_day = timestamp2day(time(NULL));
    m_day_restrict_map.clear();

    //初始化三个定时器
    INIT_LIST_HEAD(&(m_match_opp_timer.timer_list));
    ADD_TIMER_EVENT(&m_match_opp_timer, &wrap_check_waiting_user, this, get_now_tv()->tv_sec + m_match_opp_time);

    INIT_LIST_HEAD(&(m_click_start_timer.timer_list));
    ADD_TIMER_EVENT(&m_click_start_timer, &wrap_check_starting_user, this, get_now_tv()->tv_sec + m_click_start_time);

    INIT_LIST_HEAD(&(m_finished_game_timer.timer_list));
    ADD_TIMER_EVENT(&m_finished_game_timer, &wrap_check_finished_user, this, get_now_tv()->tv_sec + m_finished_game_time);

    INIT_LIST_HEAD(&(m_imitate_timer.timer_list));
    ADD_TIMER_EVENT(&m_imitate_timer, &wrap_imitate_user, this, get_now_tv()->tv_sec + m_imitate_time);

    m_inited = 1;
    return 0;
}

int c_work_proc::uninit()
{
    if(!m_inited)
    {
        return -1;
    }
    memset(&m_waiting_match_user, 0, sizeof(m_waiting_match_user));

    m_day_restrict_map.clear();
    //m_uid_fdsess_map.clear();
    m_uid_id_map.clear();
    m_id_fdsess_map.clear();
    m_matched_gamer_map.clear();
    m_gaming_gamer_map.clear();

    m_match_to_robot_map.clear();
    m_gaming_with_robot_map.clear();

    m_user_id = 0;
    m_errno = ERR_NO_ERR;
    m_msg_type = 0;


    memset(m_send_buffer, 0, sizeof(m_send_buffer));
    m_send_buffer_len = 0;
    m_p_send_header = NULL;

    m_inited = 0;

    return 0;
}


int c_work_proc::register_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
	m_errno = ERR_NO_ERR;
	m_user_id = p_recv_header->user_id;
	m_msg_type = p_recv_header->msg_type;
	m_send_buffer_len = sizeof(svr_msg_header_t);

	online_register_switch_t *p_online_reg = (online_register_switch_t *)p_recv_header->body;
	map<int, fdsession_t*>::iterator iter = m_id_fdsess_map.find(p_online_reg->online_id);

	m_errno = check_val_len(sizeof(online_register_switch_t), body_len);
	if(m_errno != 0)
	{
		goto end;
	}


	if(iter != m_id_fdsess_map.end())
	{//正常情况时不应该走到这里面，如果不幸走到这里，说明有online重复发起注册请求
		WARN_LOG("[regiter_online]recv ip:%s port:%u fd:%d id:%u repeat register.", p_online_reg->online_ip, p_online_reg->online_port, fdsess->fd, p_online_reg->online_id);
	}
	else
	{
		INFO_LOG("[register_online]recv ip:%s port:%u fd:%d id:%u normal register.", p_online_reg->online_ip, p_online_reg->online_port, fdsess->fd, p_online_reg->online_id);

        int fd = fdsess->fd;
int online_id = p_online_reg->online_id;
		m_id_fdsess_map.insert(pair<int, fdsession_t*>(online_id, fdsess));
		m_fd_id_map[fd] = online_id;

        INIT_LIST_HEAD(&(m_online_keepalive_tmr[online_id - 1].timer_list));//online_id从1开始编号
        m_online_keepalive_tmr[online_id - 1].tmr = ADD_TIMER_EVENT(&m_online_keepalive_tmr[online_id - 1], c_work_proc::online_keepalive_check, (void*)(&fd), get_now_tv()->tv_sec + 70);

	}

end:
	return 0;

}

int c_work_proc::online_keepalive_check(void *owner, void *p_data)
{
    int *p_fd = (int*)p_data;
    WARN_LOG("keepalive check expired. online closed fd:%d ", *p_fd);
    close_client_conn(*p_fd);
    return 0;
}

int c_work_proc::make_online_keepalive(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
	m_errno = ERR_NO_ERR;
	m_user_id = p_recv_header->user_id;
	m_msg_type = p_recv_header->msg_type;
	m_send_buffer_len = sizeof(svr_msg_header_t);

    int online_id = m_fd_id_map[fdsess->fd];
	if(online_id < 1)
	{
			return 0;
	}
    mod_expire_time(m_online_keepalive_tmr[online_id - 1].tmr, get_now_tv()->tv_sec + 70 );
    return 0;
}

int c_work_proc::online_closed(int fd)
{
    int online_id = m_fd_id_map[fd];
    m_fd_id_map.erase(fd);
    if(online_id == 0)
    {
        return 0;
    }

    map<int, fdsession_t*>::iterator iter = m_id_fdsess_map.find(online_id);
    if(iter != m_id_fdsess_map.end())
    {
        INFO_LOG("[online_closed]fd:%d id:%u closed connnection.", fd, online_id);

        m_online_keepalive_tmr[online_id - 1].tmr = 0;
        REMOVE_TIMERS(&m_online_keepalive_tmr[online_id - 1]);

        m_id_fdsess_map.erase(iter);
    }
    else
    {
        WARN_LOG("[online_closed]Not found fd:%d online_id:%u in online_map.", fd, online_id);
    }
    return 0;
}

int c_work_proc::online_exit(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    online_closed(fdsess->fd);
    return 0;
}


int c_work_proc::enter_two_puzzle(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_user_id = p_recv_header->user_id;
    m_msg_type = p_recv_header->msg_type;

    m_send_buffer_len = sizeof(svr_msg_header_t);

    m_errno = check_val_len(sizeof(two_gamer_info_t), body_len);
    if(m_errno != 0)
    {
        goto end;
    }

    if(m_waiting_match_user.uid == m_user_id)
    {//非法用户 重复进入游戏
        ERROR_LOG("user %u enter two puzzle repeatly.", m_user_id);
        m_errno = ERR_REPEAT_ENTER_TWO_PUZZLE;
        goto end;
    }
    else if(m_matched_gamer_map.find(m_user_id) != m_matched_gamer_map.end())
    {
        WARN_LOG("user %u in start list", m_user_id);
        if(m_matched_gamer_map[m_user_id].waiting_to_delete == 1)
        {
            m_matched_gamer_map.erase(m_user_id);
        }
        else
        {
            ERROR_LOG("user %u in matched list ,enter two puzzle repeatly.", m_user_id);
            m_errno = ERR_REPEAT_ENTER_TWO_PUZZLE;
            goto end;
        }

    }
    else if(m_gaming_gamer_map.find(m_user_id) != m_gaming_gamer_map.end())
    {
        WARN_LOG("user %u in gaming", m_user_id);
        if(m_gaming_gamer_map[m_user_id].waiting_to_delete == 1)
        {
            m_gaming_gamer_map.erase(m_user_id);
        }
        else
        {
            ERROR_LOG("user %u in gaming ,enter two puzzle repeatly.", m_user_id);
            m_errno = ERR_REPEAT_ENTER_TWO_PUZZLE;
            goto end;
        }
    }
    else if(m_match_to_robot_map.find(m_user_id) != m_match_to_robot_map.end())
    {
        ERROR_LOG("user %u in match_to_robot list", m_user_id);
        m_errno = ERR_REPEAT_ENTER_TWO_PUZZLE;
        goto end;
    }
    else if(m_gaming_with_robot_map.find(m_user_id) != m_gaming_with_robot_map.end())
    {
        ERROR_LOG("user %u in gaming_with_robot map", m_user_id);
        m_errno = ERR_REPEAT_ENTER_TWO_PUZZLE;
        goto end;

    }

    {
       // if(m_uid_fdsess_map.find(m_user_id) == m_uid_fdsess_map.end())
        if(m_uid_id_map.find(m_user_id) == m_uid_id_map.end())
        {
            map<int, int>::iterator fd_id_iter = m_fd_id_map.find(fdsess->fd);
            if(fd_id_iter == m_fd_id_map.end())
            {
                goto end;
            }
            m_uid_id_map.insert(pair<uint32_t, int>(m_user_id, fd_id_iter->second));
        }

        two_gamer_info_t *p_gamer_info = (two_gamer_info_t*)p_recv_header->body;

        uint8_t is_found = 0;
        if(m_waiting_match_user.uid == 0)
        {//第一个进入二人益智的用户，将其置为等待的用户

            DEBUG_LOG("Temperary no user is in puzzle , You(%u) need waiting for it.", m_user_id);

            m_waiting_match_user.uid = m_user_id;
            memcpy(m_waiting_match_user.uname, p_gamer_info->uname, 16);
            m_waiting_match_user.is_vip = p_gamer_info->is_vip;
            m_waiting_match_user.mon_id = p_gamer_info->monster_id;
            m_waiting_match_user.mon_main_color = p_gamer_info->monster_main_color;
            m_waiting_match_user.mon_exp_color = p_gamer_info->monster_exp_color;
            m_waiting_match_user.mon_eye_color = p_gamer_info->monster_eye_color;
            m_begin_waiting_timestamp = get_now_tv()->tv_sec;

            if((m_errno = pack((void *)&is_found, sizeof(uint8_t))) != 0 )
            {
                ERROR_LOG("[enter_two_puzzle]: send_buffer overflowed(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
                goto end;
            }
        }
        else
        {//与正在等待的用户进行匹配
            //生成题库
            DEBUG_LOG("match success. m_user_id:%u  VS match_user:%u", m_user_id, m_waiting_match_user.uid);

            is_found = 1;
            if((m_errno = pack((void *)&is_found, sizeof(uint8_t))) != 0 )
            {
                ERROR_LOG("[enter_two_puzzle]: send_buffer overflowed(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
                goto end;
            }

            if((m_errno = pack((void *)&m_waiting_match_user, sizeof(two_puzzle_opponent_info_t))) != 0 )
            {
                ERROR_LOG("[enter_two_puzzle]: send_buffer overflowed while packing userinfo(m_send_buffer_len:%u, pack_len:%lu).", m_send_buffer_len, sizeof(two_puzzle_opponent_info_t));
                goto end;
            }

            set_question();

            match_gamer_info_t user_info;
            user_info.uid = m_user_id;
            user_info.opponent_uid = m_waiting_match_user.uid;
            user_info.enter_timestamp = get_now_tv()->tv_sec;
            user_info.is_start = 0;
            user_info.waiting_to_delete = 0;

            //发送匹配成功信息给m_user_id
            if((m_errno = pack(m_question_buffer, m_question_buffer_len)) != 0)
            {
                  ERROR_LOG("[enter_two_puzzle]: send_buffer overflowed while pack puzzle question(m_send_buffer_len:%u, pack_len:%u).", m_send_buffer_len, m_question_buffer_len);
                  goto end;
            }
            DEBUG_LOG("send match success and question to user:%u", m_user_id);
            send_to_client(fdsess, p_recv_header->seq_id, p_recv_header->msg_type);
            //将用户加入匹配成功列表
            m_matched_gamer_map.insert(pair<uint32_t, match_gamer_info_t>(m_user_id, user_info));

            //推送匹配成功信息给waiting_user

            fdsession_t *p_fdsess = find_fdsess(m_waiting_match_user.uid);
            if(p_fdsess == NULL)
            {
                ERROR_LOG("[enter_two_puzzle]: fdsess of waiting_user %u is not found.", m_waiting_match_user.uid);
            }
            else
            {
                DEBUG_LOG("send match success and question to waiting_user:%u", m_waiting_match_user.uid);
                m_errno = ERR_NO_ERR;
                //等待者
                user_info.opponent_uid = m_user_id;
                m_user_id = m_waiting_match_user.uid;
                m_send_buffer_len = sizeof(svr_msg_header_t);
                user_info.uid = m_user_id;

                m_waiting_match_user.uid = user_info.opponent_uid;
                memcpy(m_waiting_match_user.uname, p_gamer_info->uname, 16);
                m_waiting_match_user.is_vip = p_gamer_info->is_vip;
                m_waiting_match_user.mon_id = p_gamer_info->monster_id;
                m_waiting_match_user.mon_main_color = p_gamer_info->monster_main_color;
                m_waiting_match_user.mon_exp_color = p_gamer_info->monster_exp_color;
                m_waiting_match_user.mon_eye_color = p_gamer_info->monster_eye_color;

                if((m_errno = pack((void *)&is_found, sizeof(uint8_t))) != 0 )
                {
                    ERROR_LOG("[enter_two_puzzle]: send_buffer overflowed to waitinguser(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
                    goto end;
                }

                if((m_errno = pack((void *)&m_waiting_match_user, sizeof(two_puzzle_opponent_info_t))) != 0 )
                {
                    ERROR_LOG("[enter_two_puzzle]: send_buffer overflowed while packing userinfo to waitinguser(m_send_buffer_len:%u, pack_len:%lu).", m_send_buffer_len, sizeof(two_puzzle_opponent_info_t));
                    goto end;
                }


                if((m_errno = pack(m_question_buffer, m_question_buffer_len)) != 0)
                {
                      ERROR_LOG("[enter_two_puzzle]: send_buffer overflowed while pack puzzle questioni to waitinguser(m_send_buffer_len:%u, pack_len:%u).", m_send_buffer_len, m_question_buffer_len);
                }

                send_to_client(p_fdsess, 0, svr_msg_multi_online_match);
                //将对手加入匹配成功列表
                m_matched_gamer_map.insert(pair<uint32_t, match_gamer_info_t>(m_user_id, user_info));
            }

            //将waiting_user清0
            memset(&m_waiting_match_user, 0, sizeof(two_puzzle_opponent_info_t));
            m_begin_waiting_timestamp = 0;
            return 0;

        }
    }

end:
    return send_to_client(fdsess, p_recv_header->seq_id, p_recv_header->msg_type);
}

int c_work_proc::start_two_puzzle(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_user_id = p_recv_header->user_id;
    m_msg_type = p_recv_header->msg_type;

    m_send_buffer_len = sizeof(svr_msg_header_t);


    m_matched_gamer_iterator self_iter = m_matched_gamer_map.find(m_user_id);
    if(self_iter == m_matched_gamer_map.end())
    {//未匹配的用户
        map<uint32_t, match_robot_info_t>::iterator robot_iter = m_match_to_robot_map.find(m_user_id);
        if(robot_iter == m_match_to_robot_map.end())
        {
            ERROR_LOG("[start_two_puzzle]:Not matched user(%u).", m_user_id);
            goto end;
        }
        else
        {//与机器人匹配的用户
            uint8_t opponent_status = opp_started; //对手已经开始
            if((m_errno = pack(&opponent_status, sizeof(uint8_t))) != 0)
            {
                ERROR_LOG("[2、start_two_puzzle]: send_buffer overflowed while pack opponent_status(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
                goto end;
            }

            //将用户从匹配列表移到比赛列表中
            //
            uint16_t imitate_index = uniform_rand(0, m_imitate_num - 1);
            gaming_with_robot_info_t gaming_robot;
            gaming_robot.self_uid = m_user_id;
            gaming_robot.robot_uid = robot_iter->second.robot_uid;
            gaming_robot.imitate_index = imitate_index;
            gaming_robot.begin_timestamp = time(NULL);
            gaming_robot.is_end = 0;
            gaming_robot.is_robot_end = 0;
            gaming_robot.remain_time = m_init_total_time;
            gaming_robot.cur_score = 0;
            gaming_robot.trigger_robot_timestamp = gaming_robot.begin_timestamp + m_imitate_answer_vec[imitate_index][0].next_time;
            gaming_robot.step_index = 0;

            m_gaming_with_robot_map.insert(pair<uint32_t, gaming_with_robot_info_t>(m_user_id, gaming_robot));
            m_match_to_robot_map.erase(m_user_id);

            send_to_client(fdsess, p_recv_header->seq_id, p_recv_header->msg_type);
        }
    }
    else
    {
        //找到对手
        uint32_t opponent_uid = self_iter->second.opponent_uid;
        uint8_t opponent_status = opp_not_start; //默认取值对手未开始
        m_matched_gamer_iterator opponent_iter = m_matched_gamer_map.find(opponent_uid);
        if(opponent_iter == m_matched_gamer_map.end())
        {//对手信息不存在
            opponent_status = opp_leaved; //对手已离开
            if((m_errno = pack(&opponent_status, sizeof(uint8_t))) != 0)
            {
                ERROR_LOG("[1、start_two_puzzle]: send_buffer overflowed while pack opponent_status(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
            }
            m_matched_gamer_map.erase(self_iter);//added by henry at 2012-2-3 11:14 对手已经离开，将用户自己从匹配列表中删去
            goto end;
        }

        if(opponent_iter->second.is_start == 0)
        {
            opponent_status = opp_not_start; //对手尚未开始
            self_iter->second.is_start = 1;  //将用户自己置为已开始
        }
        else
        {
            opponent_status = opp_started; //对手已经开始
        }

        if((m_errno = pack(&opponent_status, sizeof(uint8_t))) != 0)
        {
            ERROR_LOG("[2、start_two_puzzle]: send_buffer overflowed while pack opponent_status(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
            goto end;
        }

        send_to_client(fdsess, p_recv_header->seq_id, p_recv_header->msg_type);

        if(opponent_status == opp_started)
        {//通知对手进入游戏
            //将用户加入在游戏中的列表,同时从等待列表中删除

            gaming_gamer_info_t gaming_gamer;
            gaming_gamer.uid = m_user_id;
            gaming_gamer.opponent_uid = opponent_uid;
            gaming_gamer.begin_timestamp = get_now_tv()->tv_sec;
            gaming_gamer.is_finished = 0;
            gaming_gamer.is_in_game = 1;
            gaming_gamer.remain_time = m_init_total_time;
            gaming_gamer.cur_score = 0;
            gaming_gamer.waiting_to_delete = 0;

            m_gaming_gamer_map.insert(pair<uint32_t, gaming_gamer_info_t>(m_user_id, gaming_gamer));
            m_matched_gamer_map.erase(m_user_id);

            opponent_status = enter_game;//进入游戏
            fdsession_t *p_fdsess = find_fdsess(opponent_uid);

            gaming_gamer.uid = opponent_uid;
            gaming_gamer.opponent_uid = m_user_id;

            if(p_fdsess != NULL)
            {
                DEBUG_LOG("send enter game info to waiting_user:%u", opponent_uid);
                m_errno = ERR_NO_ERR;
                m_user_id = opponent_uid;
                m_send_buffer_len = sizeof(svr_msg_header_t);
                if((m_errno = pack(&opponent_status, sizeof(uint8_t))) != 0)
                {
                    ERROR_LOG("[start_two_puzzle]: send_buffer overflowed while pack start_satus(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
                }
                send_to_client(p_fdsess, 0, svr_msg_multi_online_start);
            }
            else
            {
                ERROR_LOG("[start_two_puzzle]: fdsess of opponent %u is not found.", opponent_uid);
                gaming_gamer.is_finished = 1;
                gaming_gamer.is_in_game = 0;
                gaming_gamer.remain_time = 0;
            }

            m_gaming_gamer_map.insert(pair<uint32_t, gaming_gamer_info_t>(opponent_uid, gaming_gamer));
            m_matched_gamer_map.erase(opponent_uid);

        }

    }
    return 0;

end:
    return send_to_client(fdsess, p_recv_header->seq_id, p_recv_header->msg_type);
}

int c_work_proc::answer_two_puzzle(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_user_id = p_recv_header->user_id;
    m_msg_type = p_recv_header->msg_type;

    m_send_buffer_len = sizeof(svr_msg_header_t);

    m_errno = check_val_len(sizeof(answer_question_request_t), body_len);
    if(m_errno != 0)
    {
        goto end;
    }

    {
        answer_question_request_t *p_answer = (answer_question_request_t*)(p_recv_header->body);
        INFO_LOG("user %u answer: %u %u %u", m_user_id, p_answer->is_corrent, p_answer->is_end, p_answer->remain_time);

        m_gaming_gamer_iterator self_iter = m_gaming_gamer_map.find(m_user_id);
        if(self_iter == m_gaming_gamer_map.end() || self_iter->second.is_in_game == 0)
        {//检查用户自己是否处于游戏状态中
            map<uint32_t, gaming_with_robot_info_t>::iterator robot_iter = m_gaming_with_robot_map.find(m_user_id);
            if(robot_iter == m_gaming_with_robot_map.end())
            {
                ERROR_LOG("[answer_two_puzzle]:User %u not in gaming status.", m_user_id);
                m_errno = ERR_NOT_IN_GAMING_USER;
                goto end;
            }
            else
            {//与机器人对战
                KINFO_LOG(m_user_id, "user is in gaming with robot");
                if(robot_iter->second.is_end == 0)
                {//确保用户答题未结束
                    if(p_answer->is_corrent == 0)
                    {//答题正确
                        robot_iter->second.cur_score += 1;
                    }

                    if(p_answer->is_end == 1)
                    {//用户答题结束
                        robot_iter->second.is_end = 1;
                        robot_iter->second.remain_time = 0;
                    }
                    else
                    {
                        if(p_answer->remain_time > robot_iter->second.remain_time)
                        {//发过来的剩余时间比服务器记录的时间大，置为结束答题
                            robot_iter->second.is_end = 1;
                            robot_iter->second.remain_time = 0;
                        }
                        else
                        {
                            robot_iter->second.remain_time = p_answer->remain_time;
                        }
                    }

                    uint8_t answer_status = self_not_finished;
                    if(robot_iter->second.is_end == 1 && robot_iter->second.is_robot_end == 0)
                    {//用户答题结束，机器人答题未结束
                        answer_status = opp_not_finished;
                    }
                    else if(robot_iter->second.is_end == 1 && robot_iter->second.is_robot_end == 1)
                    {//用户答题结束，机器人答题结束
                        answer_status = both_finished;
                    }
                    else
                    {
                        answer_status = self_not_finished;
                    }

                    if((m_errno = pack(&answer_status, sizeof(uint8_t))) != 0)
                    {
                        ERROR_LOG("[answer_two_puzzle_robot]: send_buffer overflowed while pack answer_satus(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
                        goto end;
                    }

                    if(answer_status == both_finished)
                    {//彼此豆结束
                        two_puzzle_result_t user_result;

                        uint16_t imitate_index = robot_iter->second.imitate_index;
                        uint16_t step_index = robot_iter->second.step_index;
                        uint16_t robot_score = m_imitate_answer_vec[imitate_index][step_index + 1].score;

                        user_result.self_score = robot_iter->second.cur_score;
                        user_result.opp_score = robot_score;

                        if(user_result.self_score > user_result.opp_score)
                        {//自己胜利
                            user_result.result = two_puzzle_win;
                            user_result.rewards_coins = m_win_reward_coins;
                        }
                        else if(user_result.self_score < user_result.opp_score)
                        {//对手胜利
                            user_result.result = two_puzzle_lose;
                            user_result.rewards_coins = user_result.self_score == 0 ? 0 : m_lose_reward_coins;
                        }
                        else
                        {//平局
                            user_result.result = two_puzzle_tie;
                            user_result.rewards_coins = user_result.self_score == 0 ? 0 : m_tie_reward_coins;
                        }


                        //重新设置奖励，查看是否超过每日限制
                        set_real_coins(m_user_id, &user_result);

                        if((m_errno = pack(&user_result, sizeof(two_puzzle_result_t))) != 0)
                        {
                            ERROR_LOG("[answer_two_puzzle]: send_buffer overflowed while pack user_result(m_send_buffer_len:%u, pack_len:%lu).", m_send_buffer_len, sizeof(two_puzzle_result_t));
                        }

                        INFO_LOG(" normal send result to user %u:(自己得分:%u 机器人得分%u 结果：%u 奖励:%u 是否达到每日限制:%u) ", m_user_id, user_result.self_score, user_result.opp_score, user_result.result, user_result.rewards_coins, user_result.is_restrict );
                        m_gaming_with_robot_map.erase(m_user_id);
                    }

                    send_to_client(fdsess, p_recv_header->seq_id, p_recv_header->msg_type);
                }
            }
            return 0;
        }

        if(self_iter->second.is_finished == 1)
        {//已经结束答题的用户又发包过来了
            ERROR_LOG("[answer_two_puzzle]: User %u has finished. Should not send pkg here.", m_user_id);
            m_errno = ERR_HAVE_FINISHED;
            goto end;
        }

        m_gaming_gamer_iterator opponent_iter = m_gaming_gamer_map.find(self_iter->second.opponent_uid);

        if(opponent_iter == m_gaming_gamer_map.end())
        {//如果走到这里，应该时程序逻辑错误
            ERROR_LOG("[answer_two_puzzle]: It's not possible, User %u s opponent %u is not in gaming.", m_user_id, self_iter->second.opponent_uid);
            m_errno = ERR_NOT_FOUND_MATCHER;
            goto end;
        }

        uint8_t answer_status = p_answer->is_end == 0 ? self_not_finished : opp_not_finished;
        self_iter->second.is_finished = p_answer->is_end;

        INFO_LOG("user %u answer status %u", m_user_id, answer_status);
        if(p_answer->remain_time > self_iter->second.remain_time)
        {//如果发送过来的剩余时间比服务器记录的剩余时间还大，就将该用户置为结束状态
            ERROR_LOG(" user %u send  illegal pkg, remain_time(%u) > last_remain_time(%u).", m_user_id, p_answer->remain_time, self_iter->second.remain_time);
            answer_status = opp_not_finished;
            self_iter->second.is_finished = 1;
            self_iter->second.remain_time = 0;
        }
        else
        {
            self_iter->second.remain_time = p_answer->remain_time;
        }

        if(opponent_iter->second.is_finished == 1 && answer_status == opp_not_finished)
        {//对方也已结束答题
            INFO_LOG("opponent %u has ended", opponent_iter->second.uid);
            answer_status = both_finished;
        }

        if(p_answer->is_corrent == 0)
        {//答题正确
            self_iter->second.cur_score += 1;
        }


        if((m_errno = pack(&answer_status, sizeof(uint8_t))) != 0)
        {
            ERROR_LOG("[answer_two_puzzle]: send_buffer overflowed while pack answer_satus(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
            goto end;
        }

        fdsession_t *p_opp_fdsess = find_fdsess(opponent_iter->second.uid);

        if(answer_status == both_finished)
        {//双方都已结束
            INFO_LOG("both ended %u VS %u", m_user_id, opponent_iter->second.uid);
            two_puzzle_result_t self_result, opp_result;

            self_result.self_score = opp_result.opp_score = self_iter->second.cur_score;
            self_result.opp_score = opp_result.self_score = opponent_iter->second.cur_score;

            if(self_result.self_score > self_result.opp_score)
            {//自己胜利
                self_result.result = two_puzzle_win;
                self_result.rewards_coins = m_win_reward_coins;
                opp_result.result = two_puzzle_lose;
                opp_result.rewards_coins = self_result.opp_score == 0 ? 0 : m_lose_reward_coins;
            }
            else if(self_result.self_score < self_result.opp_score)
            {//对手胜利
                self_result.result = two_puzzle_lose;
                self_result.rewards_coins = self_result.self_score == 0 ? 0 : m_lose_reward_coins;
                opp_result.result = two_puzzle_win;
                opp_result.rewards_coins = m_win_reward_coins;
            }
            else
            {//平局
                self_result.result = opp_result.result = two_puzzle_tie;
                self_result.rewards_coins = opp_result.rewards_coins = self_result.self_score == 0 ? 0 : m_tie_reward_coins;
            }

            if(opponent_iter->second.is_in_game == 0)
            {//对手不在游戏中, 奖励没有
                opp_result.rewards_coins = 0;
            }

            //重新设置奖励，查看是否超过每日限制
            set_real_coins(m_user_id, &self_result);

            if((m_errno = pack(&self_result, sizeof(two_puzzle_result_t))) != 0)
            {
                ERROR_LOG("[answer_two_puzzle]: send_buffer overflowed while pack self_result(m_send_buffer_len:%u, pack_len:%lu).", m_send_buffer_len, sizeof(two_puzzle_result_t));
            }

            INFO_LOG("send result to user %u:(自己得分:%u 对手得分%u 结果：%u 奖励:%u 是否达到每日限制:%u) ", m_user_id, self_result.self_score, self_result.opp_score, self_result.result, self_result.rewards_coins, self_result.is_restrict );
            send_to_client(fdsess, p_recv_header->seq_id, p_recv_header->msg_type);
            m_gaming_gamer_map.erase(m_user_id);

            //推送比赛结果信息给对方
            if(p_opp_fdsess != NULL && opponent_iter->second.is_in_game == 1)
            {//对方的连接在并且对方也还在游戏中
                m_errno = ERR_NO_ERR;
                m_user_id = opponent_iter->second.uid;

                //重新设置奖励，查看是否超过每日限制
                set_real_coins(m_user_id, &opp_result);

                m_send_buffer_len = sizeof(svr_msg_header_t);
                if((m_errno = pack(&opp_result, sizeof(two_puzzle_result_t))) != 0)
                {
                    ERROR_LOG("[answer_two_puzzle]: send_buffer overflowed while pack opp_result(m_send_buffer_len:%u, pack_len:%lu).", m_send_buffer_len, sizeof(two_puzzle_result_t));
                }

                INFO_LOG("send result to user %u:(自己得分:%u 对手得分%u 结果：%u 奖励:%u 是否达到每日限制:%u) ", m_user_id, opp_result.self_score, opp_result.opp_score, opp_result.result, opp_result.rewards_coins, opp_result.is_restrict );
                send_to_client(p_opp_fdsess, 0, svr_msg_multi_online_result);
            }

            m_gaming_gamer_map.erase(opponent_iter);
        }
        else
        {//有一方尚未结束
            send_to_client(fdsess, p_recv_header->seq_id, p_recv_header->msg_type);

            //通知对方对手的成绩
            if(p_opp_fdsess != NULL && opponent_iter->second.is_in_game == 1)
            {
                m_errno = ERR_NO_ERR;
                m_user_id = opponent_iter->second.uid;
                m_send_buffer_len = sizeof(svr_msg_header_t);

                //生成包体
                opp_answer_info_t opp_answer;
                opp_answer.opp_cur_score = self_iter->second.cur_score;
                opp_answer.opp_remain_time = self_iter->second.remain_time;
                opp_answer.opp_is_end = self_iter->second.is_finished;
                INFO_LOG("send to %u(opp_score:%u, remain_time:%u, is_end:%u)", m_user_id, self_iter->second.cur_score, self_iter->second.remain_time, self_iter->second.is_finished );

                if((m_errno = pack(&opp_answer, sizeof(opp_answer_info_t))) != 0)
                {
                    ERROR_LOG("[answer_two_puzzle]: send_buffer overflowed while pack opp_answer(m_send_buffer_len:%u, pack_len:%lu).", m_send_buffer_len, sizeof(opp_answer_info_t));
                }

                send_to_client(p_opp_fdsess, 0, svr_msg_multi_online_answer);
            }
        }
        return 0;
    }
end:
    return send_to_client(fdsess, p_recv_header->seq_id, p_recv_header->msg_type);
}

int c_work_proc::user_logout_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_user_id = p_recv_header->user_id;
    m_msg_type = p_recv_header->msg_type;

    if(m_waiting_match_user.uid == m_user_id)
    {
        m_waiting_match_user.uid = 0;
        memset(&m_waiting_match_user, 0, sizeof(m_waiting_match_user));
        m_begin_waiting_timestamp = 0;
        INFO_LOG("User %u removed from waiting user.", m_user_id);

    }
    else if(m_matched_gamer_map.find(m_user_id) != m_matched_gamer_map.end())
    {
        m_matched_gamer_map.erase(m_user_id);
        INFO_LOG("User %u removed from matched user.", m_user_id);
    }
    else if(m_match_to_robot_map.find(m_user_id) != m_match_to_robot_map.end())
    {
        m_match_to_robot_map.erase(m_user_id);
        INFO_LOG("User %u remove from match_to_robot_map", m_user_id);
    }
    else if(m_gaming_with_robot_map.find(m_user_id) != m_gaming_with_robot_map.end())
    {
        m_gaming_with_robot_map.erase(m_user_id);
        INFO_LOG("User %u remove from gaming_with_robot_map", m_user_id);
    }
    else
    {
        m_gaming_gamer_iterator iter = m_gaming_gamer_map.find(m_user_id);
        if(iter != m_gaming_gamer_map.end())
        {
            iter->second.is_in_game = 0;
            iter->second.is_finished = 1;
            m_gaming_gamer_iterator opp_iter = m_gaming_gamer_map.find(iter->second.opponent_uid);
            if(opp_iter == m_gaming_gamer_map.end() || opp_iter->second.is_in_game == 0)
            {//两者都已不再游戏中，删除之
                INFO_LOG("user %u and %u both logout", m_user_id, iter->second.opponent_uid);
                m_gaming_gamer_map.erase(iter->second.opponent_uid);
                m_gaming_gamer_map.erase(iter);
            }
            else
            {
                INFO_LOG("user %u logout", m_user_id);
            }
        }
        else
        {
//            INFO_LOG("User %u is not in two_puzzle.", m_user_id);
        }
    }

    //m_uid_fdsess_map.erase(m_user_id);
    m_uid_id_map.erase(m_user_id);

    return 0;
}

int c_work_proc::load_imitate(const char *imitate_file)
{
    //加载xml
   if(imitate_file == NULL)
   {
        ERROR_LOG("imitate_file is NULL.");
        return -1;
   }

   xmlDocPtr doc = xmlParseFile(imitate_file);
   if(doc == NULL)
   {
        ERROR_LOG("parse xmlfile failed(%s).", strerror(errno));
        return -1;
   }

   xmlNodePtr root_node = xmlDocGetRootElement(doc);
   if(root_node == NULL)
   {
        ERROR_LOG("get root element of xml_file failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
   }

    if(xmlStrcmp(root_node->name, BAD_CAST"imitate_answer"))
    {
        ERROR_LOG("Root element is not imitate_answer.");
        xmlFreeDoc(doc);
        return -1;
    }
    xmlNodePtr child_node;
    child_node = root_node->xmlChildrenNode;
    child_node = child_node->next;
    while(child_node != NULL)
    {
        if(!xmlStrcmp(child_node->name, BAD_CAST"comment"))
        {
            child_node = child_node->next->next;
            continue;
        }

        if(xmlStrcmp(child_node->name, BAD_CAST"imitate"))
        {
            ERROR_LOG("Child node is not imitate%s", child_node->name);
            xmlFreeDoc(doc);
            return -1;
        }

        xmlNodePtr item_node = child_node->xmlChildrenNode;
        item_node = item_node->next;
        vector<imitate_answer_t> imitate_vec;
        while(item_node != NULL)
        {
            if(!xmlStrcmp(item_node->name, BAD_CAST"step"))
            {
                xmlChar *attr = NULL;
                imitate_answer_t imitate;

                if(xmlHasProp(item_node, (const xmlChar*)"next_time"))
                {
                    attr = xmlGetProp(item_node, (const xmlChar*)"next_time");
                    imitate.next_time = atoi((char*)attr);
                }
                else
                {
                    ERROR_LOG("parse imitate xml file failed, no next_time.");
                    xmlFreeDoc(doc);
                    return -1;
                }

                if(xmlHasProp(item_node, (const xmlChar*)"correct_num"))
                {
                    attr = xmlGetProp(item_node, (const xmlChar*)"correct_num");
                    imitate.score = atoi((char*)attr);
                }
                else
                {
                    ERROR_LOG("parse imitate xml file failed, no correct_num.");
                    xmlFreeDoc(doc);
                    return -1;
                }

                if(xmlHasProp(item_node, (const xmlChar*)"remain_time"))
                {
                    attr = xmlGetProp(item_node, (const xmlChar*)"remain_time");
                    imitate.remain_time = atoi((char*)attr);
                }
                else
                {
                    ERROR_LOG("parse imitate xml file failed, no remain_time.");
                    xmlFreeDoc(doc);
                    return -1;
                }
                imitate_vec.push_back(imitate);
            }

            item_node = item_node->next->next;
        }

        m_imitate_answer_vec.push_back(imitate_vec);
        child_node = child_node->next->next;
    }

    m_imitate_num = m_imitate_answer_vec.size();
    DEBUG_LOG("initial imitate num:%d", m_imitate_num);
    return 0;
}

int c_work_proc::load_robot(const char *robot_file)
{
    //加载xml
   if(robot_file == NULL)
   {
        ERROR_LOG("robot_file is NULL.");
        return -1;
   }

   xmlDocPtr doc = xmlParseFile(robot_file);
   if(doc == NULL)
   {
        ERROR_LOG("parse xmlfile failed(%s).", strerror(errno));
        return -1;
   }

   xmlNodePtr cur_node = xmlDocGetRootElement(doc);
   if(cur_node == NULL)
   {
        ERROR_LOG("get root element of xml_file failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
   }

    if(xmlStrcmp(cur_node->name, BAD_CAST"robots"))
    {
        ERROR_LOG("Root element is not robots.");
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    while(cur_node != NULL)
    {
        if(!xmlStrcmp(cur_node->name, BAD_CAST"robot"))
        {
            xmlChar *attr = NULL;
            two_puzzle_opponent_info_t robot_info;

            if(xmlHasProp(cur_node, (const xmlChar*)"uid"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"uid");
                robot_info.uid = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("parse puzzle xml file failed, no uid.");
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"uname"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"uname");
                strcpy(robot_info.uname, (char*)attr);
            }
            else
            {
                ERROR_LOG("parse puzzle xml file failed, no uname.");
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"is_vip"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"is_vip");
                robot_info.is_vip = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("parse puzzle xml file failed, no is_vip.");
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"mon_id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"mon_id");
                robot_info.mon_id = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("parse puzzle xml file failed, no mon_id.");
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"mon_main_color"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"mon_main_color");
                robot_info.mon_main_color = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("parse puzzle xml file failed, no mon_main_color.");
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"mon_exp_color"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"mon_exp_color");
                robot_info.mon_exp_color = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("parse puzzle xml file failed, no mon_exp_color.");
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"mon_eye_color"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"mon_eye_color");
                robot_info.mon_eye_color = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("parse puzzle xml file failed, no mon_eye_color.");
                xmlFreeDoc(doc);
                return -1;
            }

            m_robot_info_vec.push_back(robot_info);
        }

        cur_node = cur_node->next->next;
    }

    m_robot_num = m_robot_info_vec.size();
    return 0;
}

int c_work_proc::load_puzzle(const char *puzzle_config_file)
{
//加载xml
    if(puzzle_config_file == NULL)
    {
        ERROR_LOG("puzzle_config_file is NULL.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(puzzle_config_file);
    if(doc == NULL)
    {
        ERROR_LOG("parse xmlfile failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL)
    {
        ERROR_LOG("get root element of xml_file failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlStrcmp(cur_node->name, BAD_CAST"puzzles"))
    {
        ERROR_LOG("Root element is not puzzles.");
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    while(cur_node != NULL)
    {
        if(!xmlStrcmp(cur_node->name, BAD_CAST"puzzle"))
        {
            xmlChar *attr = NULL;
            puzzle_quest_t puzzle_quest;

            if(xmlHasProp(cur_node, (const xmlChar*)"id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"id");
                puzzle_quest.seq_id = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("parse puzzle xml file failed, no id.");
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"type"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"type");
                puzzle_quest.quest.quest_type = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("parse puzzle xml file failed, no type.");
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"puzzle_id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"puzzle_id");
                puzzle_quest.quest.quest_id = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("parse puzzle xml file failed, no puzzle_id.");
                xmlFreeDoc(doc);
                return -1;
            }

            m_puzzle_vec.push_back(puzzle_quest);
        }

        cur_node = cur_node->next->next;
    }

    m_puzzle_total = m_puzzle_vec.size();
    return 0;
}

int c_work_proc::set_question()
{
    //m_question_tmp_map.clear();
    m_question_tmp_vec.clear();
    memset(m_question_buffer, 0, sizeof(m_question_buffer));

    //生成足够的比赛题目
    set<int> index_set;
    while(m_question_tmp_vec.size() < m_quest_num)
    {
        int index = uniform_rand(0, m_puzzle_total - 1);
	if (index_set.find(index) != index_set.end())
	{
		continue;
	}
	index_set.insert(index);
        m_question_tmp_vec.push_back(m_puzzle_vec[index].quest);
       // m_question_tmp_map.insert(pair<uint32_t, quest_type_t>(m_puzzle_vec[index].seq_id, m_puzzle_vec[index].quest));
    }

    two_puzzle_compete_t *p_puzzle = (two_puzzle_compete_t*)(m_question_buffer);

    p_puzzle->compete_init_time = m_init_total_time;
    p_puzzle->question_num = m_quest_num;
    int idx = 0;
    //打包比赛题目
    //for(map<uint32_t, quest_type_t>::iterator iter = m_question_tmp_map.begin(); iter != m_question_tmp_map.end(); iter++)
    for(vector<quest_type_t>::iterator iter = m_question_tmp_vec.begin(); iter != m_question_tmp_vec.end(); iter++)
    {
        p_puzzle->quests[idx].quest_type = iter->quest_type;
        p_puzzle->quests[idx].quest_id = iter->quest_id;
	INFO_LOG("question_type:%u, question_id:%u", iter->quest_type, iter->quest_id);
        idx++;
        if(idx > m_quest_num)
        {
            break;
        }
    }

    m_question_buffer_len = sizeof(two_puzzle_compete_t) + m_quest_num * sizeof(quest_type_t);

    return 0;
}

                        //重新设置奖励，查看是否超过每日限制
int c_work_proc::set_real_coins(uint32_t user_id, two_puzzle_result_t *p_result)
{
    if(timestamp2day(time(NULL)) != m_last_day)
    {//不是同一天
        m_last_day = timestamp2day(time(NULL));
        m_day_restrict_map.clear();
    }

    map<uint32_t, uint16_t>::iterator iter = m_day_restrict_map.find(user_id);
    if(iter == m_day_restrict_map.end())
    {
        m_day_restrict_map.insert(pair<uint32_t, uint16_t>(user_id, p_result->rewards_coins));
        p_result->is_restrict = 0;
    }
    else
    {
        if(iter->second >= m_day_restrict)
        {//已经达到每日上限，奖励规0
            p_result->is_restrict = 1;
            p_result->rewards_coins = 0;
        }
        else if(iter->second + p_result->rewards_coins >= m_day_restrict)
        {//本次之后达到每日上限
            p_result->is_restrict = 1;
            p_result->rewards_coins = m_day_restrict - iter->second;
            iter->second = m_day_restrict;
        }
        else
        {//尚未达到每日上限
            p_result->is_restrict = 0;
            iter->second += p_result->rewards_coins;
        }
    }
    return 0;
}

int c_work_proc::pack(const void *val, uint32_t val_len)
{
    if(m_send_buffer_len + val_len > MAX_SEND_PKG_SIZE)
    {
        return ERR_SNDBUF_OVERFLOW;
    }

    memcpy(m_send_buffer + m_send_buffer_len, val, val_len);
    m_send_buffer_len += val_len;
    return 0;

}

int c_work_proc::send_to_client(fdsession_t *fdsess, uint32_t seq_id, uint16_t msg_type)
{
    if(m_errno != ERR_NO_ERR)
    {
        m_send_buffer_len = sizeof(svr_msg_header_t);
    }

    m_p_send_header->len = m_send_buffer_len;
    m_p_send_header->seq_id = seq_id;
    m_p_send_header->msg_type = msg_type;
    m_p_send_header->result = m_errno;
    m_p_send_header->user_id = m_user_id;

    if(0 != send_pkg_to_client(fdsess, m_send_buffer, m_send_buffer_len))
    {
            ERROR_LOG("send msg_type:%u to user:%u client failed.", msg_type, m_user_id);
    }
    return 0;
}



int c_work_proc::wrap_check_waiting_user(void *owner, void *p_data)
{
    c_work_proc *p_obj = (c_work_proc*)p_data;
    return p_obj->check_waiting_user();
}

int c_work_proc::check_waiting_user()
{
    if(m_waiting_match_user.uid != 0 && get_now_tv()->tv_sec - m_begin_waiting_timestamp > m_match_opp_time)
    {//等待已经超时,与机器人进行匹配
    	INFO_LOG("USER %u waiting timeout, match to robot", m_waiting_match_user.uid );

        m_errno = ERR_NO_ERR;
        m_user_id = m_waiting_match_user.uid;
        m_send_buffer_len = sizeof(svr_msg_header_t);

        fdsession_t *p_fdsess = find_fdsess(m_waiting_match_user.uid);

        if(p_fdsess == NULL)
        {
             ERROR_LOG("[check_waiting_user]: fdsess of waiting_user %u is not found.", m_waiting_match_user.uid);
             memset(&m_waiting_match_user, 0, sizeof(m_waiting_match_user));
             m_begin_waiting_timestamp = 0;
             return 0;
        }

        int robot_index = uniform_rand(0, m_robot_num - 1);
        uint8_t is_found = 1;//找到对手

        if((m_errno = pack((void *)&is_found, sizeof(uint8_t))) != 0 )
        {
             ERROR_LOG("[check_waiting_user]: send_buffer overflowed(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
             m_errno = ERR_SNDBUF_OVERFLOW;
        }

        two_puzzle_opponent_info_t opp_robot = m_robot_info_vec[robot_index];

        KINFO_LOG(m_user_id, "opp_robot_info:%u name:%s is_vip:%u mon_id:%u mon_main_color:%u mon_exp_color:%u mon_eye_color:%u", opp_robot.uid, opp_robot.uname, opp_robot.is_vip, opp_robot.mon_id, opp_robot.mon_main_color, opp_robot.mon_exp_color, opp_robot.mon_eye_color);

        if((m_errno = pack((void*)&opp_robot, sizeof(two_puzzle_opponent_info_t))) != 0)
        {
            ERROR_LOG("[check_waiting_user]: send_buffer overflowed while pack opp_robot(m_send_buffer_len:%u, pack_len:%lu).", m_send_buffer_len, sizeof(two_puzzle_opponent_info_t));
             m_errno = ERR_SNDBUF_OVERFLOW;
        }

        set_question();

        if((m_errno = pack(m_question_buffer, m_question_buffer_len)) != 0)
        {
            ERROR_LOG("[check_waiting_user]: send_buffer overflowed while pack puzzle question(m_send_buffer_len:%u, pack_len:%u).", m_send_buffer_len, m_question_buffer_len);
             m_errno = ERR_SNDBUF_OVERFLOW;
        }

        send_to_client(p_fdsess, 0, svr_msg_multi_online_match);
        memset(&m_waiting_match_user, 0, sizeof(m_waiting_match_user));
        m_begin_waiting_timestamp = 0;

        //将用户加入与机器人匹配成功列表
        match_robot_info_t match_user;
        match_user.self_uid = m_user_id;
        match_user.robot_uid = opp_robot.uid;
        match_user.matched_timestamp = time(NULL);
        m_match_to_robot_map.insert(pair<uint32_t, match_robot_info_t>(m_user_id, match_user));

        /** 之前做的不与机器人匹配，直接告知用户未找到对手
        m_errno = ERR_NO_ERR;
        m_user_id = m_waiting_match_user.uid;
        m_send_buffer_len = sizeof(svr_msg_header_t);

        fdsession_t *p_fdsess = find_fdsess(m_waiting_match_user.uid);

        if(p_fdsess == NULL)
        {
             ERROR_LOG("[check_waiting_user]: fdsess of waiting_user %u is not found.", m_waiting_match_user.uid);
             memset(&m_waiting_match_user, 0, sizeof(m_waiting_match_user));
             m_begin_waiting_timestamp = 0;
             return 0;
        }

        uint8_t is_found = 0;

        if((m_errno = pack((void *)&is_found, sizeof(uint8_t))) != 0 )
        {
             ERROR_LOG("[check_waiting_user]: send_buffer overflowed(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
        }

        send_to_client(p_fdsess, 0, svr_msg_multi_online_match);
        memset(&m_waiting_match_user, 0, sizeof(m_waiting_match_user));
        m_begin_waiting_timestamp = 0;
        */
    }

    ADD_TIMER_EVENT(&m_match_opp_timer, &wrap_check_waiting_user, this, get_now_tv()->tv_sec + m_match_opp_time);
    return 0;
}

int c_work_proc::wrap_check_starting_user(void *owner, void *p_data)
{
    c_work_proc *p_obj = (c_work_proc*)p_data;
    return p_obj->check_starting_user();
}

int c_work_proc::check_starting_user()
{
    m_errno = ERR_NO_ERR;
    m_send_buffer_len = sizeof(svr_msg_header_t);

    m_matched_gamer_iterator tmp_iter;
    m_matched_gamer_iterator opp_iter;
    m_matched_gamer_iterator cur_iter = m_matched_gamer_map.begin();
    for(; cur_iter != m_matched_gamer_map.end();)
    {
        opp_iter = m_matched_gamer_map.find(cur_iter->second.opponent_uid);
        if(opp_iter != m_matched_gamer_map.end() && cur_iter->second.is_start == 1)
        {//对手还在，未离开 并且用户自己已经点击start
            cur_iter++;
            continue;
        }

        //对手已经离开或者用户自己未点击start
        fdsession_t *p_fdsess = find_fdsess(cur_iter->second.uid);
        if(p_fdsess == NULL)
        {//用户自己的连接已经不在
            ERROR_LOG("[check_starting_user]:user %u fdsess is not found.", cur_iter->second.uid);
        }
        else
        {
        	m_user_id = cur_iter->second.uid;
            uint8_t user_status = self_unstarted_timeout;//用户自己长时间未点击start按钮

            if(opp_iter == m_matched_gamer_map.end())
            {//对手已经离开
                user_status = opp_unstarted_timeout;
                if((m_errno = pack((void *)&user_status, sizeof(uint8_t))) != 0 )
                {
                    ERROR_LOG("[check_starting_user]: send_buffer overflowed(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
                }
                send_to_client(p_fdsess, 0, svr_msg_multi_online_start);
            }
            else
            {//检测用户自己是否超时
                if(get_now_tv()->tv_sec - cur_iter->second.enter_timestamp > m_click_start_time)
                {//用户长时间未点击start按钮，超时
                    user_status = self_unstarted_timeout;
                    if((m_errno = pack((void *)&user_status, sizeof(uint8_t))) != 0 )
                    {
                        ERROR_LOG("[check_starting_user]: send_buffer overflowed(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
                    }

                    send_to_client(p_fdsess, 0, svr_msg_multi_online_start);

                    //超时，需要通知对手对方已超时
                    m_errno = ERR_NO_ERR;
                    m_user_id = opp_iter->second.uid;
                    m_send_buffer_len = sizeof(svr_msg_header_t);

                    opp_iter->second.waiting_to_delete = 1;

                    user_status = opp_unstarted_timeout;
                    p_fdsess = find_fdsess(m_user_id);
                    if(p_fdsess == NULL)
                    {
                        ERROR_LOG("[check_starting_user]: opponent user %u fdsess is not found.", m_user_id);
                    }
                    else
                    {
                        if((m_errno = pack((void *)&user_status, sizeof(uint8_t))) != 0 )
                        {
                            ERROR_LOG("[check_starting_user]: send_buffer overflowed while pack to oppoent(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
                        }
                        send_to_client(p_fdsess, 0, svr_msg_multi_online_start);
                    }
                }
                else
                {//用户自己未超时
                    cur_iter++;
                    continue;
                }
            }
        }

        //将超时用户从 start列表中清除
        tmp_iter = cur_iter;
        cur_iter++;
        m_matched_gamer_map.erase(tmp_iter);
    }//for

    //检测与机器人匹配的用户是否超时未点击start按钮
    map<uint32_t, match_robot_info_t>::iterator tmp_robot_iter;
    map<uint32_t, match_robot_info_t>::iterator iter = m_match_to_robot_map.begin();
    for(; iter != m_match_to_robot_map.end(); )
    {
        fdsession_t *p_fdsess = find_fdsess(iter->second.self_uid);
        if(p_fdsess == NULL)
        {//用户自己的连接已经不在
            ERROR_LOG("[check_starting_user_robot]:user %u fdsess is not found.", iter->second.self_uid);
            tmp_robot_iter = iter;
            iter++;
            m_match_to_robot_map.erase(tmp_robot_iter);
        }
        else
        {
            if(get_now_tv()->tv_sec - iter->second.matched_timestamp > m_click_start_time)
            {//超时用户
    		m_user_id = iter->second.self_uid;
                uint8_t user_status = self_unstarted_timeout;
                if((m_errno = pack((void *)&user_status, sizeof(uint8_t))) != 0 )
                {
                    ERROR_LOG("[check_starting_user_robot]: send_buffer overflowed while pack userstatus(m_send_buffer_len:%u, pack_len:1).", m_send_buffer_len);
                    m_errno = ERR_SNDBUF_OVERFLOW;
                }
                send_to_client(p_fdsess, 0, svr_msg_multi_online_start);
                tmp_robot_iter = iter;
                iter++;
                m_match_to_robot_map.erase(tmp_robot_iter);
            }
            else
            {
                iter++;
            }
        }
    }

    ADD_TIMER_EVENT(&m_match_opp_timer, &wrap_check_starting_user, this, get_now_tv()->tv_sec + m_click_start_time);
    return 0;
}

int c_work_proc::wrap_check_finished_user(void *owner, void *p_data)
{
    c_work_proc *p_obj = (c_work_proc*)p_data;
    return p_obj->check_finished_user();
}

int c_work_proc::check_finished_user()
{
    m_gaming_gamer_iterator opp_iter, tmp_iter;
    m_gaming_gamer_iterator cur_iter = m_gaming_gamer_map.begin();
    for(; cur_iter != m_gaming_gamer_map.end(); )
    {
        if(cur_iter->second.waiting_to_delete == 1)
        {
                tmp_iter = cur_iter;
                cur_iter++;
                m_gaming_gamer_map.erase(tmp_iter);
                continue;
        }

        if(cur_iter->second.is_finished == 1 || cur_iter->second.is_in_game == 0)
        {//答题已经结束 || 用户已经不在游戏中
            cur_iter++;
            continue;
        }

        if(get_now_tv()->tv_sec - cur_iter->second.begin_timestamp > m_finished_game_time)
        {//已经超时, 将其置为结束状态
            cur_iter->second.is_finished = 1;
            cur_iter->second.remain_time = 0;

            opp_iter = m_gaming_gamer_map.find(cur_iter->second.opponent_uid);
            if(opp_iter == m_gaming_gamer_map.end())
            {//对手不在列表中
                ERROR_LOG("[check_finished_user]: timeout user %u 's opponent %u not in gaming.", cur_iter->second.uid, cur_iter->second.opponent_uid);
                tmp_iter = cur_iter;
                cur_iter++;
                m_gaming_gamer_map.erase(tmp_iter);
            }
            else
            {
                if(opp_iter->second.is_finished == 1)
                {//对手已经结束答题,计算双方的奖励及战果

                    //下面的self_result代表超时者
                    two_puzzle_result_t self_result, opp_result;

                    self_result.self_score = opp_result.opp_score = cur_iter->second.cur_score;
                    self_result.opp_score = opp_result.self_score = opp_iter->second.cur_score;

                    if(self_result.self_score > self_result.opp_score)
                    {//自己胜利
                        self_result.result = two_puzzle_win;
                        self_result.rewards_coins = m_win_reward_coins;
                        opp_result.result = two_puzzle_lose;
                        opp_result.rewards_coins = self_result.opp_score == 0 ? 0 : m_lose_reward_coins;
                    }
                    else if(self_result.self_score < self_result.opp_score)
                    {//对手胜利
                        self_result.result = two_puzzle_lose;
                        self_result.rewards_coins = self_result.self_score == 0 ? 0 : m_lose_reward_coins;
                        opp_result.result = two_puzzle_win;
                        opp_result.rewards_coins = m_win_reward_coins;
                    }
                    else
                    {//平局
                        self_result.result = opp_result.result = two_puzzle_tie;
                        self_result.rewards_coins = opp_result.rewards_coins = self_result.self_score == 0 ? 0 : m_tie_reward_coins;
                     }

                    cur_iter->second.waiting_to_delete = 1;

                    fdsession_t *p_fdsess = find_fdsess(cur_iter->second.uid);
                    if(p_fdsess != NULL)
                    {
                        //推送结果给超时者
                        m_errno = ERR_NO_ERR;
                        m_user_id = cur_iter->second.uid;
                        m_send_buffer_len = sizeof(svr_msg_header_t);

                        //重新设置奖励，查看是否超过每日限制
                        set_real_coins(m_user_id, &self_result);

                        m_send_buffer_len = sizeof(svr_msg_header_t);
                        if((m_errno = pack(&self_result, sizeof(two_puzzle_result_t))) != 0)
                        {
                            ERROR_LOG("[check_finished_puzzle]: send_buffer overflowed while pack self_result(m_send_buffer_len:%u, pack_len:%lu).", m_send_buffer_len, sizeof(two_puzzle_result_t));
                        }

                        send_to_client(p_fdsess, 0, svr_msg_multi_online_result);
                    }

                    opp_iter->second.waiting_to_delete = 1;

                    p_fdsess = find_fdsess(opp_iter->second.uid);
                    if(p_fdsess != NULL)
                    {
                        //推送结果给对方
                        m_errno = ERR_NO_ERR;
                        m_user_id = opp_iter->second.uid;
                        m_send_buffer_len = sizeof(svr_msg_header_t);

                        //重新设置奖励，查看是否超过每日限制
                        set_real_coins(m_user_id, &opp_result);

                        m_send_buffer_len = sizeof(svr_msg_header_t);
                        if((m_errno = pack(&opp_result, sizeof(two_puzzle_result_t))) != 0)
                        {
                            ERROR_LOG("[check_finished_puzzle]: send_buffer overflowed while pack opp_result(m_send_buffer_len:%u, pack_len:%lu).", m_send_buffer_len, sizeof(two_puzzle_result_t));
                        }

                        send_to_client(p_fdsess, 0, svr_msg_multi_online_result);
                    }
                }
                cur_iter++;
            }
        }//超时处理结束
        else
        {
            cur_iter++;
        }
    }//循环结束

    ADD_TIMER_EVENT(&m_match_opp_timer, &wrap_check_finished_user, this, get_now_tv()->tv_sec + m_finished_game_time);
    return 0;
}


int c_work_proc::wrap_imitate_user(void *owner, void *p_data)
{
    c_work_proc *p_obj = (c_work_proc*)p_data;
    return p_obj->imitate_user();
}

int c_work_proc::imitate_user()
{
    map<uint32_t, gaming_with_robot_info_t>::iterator tmp_iter;
    map<uint32_t, gaming_with_robot_info_t>::iterator iter = m_gaming_with_robot_map.begin();

    while(iter != m_gaming_with_robot_map.end())
    {
        if(iter->second.is_end == 1)
        {//用户答题已经结束

        }
        else if(get_now_tv()->tv_sec - iter->second.begin_timestamp > m_finished_game_time)
        {//超时用户,将其置为结束状态 等待机器人答题结束
            iter->second.is_end = 1;
            iter->second.remain_time = 0;
        }

        uint16_t imitate_index = iter->second.imitate_index;
        uint16_t step_index = iter->second.step_index;
        m_errno = ERR_NO_ERR;
        m_user_id = iter->second.self_uid;
        m_send_buffer_len = sizeof(svr_msg_header_t);

        fdsession_t *p_fdsess = find_fdsess(iter->second.self_uid);
        if(p_fdsess == NULL)
        {
            tmp_iter = iter;
            iter++;
            m_gaming_with_robot_map.erase(tmp_iter);
            KWARN_LOG(m_user_id, "remove from m_gaming_with_robot_map for not found fdsess");
            continue;
        }

        if(iter->second.is_robot_end == 0)
        {//机器人答题未结束
            opp_answer_info_t opp_answer;

            if(get_now_tv()->tv_sec - iter->second.trigger_robot_timestamp > 0)
            {//机器人答题
                if(m_imitate_answer_vec[imitate_index][step_index + 1].remain_time == 0)
                {//机器人结束
                    iter->second.is_robot_end = 1;
                    if(iter->second.is_end == 1)
                    {//用户答题也已结束
                        two_puzzle_result_t user_result;

                        uint16_t robot_score = m_imitate_answer_vec[imitate_index][step_index + 1].score;

                        user_result.self_score = iter->second.cur_score;
                        user_result.opp_score = robot_score;

                        if(user_result.self_score > user_result.opp_score)
                        {//自己胜利
                            user_result.result = two_puzzle_win;
                            user_result.rewards_coins = m_win_reward_coins;
                        }
                        else if(user_result.self_score < user_result.opp_score)
                        {//对手胜利
                            user_result.result = two_puzzle_lose;
                            user_result.rewards_coins = user_result.self_score == 0 ? 0 : m_lose_reward_coins;
                        }
                        else
                        {//平局
                            user_result.result = two_puzzle_tie;
                            user_result.rewards_coins = user_result.self_score == 0 ? 0 : m_tie_reward_coins;
                        }


                        //重新设置奖励，查看是否超过每日限制
                        set_real_coins(m_user_id, &user_result);

                        if((m_errno = pack(&user_result, sizeof(two_puzzle_result_t))) != 0)
                        {
                            ERROR_LOG("[imitate_user]: send_buffer overflowed while pack self_result(m_send_buffer_len:%u, pack_len:%lu).", m_send_buffer_len, sizeof(two_puzzle_result_t));
                        }

                        INFO_LOG("imitate 1 send result to user %u:(自己得分:%u 机器人得分%u 结果：%u 奖励:%u 是否达到每日限制:%u) ", m_user_id, user_result.self_score, user_result.opp_score, user_result.result, user_result.rewards_coins, user_result.is_restrict );

                        send_to_client(p_fdsess, 0, svr_msg_multi_online_result);

                        tmp_iter = iter;
                        iter++;
                        m_gaming_with_robot_map.erase(tmp_iter);

                        continue;
                    }
                    else
                    {
                        opp_answer.opp_cur_score = m_imitate_answer_vec[imitate_index][step_index + 1].score;
                        opp_answer.opp_remain_time = 0;
                        opp_answer.opp_is_end = 1;
                    }
                }
                else
                {
                    opp_answer.opp_cur_score = m_imitate_answer_vec[imitate_index][step_index + 1].score;
                    opp_answer.opp_remain_time = m_imitate_answer_vec[imitate_index][step_index + 1].remain_time;
                    opp_answer.opp_is_end = 0;
                    iter->second.trigger_robot_timestamp += m_imitate_answer_vec[imitate_index][step_index + 1].next_time;
                    iter->second.step_index += 1;

                }

                INFO_LOG("send to %u(机器人成绩opp_score:%u, remain_time:%u, is_end:%u)", m_user_id, opp_answer.opp_cur_score, opp_answer.opp_remain_time, opp_answer.opp_is_end);

                if((m_errno = pack(&opp_answer, sizeof(opp_answer_info_t))) != 0)
                {
                    ERROR_LOG("[answer_two_puzzle]: send_buffer overflowed while pack opp_answer(m_send_buffer_len:%u, pack_len:%lu).", m_send_buffer_len, sizeof(opp_answer_info_t));
                }

                send_to_client(p_fdsess, 0, svr_msg_multi_online_answer);
            }
        }
        else
        {//机器人答题已结束
            //这种情况无需处理
            if(iter->second.is_end == 1)
            {//用户也结束
                two_puzzle_result_t user_result;

                uint16_t robot_score = m_imitate_answer_vec[imitate_index][step_index + 1].score;

                user_result.self_score = iter->second.cur_score;
                user_result.opp_score = robot_score;

                if(user_result.self_score > user_result.opp_score)
                {//自己胜利
                    user_result.result = two_puzzle_win;
                    user_result.rewards_coins = m_win_reward_coins;
                }
                else if(user_result.self_score < user_result.opp_score)
                {//对手胜利
                    user_result.result = two_puzzle_lose;
                    user_result.rewards_coins = user_result.self_score == 0 ? 0 : m_lose_reward_coins;
                }
                else
                {//平局
                    user_result.result = two_puzzle_tie;
                    user_result.rewards_coins = user_result.self_score == 0 ? 0 : m_tie_reward_coins;
                }


                //重新设置奖励，查看是否超过每日限制
                set_real_coins(m_user_id, &user_result);

                if((m_errno = pack(&user_result, sizeof(two_puzzle_result_t))) != 0)
                {
                    ERROR_LOG("[imitate_user]: send_buffer overflowed while pack self_result(m_send_buffer_len:%u, pack_len:%lu).", m_send_buffer_len, sizeof(two_puzzle_result_t));
                }

                INFO_LOG("imitate 2 send result to user %u:(自己得分:%u 机器人得分%u 结果：%u 奖励:%u 是否达到每日限制:%u) ", m_user_id, user_result.self_score, user_result.opp_score, user_result.result, user_result.rewards_coins, user_result.is_restrict );

                send_to_client(p_fdsess, 0, svr_msg_multi_online_result);

                tmp_iter = iter;
                iter++;
                m_gaming_with_robot_map.erase(tmp_iter);
                continue;
            }
        }

        iter++;
    }

    ADD_TIMER_EVENT(&m_imitate_timer, &wrap_imitate_user, this, get_now_tv()->tv_sec + m_imitate_time);
    return 0;
}
