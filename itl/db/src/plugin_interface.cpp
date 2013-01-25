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

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <libtaomee/log.h>


#include "db_interface.h"
#include "plugin_interface.h"
#include "itl_common.h"
#include "dispatch.h"
#include "log_archive.h"
#include "config.h"



int plugin_init(int type)
{
    switch (type)
    {
        case PROC_MAIN:
            break;
        case PROC_WORK:
        {
            if (0 != init_db(config_get_strval("DB_HOST", "0.0.0.0"),
                        config_get_intval("DB_PORT", 3306),
                        config_get_strval("DB_NAME", "db_itl"), 
                        config_get_strval("DB_USER", "root"),
                        config_get_strval("DB_PASS", "")))
            {
                return -1;
            }

            const char *p_proc_name = config_get_strval("proc_name", "itl_db");
            uint32_t work_id = get_work_id();
            set_title("%s-WORK%u", p_proc_name, work_id);

            if (0 == get_work_idx())
            {
                setup_timer();
                init_log_archive(config_get_strval("log_dir", "./log/"), 2 * 3600);
            }

            break;
        }
        case PROC_CONN:
            {
                init_allow_ip(config_get_strval("allow_client_ip", "*"));
            }
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
            if (0 == get_work_idx())
            {
                fini_log_archive();
                destroy_timer();
            }
            fini_db();
            break;
        case PROC_CONN:
            {
                fini_allow_ip();
            }
            break;
        default:
            break;
    }
    return 0;
}


void time_event()
{
    if (0 == get_work_idx())
    {
        handle_timer();

    }

}


int get_pkg_len_cli(const char * buf, uint32_t len)
{
    if (len < 4)
    {
        return 0;
    }

    const db_proto_t * pkg = reinterpret_cast<const db_proto_t *>(buf);
    uint32_t pkg_len = pkg->len;

    if (pkg_len > PROTO_MAX_LEN || pkg_len < sizeof(db_proto_t))
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
    if (is_allowed_ip(ip))
    {
        return 0;
    }
    else
    {
        WARN_LOG("forbidden ip: %s", long2ip(ip));
        return -1;
    }
}

int select_channel(int fd, const char * buf, uint32_t len, uint32_t ip, uint32_t work_num)
{
    return -1;
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
