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

#include "dispatch.h"
#include "node.h"
#include "rrd.h"
#include "alarm.h"
#include "db.h"


int plugin_init(int type)
{
    switch (type)
    {
        case PROC_MAIN:
            break;
        case PROC_WORK:
            {
                setup_timer();
                init_connect_to_rrd();
                init_connect_to_db();
                init_connect_to_alarm();
                init_nodes();
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
                fini_nodes();
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
    handle_timer();

}


int get_pkg_len_cli(const char * buf, uint32_t len)
{
    if (len < 4)
    {
        return 0;
    }

    const head_proto_t * pkg = reinterpret_cast<const head_proto_t *>(buf);
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
    const head_proto_t * pkg = reinterpret_cast<const head_proto_t *>(buf);
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
    if (g_alarm->check_fd(fd))
    {
        g_alarm->dispatch(dispatch_alarm, buf, len);
    }

}


void link_up_cli(int fd, uint32_t ip)
{
    DEBUG_LOG("client link up, fd: %d", fd);

}


void link_down_cli(int fd)
{
    DEBUG_LOG("client link down, fd: %d", fd);
    c_node * p_node = find_node_by_fd(fd);
    if (NULL != p_node)
    {
        node_down(p_node);
    }

}


void link_down_ser(int fd)
{
    DEBUG_LOG("server link down, fd: %d", fd);
    
    if (g_rrd->check_fd(fd))
    {
        g_rrd->on_server_down();
    }
    else if (g_alarm->check_fd(fd))
    {
        g_alarm->on_server_down();
    }
    else if (g_db->check_fd(fd))
    {
        g_db->on_server_down();
    }

}
