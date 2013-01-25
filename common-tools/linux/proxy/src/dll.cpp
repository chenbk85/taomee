extern "C"
{
#include <libtaomee/list.h>
#include <libtaomee/timer.h>
#include <libtaomee/log.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/project/utilities.h>
}
#include <libtaomee++/inet/pdumanip.hpp>

#include "common.hpp"
#include "CProxyRoute.hpp"
#include "CProxyDealBase.hpp"
#include "CProxyClientManage.hpp"

using namespace taomee;

/**
  * @brief Initialize service
  */
//CProxyRoute g_proxy_route;
extern "C" int init_service(int isparent)
{
    if (!isparent) {
		srand(time(0));
		setup_timer();

		g_proxy_route.reload_svr_config(config_get_strval("route_file"));
		g_proxy_route.reload_security_config(config_get_strval("security_file"));
    }

    return 0;
}

/**
  * @brief Finalize service
  */
extern "C" int fini_service(int isparent)
{
    if (!isparent) {

    }

    return 0;
}

/**
  * @brief Process events such as timers and signals
  */
extern "C" void proc_events()
{
	//proc_cached_pkgs();
	handle_timer();
}

/**
  * @brief Return length of the receiving package
  */
extern "C" int get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{
    if (avail_len < 4) {
        ERROR_LOG("avail_len len=%d from fd=%d", avail_len, fd);
        return 0;
	}

	int len = -1;
	if (isparent) {
		len = *(uint32_t*)(avail_data);
	} else {
		/*svr_data_t* p_mdd_svr = g_proxy_route.get_svr(game_mdd, zone_tel, svr_dbproxy);
		if (p_mdd_svr && fd == p_mdd_svr->fd) {
			len = taomee::bswap(*(uint32_t*)(avail_data));	
		} else {
   	 		len = *(uint32_t*)(avail_data);
		}*/
   	 	len = *(uint32_t*)(avail_data);
	}

    if ((len > max_pkg_len) || (len < min_header_len))
    {
        ERROR_LOG("invalid len=%d from fd=%d", len, fd);
        return -1;
    }

    return len;
}

/**
  * @brief Process packages from clients
  */
extern "C" int proc_pkg_from_client(void* pkg, int pkglen, fdsession_t* fdsess)
{
    //DEBUG_LOG("proc client len=%d", pkglen);
	return g_proxy_route.dispatch(pkg, fdsess, pkglen);
}

/**
  * @brief Process packages from servers
  */
extern "C" void proc_pkg_from_serv(int fd, void* pkg, int pkglen)
{
    //DEBUG_LOG("proc server len=%d", pkglen);
	g_proxy_route.handle_svr_return(fd, pkg, pkglen);
}

/**
  * @brief Called to process mcast package from the address and port configured in the config file
  */
extern "C" void proc_mcast_pkg(const void* data, int len)
{

}


/**
  * @brief Called each time on client connection closed
  */
extern "C" void on_client_conn_closed(int fd)
{
	DEBUG_LOG("client close fd=%d", fd);
	g_clients.del_clients(fd);
}

/**
  * @brief Called each time on close of the fds created by the child process
  */
extern "C" void on_fd_closed(int fd)
{
	DEBUG_LOG("fd close fd=%d", fd);
	g_proxy_route.svr_fd_closed(fd);
}


