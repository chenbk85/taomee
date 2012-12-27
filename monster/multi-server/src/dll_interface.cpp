/*
 * =====================================================================================
 *
 *       Filename:  online.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月18日 14时19分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */

#include <string.h>

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>
}

#include "../../common/message.h"
#include "../../common/constant.h"
#include "../../common/data_structure.h"
#include "work_proc.h"

using namespace std;

extern c_work_proc g_work_proc;


int dispatch(void *p_data, int len, fdsession_t *fdsess);

/**
 * @brief init_service 异步框架要求实现的接口之一
 *
 * @param isparent
 *
 * @return
 */
extern "C" int init_service(int isparent)
{
    if(!isparent)
    {
        setup_timer();
        if(g_work_proc.init() != 0)
        {
            return -1;
        }
    }

    return 0;
}

/**
 * @brief fini_service 异步框架要求实现的接口之一
 *
 * @param isparent
 *
 * @return
 */
extern "C" int fini_service(int isparent)
{
    DEBUG_LOG("fini service, isparent:%u", isparent);
    if(!isparent)
    {
        g_work_proc.uninit();
        destroy_timer();
    }
    return 0;
}

/**
 * @brief proc_events 异步框架要求实现的接口之一
 */
extern "C" void proc_events()
{
    handle_timer();
}

/**
 * @brief get_pkg_len 异步框架要求实现的接口之一
 *
 * @param fd
 * @param pkg
 * @param pkglen
 * @param isparent
 *
 * @return
 */
extern "C" int get_pkg_len(int fd, const void* pkg, int pkglen, int isparent)
{
    if(pkglen < 4)
    {
        return 0;
    }

    svr_msg_header_t *p_header = (svr_msg_header_t*)pkg;
    if(p_header->len > MAX_RECV_PKG_SIZE || p_header->len < sizeof(svr_msg_header_t))
    {
        ERROR_LOG("invalid len = %u from fd = %d.", p_header->len, fd);
        return -1;
    }

    if((int)p_header->len <= pkglen)
    {
        return p_header->len;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief proc_pkg_from_client 异步框架要求实现的接口之一
 *
 * @param data
 * @param len
 * @param fdsess
 *
 * @return
 */
extern "C" int proc_pkg_from_client(void *data, int len, fdsession_t *fdsess)
{
    return dispatch(data, len, fdsess);
}

/**
 * @brief proc_pkg_from_serv 异步框架要求实现的接口之一
 *
 * @param fd
 * @param data
 * @param len
 *
 * @return
 */
extern "C" void proc_pkg_from_serv(int fd, void *data, int len)
{
}

/**
 * @brief on_client_conn_closed 异步框架要求实现的接口之一
 *
 * @param fd
 */
extern "C" void on_client_conn_closed(int fd)
{
    g_work_proc.online_closed(fd);
}

/**
 * @brief on_fd_closed 异步框架要求实现的接口之一
 *
 * @param fd
 */
extern "C" void on_fd_closed(int fd)
{
}

int dispatch(void *p_data, int len, fdsession_t *fdsess)
{
    svr_msg_header_t *p_recv_msg = (svr_msg_header_t*)p_data;


    uint16_t msg_type = p_recv_msg->msg_type;
    if((int)p_recv_msg->len > len)
    {
        ERROR_LOG("msg_len error,type:%u, len in header:%u , recved_len:%d.", msg_type, p_recv_msg->len, len);
        return -1;
    }

    int body_len = p_recv_msg->len - sizeof(svr_msg_header_t);
    switch(msg_type)
    {
        case svr_msg_online_multi_enter://进入二人益智游戏
            return g_work_proc.enter_two_puzzle(p_recv_msg, body_len, fdsess);
        case svr_msg_online_multi_start://点击start按钮
            return g_work_proc.start_two_puzzle(p_recv_msg, body_len, fdsess);
        case svr_msg_online_multi_answer://回答益智题目
            return g_work_proc.answer_two_puzzle(p_recv_msg, body_len, fdsess);
        case svr_msg_logout://用户退出online
            return g_work_proc.user_logout_online(p_recv_msg, body_len, fdsess);
        case svr_msg_online_keepalive:
            return g_work_proc.make_online_keepalive(p_recv_msg, body_len, fdsess);
        case svr_msg_online_register:
            return g_work_proc.register_online(p_recv_msg, body_len, fdsess);
        case svr_msg_online_exit:
            return g_work_proc.online_exit(p_recv_msg, body_len, fdsess);
        default:
            ERROR_LOG("invalid msg_type=%u from fd=%u.", msg_type, fdsess->fd);
            return -1;
    }

    return 0;
}
