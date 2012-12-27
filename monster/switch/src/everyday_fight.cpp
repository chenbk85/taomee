/*
 * =====================================================================================
 *
 *       Filename:  everyday_fight.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年06月04日 16时58分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <string.h>
#include "utility.h"
#include "everyday_fight.h"

c_everyday_fight g_ed_fight;

c_everyday_fight::c_everyday_fight()
{
    memset(m_npc_score, 0, sizeof(m_npc_score));
    m_cur_day = 0;
    m_score_index = 0;
    memset(m_daily_fight, 0, sizeof(m_daily_fight));
    m_inited = 0;
}

c_everyday_fight::~c_everyday_fight()
{
    memset(m_npc_score, 0, sizeof(m_npc_score));
    m_cur_day = 0;
    m_score_index = 0;
    memset(m_daily_fight, 0, sizeof(m_daily_fight));
    m_inited = 0;

}


int c_everyday_fight::init()
{
    if(m_inited)
    {
        return -1;
    }

    memset(m_send_buffer, 0, sizeof(m_send_buffer));
    m_send_buffer_len = 0;
    m_p_send_header = (svr_msg_header_t*)m_send_buffer;

    m_errno = ERR_NO_ERR;
    m_userid = 0;
    m_msg_type= 0;

    m_cur_day = timestamp2day(time(NULL));
    m_score_over15[0] = 12345;
    m_score_over15[1] = 8888804;
    m_score_index = 2;
    memset(m_npc_score, 0, sizeof(m_npc_score));
    memset(m_daily_fight, 0, sizeof(m_daily_fight));
        m_total_npc_score_num = 0;
        m_total_daily_fight_num = 0;

    m_inited = 1;
    return 0;
}

int c_everyday_fight::uninit()
{
    if(!m_inited)
    {
        return -1;
    }

    memset(m_send_buffer, 0, sizeof(m_send_buffer));
    m_p_send_header = NULL;
    memset(m_npc_score, 0, sizeof(m_npc_score));
    memset(m_daily_fight, 0, sizeof(m_daily_fight));
        m_total_npc_score_num = 0;
        m_total_daily_fight_num = 0;
    m_inited = 0;
    return 0;
}

int c_everyday_fight::encourage_guide(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_userid = p_recv_header->user_id;
    m_msg_type = p_recv_header->msg_type;
    m_send_buffer_len =  sizeof(svr_msg_header_t);

    m_errno = check_val_len(sizeof(db_request_encourage_t), body_len);
    if(m_errno != 0)
    {
        goto end;
    }
    {
    db_request_encourage_t *p_body = (db_request_encourage_t*)p_recv_header->body;
    int body_day = timestamp2day(p_body->timestamp);
    if(body_day != m_cur_day)
    {
        m_score_index = 2;
        memset(m_npc_score, 0, sizeof(m_npc_score));
        memset(m_daily_fight, 0, sizeof(m_daily_fight));
        m_total_npc_score_num = 0;
        m_total_daily_fight_num = 0;
        m_cur_day = body_day;
    }

    db_return_encourage_t *p_res = (db_return_encourage_t*)(m_send_buffer + m_send_buffer_len);

	p_res->win_num = 0;
    m_send_buffer_len += sizeof(db_return_encourage_t);

    p_res->type = p_body->type;
    p_res->score = p_body->score;

    if(p_body->type == ROOM_SCORE)
    {//小屋评分
        if(p_body->score > MAX_NPC_SCORE)
        {
            p_body->score = p_body->score % MAX_NPC_SCORE;
        }
        m_npc_score[p_body->score]++;
        m_total_npc_score_num++;

        if(p_body->score >= 15)
        {
            if(m_score_index < MAX_SCORE_OVER15_NUM)
            {
                m_score_over15[m_score_index++] = m_userid;
            }
        }

        p_res->total_num = m_total_npc_score_num;
        int idx = 0;
        for(idx = 0; idx < (int)p_body->score; idx++)
        {
            p_res->win_num += m_npc_score[idx];
        }

        idx = uniform_rand(0, m_score_index - 1);
        p_res->user_id =  m_score_over15[idx];
    }
    else if(p_body->type == DAILY_TEST)
    {//每日挑战
        if(p_body->score > PUZZLE_MAX_RIGHT_NUM)
        {
            p_body->score = p_body->score % PUZZLE_MAX_RIGHT_NUM;
        }
        m_daily_fight[p_body->score]++;
        m_total_daily_fight_num++;
        p_res->total_num = m_total_daily_fight_num;
        int idx = 0;
        for(idx = 0; idx < (int)p_body->score; idx++)
        {
            p_res->win_num += m_daily_fight[idx];
        }

        idx = uniform_rand(0, m_score_index - 1);
        p_res->user_id =  m_score_over15[idx];

    }
    else
    {
        m_errno = ERR_ENCOURAGE_TYPE;
        m_send_buffer_len =  sizeof(svr_msg_header_t);
    }


    KINFO_LOG(m_userid, "p_res->type:%u, score:%u, total:%u, win:%u, user_id:%u", p_res->type, p_res->score, p_res->total_num, p_res->win_num, p_res->user_id);

    m_p_send_header->len = m_send_buffer_len;
    m_p_send_header->seq_id = 0;
    m_p_send_header->msg_type = m_msg_type;
    m_p_send_header->result = m_errno;
    m_p_send_header->user_id = m_userid;
    return send_pkg_to_client(fdsess, m_send_buffer, m_send_buffer_len);
    }
end:
    return 0;
}
