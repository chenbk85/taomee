/**
 * @file echo_serv.cpp
 * @brief echo服务
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-06
 */
#include "../../library/base/log.h"
#include "../net_if.h"
#include "../async_serv.h"

void* g_p_timer = NULL;

void timer_test(void* arg)
{
    DEBUG_LOG("REPEAT TIMER expired");
}

extern "C" int init_service(int argc, char** argv, int proc_type)
{
	DEBUG_LOG("INIT...");
    if (proc_type == PROC_WORK) {
        g_p_timer = register_timer(timer_test, NULL, time(NULL) + 10, kTIMER_REPEAT, 2);
    }
	return 0;
}

extern "C" int fini_service(int proc_type)
{
	DEBUG_LOG("FINI...");
    if (proc_type == PROC_WORK) {
        if (g_p_timer) {
            remove_timer(g_p_timer);
        }
    }
	return 0;
}

extern "C" int	get_pkg_len(void* avail_data, int avail_len)
{
    int pkg_len = 16 * 1024;
    if (avail_len < pkg_len) {
        return 0;
    }
    return pkg_len;
}

extern "C" int proc_pkg_from_client(void* data, int len, fdsession_t* cli_fdsess)
{
	tcp_net_send(cli_fdsess->id, data, len);
	return 0;
}

extern "C" int proc_pkg_from_serv(void* pkg, int pkglen, fdsession_t* serv_fdsess)
{
    return 0;
}

extern "C" int on_serv_conn_complete(fdsession_t* serv_fdsess)
{
    return 0;
}

extern "C" int on_serv_conn_failed(void* connector_handler)
{
    return 0;
}

extern "C" void on_client_conn_closed(fdsession_t* cli_fdsess)
{
    DEBUG_LOG("connection[%u] close connection", cli_fdsess->id);
}

extern "C" void on_serv_conn_closed(fdsession_t* serv_fdsess)
{ }

extern "C" int proc_udp_pkg_from_client(void* avail_data, int avail_len, fdsession_t* fdsess)
{
    if (avail_len > 0) {
        DEBUG_LOG("udp [%s][%d] recv data len: %d", fdsess->ip_addr.c_str(), fdsess->port, avail_len);
        udp_net_send(fdsess->id, fdsess->ip_addr, fdsess->port, avail_data, avail_len);
    }
    return 0;
}

extern "C" int proc_udp_pkg_from_serv(void* avail_data, int avail_len, fdsession_t* fdsess)
{
    return 0;
}

extern "C" int on_udp_serv_conn_complete(fdsession_t* serv_fdsess)
{
    return 0;
}

