extern "C" 
{
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
}

#include "utils.hpp"
#include "Online.hpp"
#include "Player.hpp"
#include "Home.hpp"
#include "Map.hpp"
#include "cli_proto.hpp"
#include "fwd_decl.hpp"


/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  init_service(int isparent)
{
	if (!isparent) 
	{
		srand(time(0));
		setup_timer();

		g_online = new OnlineManage();
		g_home = new HomeManage();
		g_player = new GlobalPlayer();
		Online::init_handles();
	}

	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  fini_service(int isparent)
{
	if (!isparent) 
	{
		destroy_timer();
		SAFE_DELETE(g_online);
		SAFE_DELETE(g_home);
		SAFE_DELETE(g_player);
	}
	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void proc_events()
{
	handle_timer();
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{	
	static char request[]  = "<policy-file-request/>";
	static char response[] = "<?xml version=\"1.0\"?>"
								"<!DOCTYPE cross-domain-policy>"
								"<cross-domain-policy>"
								"<allow-access-from domain=\"*\" to-ports=\"*\" />"
								"</cross-domain-policy>";

	if (avail_len < 4) {
		return 0;
	}

	int len = -1;
	if (isparent) {
		// the client requests for a socket policy file
		if ((avail_len == sizeof(request)) && !memcmp(avail_data, request, sizeof(request))) {
			net_send(fd, response, sizeof(response));

			TRACE_LOG("Policy Req [%s] Received, Rsp [%s] Sent", request, response);
			return 0;
		}

		const cli_proto_t* pkg = reinterpret_cast<const cli_proto_t*>(avail_data);
		

		len = taomee::bswap(pkg->len);
		DEBUG_LOG("len=[%d],comid=[%d],userid=[%d],seqnum=[%d],ret=[%d]",
				len,pkg->cmd,pkg->id,pkg->seq,pkg->ret);
		if ((len > cli_proto_max_len) || (len < static_cast<int>(sizeof(cli_proto_t)))) {
			ERROR_LOG("p: invalid len=%d from fd=%d", len, fd);
			return -1;
		}
	} else {
		/*len = *reinterpret_cast<const uint32_t*>(avail_data);
		if (fd == proxysvr_fd) {
			if ((len > dbproto_max_len) || (len < static_cast<int>(sizeof(db_proto_t)))) {
				ERROR_LOG("c: invalid len=%d from fd=%d", len, fd);
				return -1;
			}
		} else {
			if ((len > btlproto_max_len) || (len < static_cast<int>(sizeof(btl_proto_t)))) {
				ERROR_LOG("c: invalid len=%d from fd=%d", len, fd);
				return -1;
			}
		}*/
	}

	return len;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	return g_online->dispatch(data, fdsess);
	//return dispatch(data, fdsess);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void proc_pkg_from_serv(int fd, void* data, int len)
{
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
	g_online->deallocOnline(fd);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_fd_closed(int fd)
{
	/*if (fd == proxysvr_fd) 
	{
		DEBUG_LOG("DB PROXY CONNECTION CLOSED\t[fd=%d]", proxysvr_fd);
		proxysvr_fd = -1;
	}*/
}

/**
  * @brief Called to process mcast package from the address and port configured in the config file
  */
extern "C" void proc_mcast_pkg(const void* data, int len)
{
	/*DEBUG_LOG("here1");
	const mcast_pkg_t* pkg = reinterpret_cast<const mcast_pkg_t*>(data);
	if (pkg->server_id != get_server_id()) {
		switch (pkg->main_cmd) {
		case mcast_reload_conf:
			reload_conf_op(pkg->minor_cmd, pkg->body, len - sizeof(mcast_pkg_t));
			break;
		default:
			ERROR_LOG("unsurported cmd %u", pkg->main_cmd);
			break;
		}
	}*/
}

