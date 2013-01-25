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

#include <unistd.h>
#include <libtaomee/log.h>
#include "dispatch.h"
#include "plugin_interface.h"
#include "rrd.h"

#define MAX_PROC_NAME_LEN 128

int g_is_switch_rrd = 0;
const char *g_p_host_name = NULL;
const char *g_p_metric_name = NULL;


int plugin_init(int type)
{
    switch (type)
    {
    case PROC_MAIN:
        break;
    case PROC_WORK:
    {
        const char * rrd_dir = NULL;
        if (NULL == (rrd_dir = config_get_strval("rrd_base_dir", NULL)))
        {
            ERROR_LOG("Init ERROR: Cannot get config of rrd_base_dir");
            return -1;
        }

        if (0 != init_rrd_handler(rrd_dir))
        {
            return -1;
        }
        g_is_switch_rrd = config_get_intval("is_switch_rrd", 0);
        g_p_host_name = config_get_strval("log_host_name", NULL);
        g_p_metric_name = config_get_strval("log_metric_name", NULL);

        const char *p_proc_name = config_get_strval("proc_name", "itl_rrd");
        uint32_t work_id = get_work_id();
        set_title("%s-WORK%u", p_proc_name, work_id);

        break;
    }
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
            fini_rrd_handler();
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

}


int get_pkg_len_cli(const char * buf, uint32_t len)
{

    return len;
    if (len < 4) {
        return 0;
    }

    const rrd_proto_t *p_rrd_pkg = reinterpret_cast<const rrd_proto_t *>(buf);
    uint32_t pkg_len = p_rrd_pkg->len;
    if (pkg_len > PROTO_MAX_LEN || pkg_len < sizeof(rrd_proto_t)){
        ERROR_LOG("Invalid package length: %u", pkg_len);
        return -1;
    }

    return pkg_len;

}

int get_pkg_len_ser(const char * buf, uint32_t len)
{
    return 0;
}


int check_open_cli(uint32_t ip, uint16_t port)
{
    return 0;
}

int select_channel(int fd, const char * buf, uint32_t len, uint32_t ip, uint32_t work_num)
{
    const rrd_proto_t *p_rrd_pkg = reinterpret_cast<const rrd_proto_t *>(buf);
    return p_rrd_pkg->id % work_num;
}


int shmq_pushed(int fd, const char * buf, uint32_t len, int flag)
{
    return 0;
}


void proc_pkg_cli(int fd, const char * buf, uint32_t len)
{
    DEBUG_LOG("len: %u, buf: %s", len, buf);
    char rsp[] = {"test"};
    net_send_cli(fd, rsp, strlen(rsp));
    return;
    dispatch(fd, buf, len);
}


void proc_pkg_ser(int fd, const char * buf, uint32_t len)
{

}


void link_up_cli(int fd, uint32_t ip)
{

}


void link_down_cli(int fd)
{

}


void link_down_ser(int fd)
{

}
