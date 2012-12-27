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
#include <libtaomee/conf_parser/config.h>
}
#include <libtaomee++/inet/byteswap.hpp>

#include "data_structure.h"

#include "data.h"
#include "cli_proto.h"
#include "db_proxy.h"
#include "function.h"
#include "stat.h"

using namespace std;

extern int parse_xml_data();

static int init_server_instance()
{
    const char *db_proxy_name = config_get_strval("db_proxy_name");
    if (NULL == db_proxy_name)
    {
        ERROR_LOG("db proxy name get from config failed.");
        return -1;
    }
    int db_bufsz = config_get_intval("db_proxy_bufsz", 65535);
    int db_timeout = config_get_intval("db_proxy_timeout", 1);

    //创建到db_proxy的连接
    if(create_server_instance(&g_p_db_proxy_svr) != 0)
    {
        ERROR_LOG("create db_proxy_server instance failed.");
        return -1;
    }

    if(g_p_db_proxy_svr->init(db_bufsz, db_timeout, db_proxy_name) != 0)
    {
        ERROR_LOG("init g_p_db_proxy_svr failed.");
        return -1;
    }

    return 0;
}

static void uninit_server_instance()
{
    if (NULL != g_p_db_proxy_svr)
    {
        g_p_db_proxy_svr->uninit();
        g_p_db_proxy_svr->release();
        g_p_db_proxy_svr = NULL;
    }
}

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
        init_db_handle_funs();
        init_cli_handle_funs();

        if (init_server_instance() != 0)
        {
            uninit_server_instance();
            return -1;
        }

        if (parse_xml_data() != 0)
        {
            return -1;
        }

        g_max_login_user = config_get_intval("max_login_num", 50);

        DEBUG_LOG("init service success");
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
        destroy_timer();
        uninit_server_instance();
    }
    return 0;
}

/**
 * @brief proc_events 异步框架要求实现的接口之一
 */
extern "C" void proc_events()
{
    proc_cached_pkgs();
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

    static char request[]  = "<policy-file-request/>";
    static char response[] = "<?xml version=\"1.0\"?>"
	                         "<!DOCTYPE cross-domain-policy SYSTEM\"/xml/dtds/cross-domain-policy.dtd\">"
			                 "<cross-domain-policy>"
			                 "<site-control permitted-cross-domain-policies=\"all\"/>"
                             "<allow-access-from domain=\"*\"to-ports=\"*\"/>"
                             "</cross-domain-policy>";

    if (pkglen < 4)
    {
        return 0;
    }

    int len = 0;
    if (isparent)
    {
        // the client requests for a socket policy file
        if ((pkglen == sizeof(request)) && !memcmp(pkg, request, sizeof(request)))
        {
            DEBUG_LOG("get flash policy from online.");
            net_send(fd, response, sizeof(response));
            return sizeof(request);
        }

        const as_msg_header_t *p_msg = (const as_msg_header_t *)pkg;

        len = taomee::bswap(p_msg->len);
        if ((len > MAX_RECV_PKG_SIZE) || (len < (int)sizeof(as_msg_header_t)))
        {
            ERROR_LOG("[parent] invalid len=%d from fd=%d", len, fd);
            return -1;
        }
        else if (len <= pkglen)
        {
            return len;
        }
        else
        {
            //接受的包长度不够
            return 0;
        }
    }
    else
    {
        len = *(uint32_t*)pkg;
        if ((len > MAX_RECV_PKG_SIZE) || (len < (int)sizeof(svr_msg_header_t)))
        {
            ERROR_LOG("[child] invalid len=%d from fd=%d", len, fd);
            return -1;
        }
        else if (len <= pkglen)
        {
            return len;
        }
        else
        {
            //接受的包长度不够
            return 0;
        }
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
    if (fd == g_p_db_proxy_svr->get_server_fd())
    {
        handle_db_return((svr_msg_header_t *)data, len);
    }
    else
    {
        ERROR_LOG("get msg from fd:%u that is unknown.", fd);
    }
}

/**
 * @brief on_client_conn_closed 异步框架要求实现的接口之一
 *
 * @param fd
 */
extern "C" void on_client_conn_closed(int fd)
{
    usr_info_t *p_user = g_user_mng.get_user(fd);
    if (p_user != NULL)
    {
        g_user_mng.dealloc_user(fd);
    }
}

/**
 * @brief on_fd_closed 异步框架要求实现的接口之一
 *
 * @param fd
 */
extern "C" void on_fd_closed(int fd)
{
    if (fd == g_p_db_proxy_svr->get_server_fd())
    {
        DEBUG_LOG("db_proxy fd:%u closed.", fd);
        g_p_db_proxy_svr->set_server_fd();
    }
    else
    {
        ERROR_LOG("unknown fd:%u closed in child.", fd);
    }
}
