/*
 * =====================================================================================
 *
 *       Filename:  switch.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月11日 15时54分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
}

#include "switch.h"
#include "online.h"
#include "shop.h"
#include "everyday_fight.h"

#include "../../common/message.h"
#include "../../common/constant.h"
#include "../../common/data_structure.h"

extern c_online g_online;
extern c_shop g_shop;
extern c_everyday_fight g_ed_fight;
/**
 * @brief init_service 异步框架要求实现的接口之一
 *
 * @param isparent
 *
 * @return
 */
int init_service(int isparent)
{
    if(!isparent)
    {
        setup_timer();
        if(g_online.init() != 0)
        {
            return -1;
        }
        if(g_shop.init() != 0)
        {
            return -1;
        }

        if(g_ed_fight.init() != 0)
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
int fini_service(int isparent)
{
    DEBUG_LOG("isparent:%d", isparent);
    if(!isparent)
    {
        g_online.uninit();
        g_shop.uninit();
        g_ed_fight.uninit();
        destroy_timer();
    }
    return 0;
}

/**
 * @brief proc_events 异步框架要求实现的接口之一
 */
void proc_events()
{
    handle_timer();
    g_shop.create_shop();
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
int get_pkg_len(int fd, const void* pkg, int pkglen, int isparent)
{
    if(pkglen < 4)
    {
        return 0;
    }

    svr_msg_header_t *p_header = (svr_msg_header_t*)pkg;
    if(p_header->len > MAX_RECV_PKG_SIZE || p_header->len < sizeof(svr_msg_header_t))
    {
        ERROR_LOG("invalid len=%u  from fd=%d.", p_header->len, fd);
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
int proc_pkg_from_client(void *data, int len, fdsession_t *fdsess)
{
    dispatch(data, len, fdsess);
    return 0;
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
void proc_pkg_from_serv(int fd, void *data, int len)
{
}

/**
 * @brief on_client_conn_closed 异步框架要求实现的接口之一
 *
 * @param fd
 */
void on_client_conn_closed(int fd)
{
//    g_online.clear_register_online(fd);
}

/**
 * @brief on_fd_closed 异步框架要求实现的接口之一
 *
 * @param fd
 */
void on_fd_closed(int fd)
{

}
