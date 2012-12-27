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
#include "ucount.h"
#include "switch.h"
#include "db_cache.h"
#include "function.h"
#include "stat.h"
#include "multi_svr.h"

using namespace std;

extern int parse_xml_data();

static int init_server_instance()
{
    const char *switch_ip = config_get_strval("switch_ip");
    if (NULL == switch_ip || strlen(switch_ip) > 16)
    {
        ERROR_LOG("switch ip get from config failed.");
        return -1;
    }
    int switch_port = config_get_intval("switch_port", 11001);
    int switch_bufsz = config_get_intval("switch_bufsz", 65535);
    int switch_timeout = config_get_intval("switch_timeout", 1);

    //创建到switch的连接
    if(create_server_instance(&g_p_switch_svr) != 0)
    {
        ERROR_LOG("create switch_server instance failed.");
        return -1;
    }

    //初始化到switch的连接
    if(g_p_switch_svr->init(switch_bufsz, switch_timeout, "switch", switch_ip, switch_port) != 0)
    {
        ERROR_LOG("init g_p_switch_svr failed.");
        return -1;
    }


    INIT_LIST_HEAD(&(g_register_to_switch_timer.timer_list));
    ADD_TIMER_EVENT(&g_register_to_switch_timer, register_to_switch, (void*)NULL, get_now_tv()->tv_sec + 10);



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

    const char *ucount_ip = config_get_strval("ucount_ip");
    if (NULL == ucount_ip || strlen(ucount_ip) > 15)
    {
        ERROR_LOG("ucount ip get from config failed.");
        return -1;
    }
    int ucount_port = config_get_intval("ucount_port", 21001);
    int ucount_bufsz = config_get_intval("ucount_bufsz", 65535);
    int ucount_timeout = config_get_intval("ucount_timeout", 1);

    //创建到ucount的连接
    if(create_server_instance(&g_p_ucount_svr) != 0)
    {
        ERROR_LOG("create ucount_server instance failed.");
        return -1;
    }

    if(g_p_ucount_svr->init(ucount_bufsz, ucount_timeout, "ucount", ucount_ip, ucount_port) != 0)
    {
        ERROR_LOG("init g_p_ucount_svr failed.");
        return -1;
    }

    //创建到多人对战游戏服务器的连接
    const char *multi_svr_ip = config_get_strval("multi_server_ip");
    if (NULL == multi_svr_ip || strlen(multi_svr_ip) > 16)
    {
        ERROR_LOG("multi_server_ip get from config failed.");
        return -1;
    }
    int multi_svr_port = config_get_intval("multi_server_port", 51001);
    int multi_svr_bufsz = config_get_intval("multi_server_bufsz", 65535);
    int multi_svr_timeout = config_get_intval("multi_server_timeout", 1);

    //创建到ucount的连接
    if(create_server_instance(&g_p_multi_server_svr) != 0)
    {
        ERROR_LOG("create multi_server instance failed.");
        return -1;
    }

    if(g_p_multi_server_svr->init(multi_svr_bufsz, multi_svr_timeout, "multi_svr", multi_svr_ip, multi_svr_port) != 0)
    {
        ERROR_LOG("init g_p_multi_server_svr failed.");
        return -1;
    }

    INIT_LIST_HEAD(&(g_register_to_multi_timer.timer_list));
    ADD_TIMER_EVENT(&g_register_to_multi_timer, register_to_multi, (void*)NULL, get_now_tv()->tv_sec + 10);


    INIT_LIST_HEAD(&(g_keepalive_timer.timer_list));
    ADD_TIMER_EVENT(&g_keepalive_timer, keepalive_check, (void*)NULL, get_now_tv()->tv_sec + 30);


    //创建到分享服务器的连接
    const char *share_ip = config_get_strval("share_ip");
    if (NULL == share_ip || strlen(share_ip) > 16)
    {
        ERROR_LOG("share_ip get from config failed.");
        return -1;
    }
    int share_port = config_get_intval("share_port", 52001);
    int share_bufsz = config_get_intval("share_bufsz", 65535);
    int share_timeout = config_get_intval("share_timeout", 1);

    //创建到share的连接
    if(create_server_instance(&g_p_share_svr) != 0)
    {
        ERROR_LOG("create share_server instance failed.");
        return -1;
    }

    if(g_p_share_svr->init(share_bufsz, share_timeout, "share_svr", share_ip, share_port) != 0)
    {
        ERROR_LOG("init g_p_share_svr failed.");
        return -1;
    }

    //创建到db缓存服务器的连接
    const char *cache_ip = config_get_strval("cache_ip");
    if (NULL == cache_ip || strlen(cache_ip) > 16)
    {
        ERROR_LOG("cache_ip get from config failed.");
        return -1;
    }
    int cache_port = config_get_intval("cache_port", 8799);
    int cache_bufsz = config_get_intval("cache_bufsz", 65535);
    int cache_timeout = config_get_intval("cache_timeout", 1);
    //创建到cache的连接
    if(create_server_instance(&g_p_db_cache_svr) != 0)
    {
        ERROR_LOG("create db-cache_server instance failed.");
        return -1;
    }

    if(g_p_db_cache_svr->init(cache_bufsz, cache_timeout, "cache_svr", cache_ip, cache_port) != 0)
    {
        ERROR_LOG("init g_p_db_cache_svr failed.");
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
    if (NULL != g_p_ucount_svr)
    {
        g_p_ucount_svr->uninit();
        g_p_ucount_svr->release();
        g_p_ucount_svr = NULL;
    }
    if (NULL != g_p_switch_svr)
    {
        g_p_switch_svr->uninit();
        g_p_switch_svr->release();
        g_p_switch_svr = NULL;
    }

    if(NULL != g_p_multi_server_svr)
    {
        g_p_multi_server_svr->uninit();
        g_p_multi_server_svr->release();
        g_p_multi_server_svr = NULL;
    }

    if(NULL != g_p_share_svr)
    {
        g_p_share_svr->uninit();
        g_p_share_svr->release();
        g_p_share_svr = NULL;
    }

    if(NULL != g_p_db_cache_svr)
    {
        g_p_db_cache_svr->uninit();
        g_p_db_cache_svr->release();
        g_p_db_cache_svr = NULL;
    }

}

static int parse_config()
{
    const char *stat_file = config_get_strval("stat_file");
    init_log_file(stat_file);

    g_tel_or_net = config_get_intval("tel_or_net", 0);
    KDEBUG_LOG(0, "tel_or_net:%u", g_tel_or_net);

    return 0;
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
	DEBUG_LOG("init server instance");

        if (parse_xml_data() != 0)
        {
            return -1;
        }
	DEBUG_LOG("parse xml data");

        if (parse_config() != 0)
        {
            return -1;
        }
	DEBUG_LOG("parse config");

       // if (register_online() != 0)
       // {
       //     ERROR_LOG("register online");
       //     return -1;
       // }
       // DEBUG_LOG("register online,id = %u,ip:%s,port:%u", config_cache.bc_elem->online_id, config_cache.bc_elem->bind_ip, config_cache.bc_elem->bind_port);

        ADD_TIMER_EVENT(&g_user_mng, broadcast_timeout_cb, NULL, time(NULL) + BROADCAST_TIME);
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
        g_user_mng.process_all_user(update_on_logout);
        sleep(5);
        exit_online();
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
           // DEBUG_LOG("get flash policy from online.");
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
    else if (fd == g_p_ucount_svr->get_server_fd())
    {
        handle_ucount_return((svr_msg_header_t *)data, len);
    }
    else if (fd == g_p_switch_svr->get_server_fd())
    {
        handle_switch_return((svr_msg_header_t *)data, len);
    }
    else if(fd == g_p_multi_server_svr->get_server_fd())
    {
        handle_multi_svr_return((svr_msg_header_t*)data, len);
    }
    else if(fd == g_p_db_cache_svr->get_server_fd())
    {
        handle_cache_svr_return((svr_msg_header_t*)data, len);
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
        if (p_user->is_register)
        {
            update_on_logout(p_user);
        }
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
    else if (fd == g_p_ucount_svr->get_server_fd())
    {
        DEBUG_LOG("ucount fd:%u closed.", fd);
        g_p_ucount_svr->set_server_fd();
    }
    else if (fd == g_p_switch_svr->get_server_fd())
    {
        DEBUG_LOG("switch fd:%u closed.", fd);
        g_p_switch_svr->set_server_fd();
    }
    else if (fd == g_p_share_svr->get_server_fd())
    {
        DEBUG_LOG("share svr fd:%u closed.", fd);
        g_p_share_svr->set_server_fd();
    }
    else if (fd == g_p_multi_server_svr->get_server_fd())
    {
        DEBUG_LOG("multi svr fd:%u closed.", fd);
        g_p_multi_server_svr->set_server_fd();
    }
    else if(fd == g_p_db_cache_svr->get_server_fd())
    {
        DEBUG_LOG("cache svr fd:%u closed.", fd);
        g_p_db_cache_svr->set_server_fd();

    }
    else
    {
        ERROR_LOG("unknown fd:%u closed in child.", fd);
    }
}
