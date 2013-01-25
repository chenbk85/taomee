/** 
 * ========================================================================
 * @file plugin_interface.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-31
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */
#include "alarm.h"
#include "plugin_interface.h"
#include "itl_common.h"
#include "dispatch.h"
#include "node.h"
#include "db.h"
#include "mobile_ban.h"
#include "log_archive.h"



int plugin_init(int type)
{
    switch (type)
    {
        case PROC_MAIN:
            break;
        case PROC_WORK:
            {
                setup_timer();
                init_mobile_ban();
                INIT_LIST_HEAD(&g_awaiting_node);
                init_connect_to_db();
                init_nodes();

                if (0 == get_work_idx())
                {
                    init_log_archive(config_get_strval("log_dir", "./log/"), 2 * 3600);
                }
            }
            break;
        case PROC_CONN:
            break;
        default:
            break;
    }
    return 0;

}


int plugin_fini(int type)
{
    switch (type)
    {
        case PROC_MAIN:
            break;
        case PROC_WORK:
            {
                if (0 == get_work_idx())
                {
                    fini_log_archive();
                }
                fini_nodes();
                list_del_init(&g_awaiting_node);
                fini_mobile_ban();
                destroy_timer();
            }
            break;
        case PROC_CONN:
            break;
        default:
            break;
    }
    return 0;
}


void time_event()
{
    proc_cached_pkg();
    handle_timer();

}


int get_pkg_len_cli(const char * buf, uint32_t len)
{
    if (len < 4)
    {
        return 0;
    }

    const alarm_proto_t * pkg = reinterpret_cast<const alarm_proto_t *>(buf);
    uint32_t pkg_len = pkg->len;

    if (pkg_len > PROTO_MAX_LEN || pkg_len < sizeof(head_proto_t))
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
    const alarm_proto_t * pkg = reinterpret_cast<const alarm_proto_t *>(buf);
    uint32_t uid = pkg->id;
    return (uid % work_num);
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
    if (g_db->check_fd(fd))
    {
        g_db->dispatch(dispatch_db, buf, len);
    }

}


void link_up_cli(int fd, uint32_t ip)
{
    DEBUG_LOG("client link up, fd: %d", fd);

}


void link_down_cli(int fd)
{
    DEBUG_LOG("client link down, fd: %d", fd);

    handle_head_down(fd);
}


void link_down_ser(int fd)
{
    DEBUG_LOG("server link down, fd: %d", fd);

    if (g_db->check_fd(fd))
    {
        send_db_alarm();
        g_db->on_server_down();
    }
}
