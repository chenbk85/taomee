/** 
 * ========================================================================
 * @file plugin_interface.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-04
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/inet/inet_utils.h>
}

#include "timer.h"
#include "db.h"
#include "collect.h"
#include "itl_common.h"
#include "dispatch.h"
#include "switch.h"
#include "switch_macro.h"
#include "plugin_interface.h"




int plugin_init(int type)
{
    switch (type)
    {
        case PROC_MAIN:
            break;
        case PROC_CONN:
            break;
        case PROC_WORK:
            {
                g_itl_head_ip = config_get_strval("head_ip", NULL);
                g_itl_head_port = config_get_intval("head_port", 0);
                if (!is_legal_ip(g_itl_head_ip) || !SWITCH_IS_BETWEEN(g_itl_head_port, 1024, 65535))
                {
                    ERROR_LOG("Invalid head_ip[%s] or head_port[%d]", g_itl_head_ip, g_itl_head_port);
                    return -1;
                }
                // 采集进程
                uint32_t work_id = get_work_id();
                char work_name[SWITCH_PROC_NAME_MAX_LEN] = {0};
                snprintf(work_name, sizeof(work_name), "itl_switch-COLLECT%u", work_id);
                set_title(work_name);
                setup_timer();
                INIT_LIST_HEAD(&(g_event.timer_list));
                INIT_LIST_HEAD(&(g_collect_timer.timer_list));
                init_connect_to_db();
                //安装定时拉去switch监控配置事件
                init_time_event();
            }
    }

    return 0;
}


int plugin_fini(int type)
{
    switch (type)
    {
        case PROC_MAIN:
            break;
        case PROC_CONN:
            break;
        case PROC_WORK:
            {
                if (1 == get_work_id())
                {
                    // 采集进程
                    fini_collect();
                    list_del_init(&(g_collect_timer.timer_list));
                    list_del_init(&(g_event.timer_list));
                    destroy_timer();
                }
                else
                {
                    destroy_timer();
                }
            }
    }
    return 0;
}


void time_event()
{
    handle_timer();
}


int get_pkg_len_cli(const char * buf, uint32_t len)
{
    // DEBUG_LOG("get pkg len cli, %u", len);
    if (len < 4)
    {
        return 0;
    }

    const node_proto_t * pkg = reinterpret_cast<const node_proto_t *>(buf);
    uint32_t pkg_len = pkg->len;

    if (pkg_len > PROTO_MAX_LEN || pkg_len < sizeof(node_proto_t))
    {
        ERROR_LOG("invalid len: %u", pkg_len);
        return -1;
    }

    return pkg_len;

}

int get_pkg_len_ser(const char * buf, uint32_t len)
{
    // DEBUG_LOG("get pkg len ser, %u", len);
    if (len < 4)
    {
        return 0;
    }

    uint32_t pkg_len = *(uint32_t *)(buf);

    if (pkg_len > PROTO_MAX_LEN || pkg_len < sizeof(itl_proto_t))
    {
        ERROR_LOG("invalid len: %u", pkg_len);
        return -1;
    }

    return pkg_len;

}


int check_open_cli(uint32_t ip, uint16_t port)
{
    return 0;
}

int select_channel(int fd, const char * buf, uint32_t len, uint32_t ip, uint32_t work_num)
{
    const node_proto_t * pkg = reinterpret_cast< const node_proto_t * >(buf);

    uint16_t cmd = pkg->cmd;

    switch (cmd)
    {
        case switch_p_get_metric_data_cmd:
            // 给collect进程
            return fd % work_num;
        default:
            // 给command进程
            return 0;
    }

    return 0;
}


int shmq_pushed(int fd, const char * buf, uint32_t len, int flag)
{
    return 0;
}


void proc_pkg_cli(int fd, const char * buf, uint32_t len)
{
    dispatch(fd, buf, len);
}


void proc_pkg_ser(int fd, const char * buf, uint32_t len)
{
    if (is_db_fd(fd))
    {
        INFO_LOG("recv pkg from db, len: %u", len);
        dispatch_db(fd, buf, len);
    }
    else if (is_switch_head_fd(fd))
    {
        INFO_LOG("recv pkg from itl head, len: %u", len);
        dispatch_switch_head(fd, buf, len);
    }
}


void link_up_cli(int fd, uint32_t ip)
{

    DEBUG_LOG("client link up, fd: %d", fd);
}


void link_down_cli(int fd)
{
    DEBUG_LOG("client link down, fd: %d", fd);

}


void link_down_ser(int fd)
{
    DEBUG_LOG("server link down, fd: %d", fd);

    if (is_switch_head_fd(fd))
    {
        WARN_LOG("OA HEAD LINK DOWN, fd: %d", fd);
        close_switch_head_fd(fd);
    }
    else if (is_db_fd(fd))
    {
        WARN_LOG("DB LINK DOWN, fd: %d", fd);
        close_db_fd();
        init_connect_to_db();
    }
    //else if (is_command_fd(fd))
    //{
    //    WARN_LOG("COMMAND LINK DOWN, fd: %d", fd);
    //    close_command_fd();
    //    // command不需要保持连接
    //    // 下次有请求自动会连接
    //}

}
