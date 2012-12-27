/*
 * =====================================================================================
 *
 *       Filename:  everyday_fight.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年06月04日 15时39分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_EVERYDAY_FIGHT_H_20120604
#define H_EVERYDAY_FIGHT_H_20120604

#include <stdint.h>
#include <map>

extern "C"
{
    #include <libtaomee/log.h>
    #include <libtaomee/timer.h>
    #include <libtaomee/list.h>
    #include <libtaomee/project/stat_agent/msglog.h>
    #include <libtaomee/conf_parser/config.h>
    #include <async_serv/net_if.h>
}

#include "switch.h"
#include "../../common/message.h"
#include "../../common/constant.h"
#include "../../common/data_structure.h"
#include "../../common/stat.h"

#define MAX_SCORE_OVER15_NUM 1000 // 最多记录1000个小屋评分大于15分的用户
class c_everyday_fight
{
    public:
        c_everyday_fight();
        ~c_everyday_fight();
        int init();
        int uninit();

        int encourage_guide(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    private:
        inline int check_val_len(uint32_t real_len, uint32_t recved_len)
        {
            if(real_len != recved_len)
            {
                ERROR_LOG("[msg_type:%u]recved_len(%u) is not equal to real_len(%u).", m_msg_type, recved_len, real_len);
                return ERR_MSG_LEN;
            }
            return 0;
        }
    private:
        uint8_t m_inited;


        char m_send_buffer[MAX_SEND_PKG_SIZE];                      //发送缓冲
        uint32_t m_send_buffer_len;
        svr_msg_header_t *m_p_send_header;                               //指向发送缓冲

        uint32_t m_errno;
        uint32_t m_userid;
        uint16_t m_msg_type;



        int m_cur_day;
        uint32_t m_npc_score[MAX_NPC_SCORE + 1]; //记录每个得分的人数
        uint32_t m_total_npc_score_num;//总共参与小屋评分的人数

        uint32_t m_score_over15[MAX_SCORE_OVER15_NUM];//记录小屋评分大于15分的用户米米号
        int m_score_index;//记录m_score_over15的下标
        uint32_t m_daily_fight[PUZZLE_MAX_RIGHT_NUM + 1];//记录每日挑战各个得分的人数
        uint32_t m_total_daily_fight_num; //总共参与每日挑战的人数

};

#endif // H_EVERYDAY_FIGHT_H_20120604
