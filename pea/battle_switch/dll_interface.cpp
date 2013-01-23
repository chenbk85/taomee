/** 
 * ========================================================================
 * @file dll_interface.cpp
 * @brief 
 * @version 1.0
 * @date 2011-11-14
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <stdio.h>
#include <string.h>


extern "C" {
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
}

#include "common.hpp"
#include "dispatch.hpp"
#include "proto.hpp"


#ifdef USE_HEX_LOG_PROTO
int g_log_send_buf_hex_flag = 1;
#else
int g_log_send_buf_hex_flag = 0;
#endif


extern "C" int init_service(int isparent)
{
    if (isparent) {

    } else {
	/* MYTEST(zog): 测试 不分时log 和 TRACE_TLOG */
#if 1
		SET_TIME_SLICE_SECS(0);
		SET_LOG_LEVEL(tlog_lvl_trace);
#endif
        setup_timer();
        clear_online();
    }

    return 0;
}

extern "C" int fini_service(int isparent)
{
    if (isparent) {
	} else {
		destroy_timer();
		clear_online();
	}
    return 0;
}

extern "C" void proc_events()
{
    proc_cached_pkg();
	route_online_timer();
}

extern "C" int get_pkg_len(int fd, const void * avail_data, int avail_len, int isparent)
{
    if (avail_len < 4) {
        return 0;
    }

    const btlsw_proto_t * pkg = (const btlsw_proto_t *)avail_data;
    uint32_t pkg_len = pkg->len;

    if (pkg_len > btlsw_proto_max_len || pkg_len < sizeof(btlsw_proto_t)) {
		ERROR_TLOG("invalid len: %u from fd: %d", pkg_len, fd);
        return -1;
    }

    if (pkg_len <= (uint32_t)avail_len) {
        return pkg_len;
    } else {
        return 0;
    }
}


extern "C" int proc_pkg_from_client(void * data, int len, fdsession_t * fdsess)
{

#ifdef USE_HEX_LOG_PROTO
    if (g_log_send_buf_hex_flag) {
        char outbuf[13000];
        bin2hex(outbuf, (char *)data, len, 2000);
        DEBUG_TLOG("[IN] %s", outbuf);
    }
#endif

    return dispatch(data, fdsess, true);
}


extern "C" void on_client_conn_closed(int fd)
{
#ifndef IGNORE_SERVER_OFF
    DEBUG_TLOG("FD %d CLOSED", fd);

    del_online_by_fd(fd);
#endif
}



extern "C" void proc_pkg_from_serv(int fd, void * data, int len)
{

}



extern "C" void on_fd_closed(int fd)
{

}
