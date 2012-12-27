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

#include "../../../common/data_structure.h"
#include "../../../common/message.h"
#include "../../../common/pack/c_pack.h"
#include "../i_work_proc.h"
#include "../memcached/c_memcached.h"

extern c_pack g_pack;

/**
 * @brief uint32_t
 *
 * @param i_mysql_iface mysql连接
 * @param uint32_t      用户米米号
 * @param uint32_t      协议类型
 * @param char          请求包体
 * @param uint32_t      请求包体长度
 *
 * @return
 */
typedef  uint32_t (*MSG_PROCESS_FUNC)(i_mysql_iface *, c_memcached *, uint32_t, uint32_t,  char *, uint32_t);

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
    int init_msg_map();

private:
    uint8_t m_inited;
    uint32_t m_errno;
    i_mysql_iface *m_p_mysql_conn;
    c_memcached m_memcached;
    uint8_t m_db_id;

    uint32_t m_user_id;
    uint32_t m_seq_id;
    uint16_t m_msg_type;
    char m_response_buffer[MAX_SEND_PKG_SIZE];

    std::map<uint16_t, MSG_PROCESS_FUNC> m_msg_func_map;

};

#endif //H_WORK_PROC_20110629
