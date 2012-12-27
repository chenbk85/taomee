extern "C" 
{
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
}

#include "dbproxy.hpp"
#include "dispatcher.hpp"
#include "mcast_proto.hpp"
#include "reload_conf.hpp"
#include "online.hpp"
#include "switch.hpp"

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  init_service(int isparent)
{
	if (!isparent) 
	{
		setup_timer();
		Online::init();
		init_funcs();
        init_global_data();
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
		Online::fini();
        fini_global_data();
		destroy_timer();
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

    process_random_event();
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{	
	if (avail_len < 4) 
	{
		return 0;
	}

	const svr_proto_t* pkg = reinterpret_cast<const svr_proto_t*>(avail_data);
	if ((pkg->len > pkg_size) || (pkg->len < sizeof(svr_proto_t))) 
	{
		ERROR_LOG("invalid len=%u from fd=%d", pkg->len, fd);
		return -1;
	}

	return pkg->len;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	return dispatch(data, fdsess);
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
	Online::clear_online_info(fd);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_fd_closed(int fd)
{
	if (fd == proxysvr_fd) 
	{
		DEBUG_LOG("DB PROXY CONNECTION CLOSED\t[fd=%d]", proxysvr_fd);
		proxysvr_fd = -1;
	}
}

/**
  * @brief Called to process mcast package from the address and port configured in the config file
  */
extern "C" void proc_mcast_pkg(const void* data, int len)
{
	DEBUG_LOG("here1");
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
	}
}

