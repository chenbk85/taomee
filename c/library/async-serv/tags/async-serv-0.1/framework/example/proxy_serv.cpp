/**
 * @file proxy_serv.cpp
 * @brief proxy_serv示例
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-07
 */
#include <string>

#include "../../library/base/log.h"
#include "../net_if.h"
#include "../async_serv.h"

std::string g_serv_ip;
u_short g_serv_port = 0;
void* g_p_serv_handler = NULL;
uint32_t g_serv_conn_id = 0;

extern "C" int init_service(int argc, char** argv, int proc_type)
{
	DEBUG_LOG("INIT...");
    if (argc != 4) {
        return -1;
    }

    if (proc_type == PROC_WORK) {
        g_serv_ip = argv[2];
        g_serv_port = atoi(argv[3]);

        g_p_serv_handler = tcp_connect_to_svr(g_serv_ip, g_serv_port);
        if (!g_p_serv_handler) {
            ERROR_LOG("tcp_connect_to_svr failed");
            return -1;
        }
    }
	return 0;
}

extern "C" int fini_service(int proc_type)
{
	DEBUG_LOG("FINI...");
    if (proc_type == PROC_WORK) {
        if (g_p_serv_handler) {
            tcp_close_serv_connection(g_p_serv_handler);
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
    uint32_t* p = (uint32_t*)data;
    *p = cli_fdsess->id;

    if (g_serv_conn_id > 0) {
        tcp_net_send(g_serv_conn_id, p, len);
    }
	return 0;
}

extern "C" int proc_pkg_from_serv(void* pkg, int pkglen, fdsession_t* serv_fdsess)
{
    uint32_t* p = (uint32_t*) pkg;
	tcp_net_send(*p, pkg, pkglen);
    return 0;
}

extern "C" int on_serv_conn_complete(fdsession_t* serv_fdsess)
{
    g_serv_conn_id = serv_fdsess->id;
    return 0;
}

extern "C" int on_serv_conn_failed(void* connector_handler)
{
    DEBUG_LOG("serv connection failed");
    if (g_p_serv_handler == connector_handler) {
        tcp_close_serv_connection(connector_handler);
        g_p_serv_handler = NULL;
    }

    return 0;
}

extern "C" void on_client_conn_closed(fdsession_t* cli_fdsess)
{
    DEBUG_LOG("client connection[%u] close connection", cli_fdsess->id);
}

extern "C" void on_serv_conn_closed(fdsession_t* serv_fdsess)
{
    DEBUG_LOG("serv connection[%u] close connection", serv_fdsess->id);
}

extern "C" int proc_udp_pkg_from_client(void* avail_data, int avail_len, fdsession_t* fdsess)
{
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

