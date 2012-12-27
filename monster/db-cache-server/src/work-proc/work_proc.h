/*
 * =====================================================================================
 *
 *       Filename:  work_proc.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年06月29日 18时23分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_WORK_PROC_H_20110629
#define H_WORK_PROC_H_20110629
#include <map>
#include <stdint.h>
#include "i_mysql_iface.h"

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
}
#include <benchapi.h>
#include "hiredis.h"

#include "../../../common/data_structure.h"
#include "../../../common/message.h"
#include "../../../common/pack/c_pack.h"
#include "../i_work_proc.h"

extern c_pack g_pack;

#define MAX_PAGE_NUM 999
#define PER_PAGE_NUM 8
//用户可以看到的麦咭的留言板的条数
#define MAIJI_PINBOARD_COUNT (MAX_PAGE_NUM * PER_PAGE_NUM)

//大众点评开始时间戳 2012-06-17 06:00:00
#define SHOW_BEGIN_TIMESTAMP 1339884000


//一周的秒数
#define WEEK_SECONDS    (7*24*60*60)


//当前是第几届大众点评
#define GET_SHOW_PERIOD(_time) (((_time) - SHOW_BEGIN_TIMESTAMP) / (WEEK_SECONDS) + 1)

//记录每一届大众点评参与者的key前缀
#define KEY_JOIN_PREFIX "joiner"

//记录每一届大众点品参与投票的用户
#define KEY_VOTE_PREFIX "vote"

//进入大众点评候选的人数
#define SHOW_USER_NUM   6

//大众点评冠军的奖励金币数
#define TOP1_REWARD_COIN 1000
//大众点品猜中冠军的奖励金币数
#define VOTE_TOP1_COIN  100

enum
{
    PHASE_ENTER = 0,//参与阶段
    PHASE_VOTE = 1, //投票阶段
    PHASE_UNDEFINED = 2,//未定义的阶段
};


class c_work_proc : public i_work_proc
{
public:
    c_work_proc();
    ~c_work_proc();
    int init(int argc, char** argv);
    int uninit();
    int release();
    int process(char *req, int req_len, char **ack, int *ack_len);

private:
    //初始化麦咭的留言板
    int init_maiji_pinboard();

    //协议处理函数
    int process_get_maiji_pinboard(const char *p_request_body, uint32_t request_len);
    int process_add_maiji_pinboard(const char *p_request_body, uint32_t request_len);

    int process_enter_show(const char *p_request_body, uint32_t request_len);
    int process_join_show(const char *p_request_body, uint32_t request_len);
    int process_vote_show(const char *p_request_body, uint32_t request_len);
    int process_history_show(const char *p_request_body, uint32_t request_len);

    //redis相关操作函数
    int connect_to_redis_svr();
    int ping_redis();

//初始化记录每届大众点评结果的文件
    int init_show_file();

    inline uint8_t get_phase_of_show()
    {//获取当前期大众点评的阶段(PHASE_ENTER :报名阶段 PHASE_VOTE:投票阶段 PHASE_UNDEFINED:未定义阶段)

		time_t time_tmp = time(NULL);
        struct tm* _tm = localtime(&time_tmp);
        if(_tm == NULL) return PHASE_UNDEFINED;
        if(!(_tm->tm_wday == 0 || _tm->tm_wday == 6))
        {//非周六周日 报名阶段
            return (uint8_t)PHASE_ENTER;
        }

        if(_tm->tm_wday == 0 && _tm->tm_hour >= 6)
        {//周日6点之后
                return (uint8_t)PHASE_ENTER;
        }

        if(_tm->tm_wday == 6 && _tm->tm_hour < 6)
        {//周六6点之前
                return (uint8_t)PHASE_ENTER;
        }

        return (uint8_t)PHASE_VOTE;
    }

private:
    uint8_t m_inited;
    uint32_t m_errno;
    i_mysql_iface *m_p_mysql_conn;
    uint8_t m_db_id;

    uint32_t m_user_id;
    uint32_t m_seq_id;
    uint16_t m_msg_type;
    char m_response_buffer[MAX_SEND_PKG_SIZE];


    pinboard_message_t m_maiji_pinboard[MAIJI_PINBOARD_COUNT];
    int m_oldest_idx;//记录麦咭留言板中最早的一条留言的位置


    redisContext *m_p_rc;
    char m_redis_command[4096];//记录redis的命令

    char m_show_file_prefix[1024];
    uint32_t m_show_file_size;

    void *m_p_cur_top_start; //当前期
    show_user_num_t *m_p_user_num;
    show_user_info_t *m_p_show;
    uint32_t m_cur_show_id;
    uint8_t m_top_index[SHOW_USER_NUM];

    void *m_p_last_top_start;//上一期
    show_user_num_t *m_p_last_user_num;
    show_user_info_t *m_p_last_show;
};


#endif //H_WORK_PROC_20110629
