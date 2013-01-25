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


#include "plugin_interface.h"
#include "itl_timer.h"
#include "db.h"
#include "itl_common.h"
#include "dispatch.h"
#include "node.h"
#include "control.h"
#include "log_archive.h"
#include "proxy.h"
#include "db_mgr_interface.h"
#include "collect_interface.h"




int plugin_init(int type)
{
    switch (type)
    {
        case PROC_MAIN:
            break;
        case PROC_CONN:
            {
                // release版本要设置网络进程的user为nobody
#ifdef SET_PROC_USER
                if (0 != set_user("nobody"))
                {
                    ERROR_LOG("set net proc to nobody failed");
                    return -1;
                }
#endif
            }
            break;
        case PROC_WORK:
            {
                setup_timer();
                g_start_timestamp = get_now_tv()->tv_sec;
                get_node_ip();
                
                switch (get_work_idx())
                {
                    case PROC_PROXY:
                        // proxy 进程
                        init_proxy_process();
                        break;
                    case PROC_COLLECT:
                        // 采集进程

                        set_title("itl_node-COLLECT");


                        init_connect_to_db();
                        init_connect_to_head();
                        init_collect();
                        init_check_update();
                        break;
                    case PROC_COMMAND:
                        set_title("itl_node-COMMAND");

                        // 监视node的ip，有变动则重启
                        monitor_node_ip(0, 0);

                        init_log_archive(config_get_strval("log_dir", "./log/"), 2 * 3600);
                        break;
                    case PROC_MYSQL:
                        set_title("itl_node-MYSQL");


                        init_connect_to_db();
                        init_mysql_instance();
                        break;
                    default:
                        break;
                }
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
                switch (get_work_idx())
                {
                    case PROC_PROXY:
                        break;
                    case PROC_COLLECT:
                        // 采集进程
                        fini_collect();
                        break;
                    case PROC_COMMAND:
                        fini_log_archive();
                        break;
                    case PROC_MYSQL:
                        fini_mysql_instance();
                        break;
                    default:
                        break;
                }
                destroy_timer();
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

    if (1 == cmd / 0x1000)
    {
        // 给数据库管理进程
        return PROC_MYSQL;
    }

    switch (cmd)
    {
        case node_p_notify_config_change_cmd:
        case node_p_node_info_cmd:
        case node_p_restart_node_by_collect_cmd:
        case node_p_switch_collect_cmd:
        case node_p_switch_auto_update_cmd:
        case node_p_check_update_cmd:
            // 给collect进程
            return PROC_COLLECT;
        case node_p_restart_node_cmd:
            return PROC_COMMAND;
        case node_p_download_file_cmd:
        case node_p_update_so_cmd:
        case node_p_update_metric_so_cmd:
        case node_p_update_bench_conf_cmd:
        case node_p_execute_command_cmd:
        case node_p_upload_file_by_ftp_cmd:
            return PROC_COMMAND;
        case node_p_notify_mysql_instance_change_cmd:
            return PROC_MYSQL;
        default:
            // 丢弃，并记录日志
            return -2;
    }

    return -1;
}


int shmq_pushed(int fd, const char * buf, uint32_t len, int flag)
{
    return 0;
}


void proc_pkg_cli(int fd, const char * buf, uint32_t len)
{
    if (PROC_MYSQL == get_work_idx())
    {
        dispatch_db_mgr(fd, buf, len);
    }
    else
    {
        dispatch(fd, buf, len);
    }
}


void proc_pkg_ser(int fd, const char * buf, uint32_t len)
{
    if (g_head->check_fd(fd))
    {
        INFO_LOG("recv pkg from head, len: %u", len);
        g_head->dispatch(dispatch_head, buf, len);
    }
    else if (g_db->check_fd(fd))
    {
        INFO_LOG("recv pkg from db, len: %u", len);
        g_db->dispatch(dispatch_db, buf, len);
    }
    else if (g_control->check_fd(fd))
    {
        INFO_LOG("recv pkg from control, len: %u", len);
        g_control->dispatch(dispatch_control, buf, len);
    }
    else if (g_node->check_fd(fd))
    {
        INFO_LOG("recv pkg from node, len: %u", len);
        g_node->dispatch(dispatch_node, buf, len);
    }

}


void link_up_cli(int fd, uint32_t ip)
{

}


void link_down_cli(int fd)
{

}


void link_down_ser(int fd)
{
    DEBUG_LOG("server link down, fd: %d", fd);

    if (g_head->check_fd(fd))
    {
        g_head->on_server_down();
    }
    else if (g_db->check_fd(fd))
    {
        g_db->on_server_down();
    }
    else if (g_control->check_fd(fd))
    {
        g_control->on_server_down();
    }
    else if (g_node->check_fd(fd))
    {
        g_node->on_server_down();
    }

}
