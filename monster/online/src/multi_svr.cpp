/*
 * =====================================================================================
 *
 *       Filename:  multi_svr.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年12月16日 11时10分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <assert.h>

#include "data.h"
#include "message.h"
#include "multi_svr.h"
#include "function.h"
#include "cli_proto.h"

void handle_multi_svr_return(svr_msg_header_t* p_pkg, int len)
{
    assert(p_pkg != NULL);

    uint16_t multi_svr_msg = p_pkg->msg_type;
#ifdef DEBUG
    KDEBUG_LOG(p_pkg->user_id, "recv msg %u from multi_svr.", multi_svr_msg);
#endif
    fdsession_t *fdsess = g_user_mng.get_fd(p_pkg->user_id);;
    if(fdsess == NULL)
    {
        KCRIT_LOG(p_pkg->user_id, "User is not online.");
        return;
    }

    usr_info_t *p_user = g_user_mng.get_user(fdsess->fd);
    if(p_user == NULL)
    {
        KCRIT_LOG(p_pkg->user_id, "User is not in Cache.");
        return;
    }

    if((int)p_pkg->len != len)
    {
        KCRIT_LOG(p_pkg->user_id, "msg len from multi_svr is not correct, p_pkg->len:%u, len:%d", p_pkg->len, len);
        return;
    }

    if(p_pkg->seq_id != 0)
    {//seq_id == 0的消息为服务器主动推送过来的消息
        uint32_t counter = p_pkg->seq_id & 0xFFFF;
        if(counter != p_user->counter)
        {
            KCRIT_LOG(p_pkg->user_id, "It should not happen this case.");
            return;
        }
        REMOVE_TIMERS(p_user);
    }

    g_errno = ERR_NO_ERR;
    bool clear_waitcmd = true;

    switch(multi_svr_msg)
    {
        case svr_msg_online_multi_enter:
            process_enter_two_puzzle_return(p_user, p_pkg, len);
            break;
        case svr_msg_multi_online_match:
            //服务器主动推送的协议
            process_two_puzzle_match_notice(p_user, p_pkg, len);
            clear_waitcmd = false;
            return;
        case svr_msg_online_multi_start:
            process_start_two_puzzle_return(p_user, p_pkg, len);
            break;
        case svr_msg_multi_online_start:
            //服务器主动推送的协议
            process_two_puzzle_start_notice(p_user, p_pkg, len);
            clear_waitcmd = false;
            return;
        case svr_msg_online_multi_answer:
            process_answer_two_puzzle_return(p_user, p_pkg, len);
            break;
        case svr_msg_multi_online_answer:
            //服务器主动推送的协议
            process_two_puzzle_answer_notice(p_user, p_pkg, len);
            clear_waitcmd = false;
            return;
        case svr_msg_multi_online_result:
            //服务器主动推送的协议
            process_two_puzzle_result_notice(p_user, p_pkg, len);
            clear_waitcmd = false;
            return;
        default:
            KCRIT_LOG(p_pkg->user_id, "recv unkonwn [msg:%u] from multi_svr.", p_pkg->msg_type);
            break;
    }

    if(g_errno != ERR_NO_ERR)
    {
        send_header_to_user(p_user, SYSTEM_ERR_NUM, g_errno);
    }

   return;
}

int process_enter_two_puzzle_return(usr_info_t *p_user, svr_msg_header_t* p_pkg, int len)
{
    assert(p_user != NULL && p_pkg != NULL);

    if(p_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "enter_two_puzzle failed(errno:%u).", p_pkg->result);
        g_errno = p_pkg->result;
        return 0;
    }

    uint8_t is_found = *(uint8_t*)p_pkg->body;
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    KINFO_LOG(p_user->uid, "Get matching info :isfound:%u", is_found);
    g_send_msg.pack(is_found);
    if(is_found != 0)
    {
        const matching_opponent_t *p_match_opp = (const matching_opponent_t*)(p_pkg->body + sizeof(uint8_t));
        g_send_msg.pack(p_match_opp->opp_info.uid);
        g_send_msg.pack(p_match_opp->opp_info.uname, 16);
        g_send_msg.pack(p_match_opp->opp_info.is_vip);
        g_send_msg.pack(p_match_opp->opp_info.mon_id);
        g_send_msg.pack(p_match_opp->opp_info.mon_main_color);
        g_send_msg.pack(p_match_opp->opp_info.mon_exp_color);
        g_send_msg.pack(p_match_opp->opp_info.mon_eye_color);

        g_send_msg.pack(p_match_opp->puz_com.compete_init_time);
        g_send_msg.pack(p_match_opp->puz_com.question_num);
        KINFO_LOG(p_user->uid, "1 question_num:%u", p_match_opp->puz_com.question_num);
        for(int i = 0; i < p_match_opp->puz_com.question_num; i++)
        {
           // KINFO_LOG(p_user->uid, "type:%u, id:%u", p_match_opp->puz_com.quests[i].quest_type, p_match_opp->puz_com.quests[i].quest_id);
            g_send_msg.pack(p_match_opp->puz_com.quests[i].quest_type);
            g_send_msg.pack(p_match_opp->puz_com.quests[i].quest_id);
        }
    }

    g_send_msg.end();

    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}

int process_two_puzzle_match_notice(usr_info_t *p_user, svr_msg_header_t* p_pkg, int len)
{
    assert(p_user != NULL && p_pkg != NULL);

    if(p_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "notice match puzzle info failed(errno:%u).", p_pkg->result);
        g_errno = p_pkg->result;
        return 0;
    }

    uint8_t is_found = *(uint8_t*)p_pkg->body;
    pack_as_pkg_header(p_user->uid, as_msg_match_two_puzzle, 0, ERR_NO_ERR);
    KINFO_LOG(p_user->uid, "notice matching info :is_found:%u", is_found);

    g_send_msg.pack(is_found);
    if(is_found != 0)
    {
        const matching_opponent_t *p_match_opp = (const matching_opponent_t*)(p_pkg->body + sizeof(uint8_t));
        g_send_msg.pack(p_match_opp->opp_info.uid);
        g_send_msg.pack(p_match_opp->opp_info.uname, 16);
        g_send_msg.pack(p_match_opp->opp_info.is_vip);
        g_send_msg.pack(p_match_opp->opp_info.mon_id);
        g_send_msg.pack(p_match_opp->opp_info.mon_main_color);
        g_send_msg.pack(p_match_opp->opp_info.mon_exp_color);
        g_send_msg.pack(p_match_opp->opp_info.mon_eye_color);

        g_send_msg.pack(p_match_opp->puz_com.compete_init_time);
        g_send_msg.pack(p_match_opp->puz_com.question_num);
        KINFO_LOG(p_user->uid, "2 question_num:%u", p_match_opp->puz_com.question_num);
        for(int i = 0; i < p_match_opp->puz_com.question_num; i++)
        {
            //KINFO_LOG(p_user->uid, "type:%u, id:%u", p_match_opp->puz_com.quests[i].quest_type, p_match_opp->puz_com.quests[i].quest_id);
            g_send_msg.pack(p_match_opp->puz_com.quests[i].quest_type);
            g_send_msg.pack(p_match_opp->puz_com.quests[i].quest_id);
        }
    }

    g_send_msg.end();

    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), false);

    return 0;
}

int process_start_two_puzzle_return(usr_info_t *p_user, svr_msg_header_t* p_pkg, int len)
{
    assert(p_user != NULL && p_pkg != NULL);

    if(p_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "start_two_puzzle info failed(errno:%u).", p_pkg->result);
        g_errno = p_pkg->result;
        return 0;
    }

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);

    uint8_t status = *(uint8_t*)p_pkg->body;
    g_send_msg.pack(status);
    if(status == 0)
    {//对方已离开
        KINFO_LOG(p_user->uid, "Opponent user has been out.");
    }
    else
    {
        KINFO_LOG(p_user->uid, "Opponent user has not been out.status = %u", status);
    }

    g_send_msg.end();

    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}

int process_two_puzzle_start_notice(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len)
{
    assert(p_user != NULL && p_pkg != NULL);

    if(p_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "notice start_two_puzzle info failed(errno:%u).", p_pkg->result);
        g_errno = p_pkg->result;
        return 0;
    }

    pack_as_pkg_header(p_user->uid, as_msg_begin_two_puzzle, 0, ERR_NO_ERR);

    uint8_t status = *(uint8_t*)p_pkg->body;
    g_send_msg.pack(status);
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), false);

    return 0;
}

int process_answer_two_puzzle_return(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len)
{
    assert(p_user != NULL && p_pkg != NULL);

    if(p_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "answer two puzzle return(errno:%u).", p_pkg->result);
        g_errno = p_pkg->result;
        return 0;
    }

    uint8_t status = *(uint8_t*)p_pkg->body;
    if(status != both_finished)
    {//比赛未结束
        KINFO_LOG(p_user->uid, "two_puzzle not end, status=%u", status);
    	pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    	g_send_msg.pack(status);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    }
    else
    {//比赛结束
        two_puzzle_result_t *p_result = (two_puzzle_result_t*)(p_pkg->body + sizeof(uint8_t));

        pack_as_pkg_header(p_user->uid, multi_puzzle_add_coin, 0, ERR_NO_ERR);
        g_send_msg.pack(p_result->self_score);
        g_send_msg.pack(p_result->opp_score);
        g_send_msg.pack(p_result->result);
        g_send_msg.pack(p_result->rewards_coins);
        g_send_msg.pack(p_result->is_restrict);
        g_send_msg.pack((uint16_t)p_user->waitcmd);
        g_send_msg.end();
        KINFO_LOG(p_user->uid, "cache a pkg, waitcmd:%u", p_user->waitcmd);
        cache_a_pkg(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), true);
    }
        return 0;
}

int process_two_puzzle_answer_notice(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len)
{
    assert(p_user != NULL && p_pkg != NULL);

    if(p_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "two puzzle answer notice(errno:%u).", p_pkg->result);
        g_errno = p_pkg->result;
        return 0;
    }

    pack_as_pkg_header(p_user->uid, as_msg_opp_answer_two_puzzle, 0, ERR_NO_ERR);
    opp_answer_info_t *p_answ = (opp_answer_info_t*)p_pkg->body;
    g_send_msg.pack(p_answ->opp_cur_score);
    g_send_msg.pack(p_answ->opp_remain_time);
    g_send_msg.pack(p_answ->opp_is_end);

    g_send_msg.end();
    KINFO_LOG(p_user->uid, "opp_score:%u remain_time:%u is_end:%u", p_answ->opp_cur_score, p_answ->opp_remain_time, p_answ->opp_is_end);
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), false);

    return 0;
}

int process_two_puzzle_result_notice(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len)
{
    assert(p_user != NULL && p_pkg != NULL);

    if(p_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "notice two_puzzle result info failed(errno:%u).", p_pkg->result);
        g_errno = p_pkg->result;
        return 0;
    }
    two_puzzle_result_t *p_result = (two_puzzle_result_t*)(p_pkg->body);

    pack_as_pkg_header(p_user->uid, multi_puzzle_add_coin, 0, ERR_NO_ERR);
    g_send_msg.pack(p_result->self_score);
    g_send_msg.pack(p_result->opp_score);
    g_send_msg.pack(p_result->result);
    g_send_msg.pack(p_result->rewards_coins);
    g_send_msg.pack(p_result->is_restrict);
    g_send_msg.pack((uint16_t)as_msg_two_puzzle_result);
    g_send_msg.end();
    KINFO_LOG(p_user->uid, "opp end cache a pkg, waitcmd:%u", p_user->waitcmd);
    cache_a_pkg(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}
