/**
 *============================================================
 *  @file      switch.cpp
 *  @brief
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

extern "C" {
#include <arpa/inet.h>
#include <libtaomee/log.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>
}
#include "init.h"
#include "net.h"
#include "proto.h"
#include "dbproxy.h"
#include "ip_counter.h"
#define PKG_SIZE   8192
extern "C" {
#include <libtaomee/dirtyword/dirtyword.h>
#include <libtaomee/tm_dirty/tm_dirty.h>
}
/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  init_service(int isparent)
{
    if(!isparent) {
        setup_timer();
        read_conf();
        ip_counter_init();
        if(load_lib()){
            ERROR_LOG("Failed to Load business lib!");
            return -1;
        }
        process_service_init();
		if (tm_load_dirty("./data/tm_dirty.dat") < 0) {
			ERROR_LOG("Failed to load dirty word!");
			return -1;
		}

    }
	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  fini_service(int isparent)
{
	if (!isparent) {
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
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{	
    const char policy[] =   "<policy-file-request/>";
    const char response[] = "<?xml version=\"1.0\"?>"
                            "<!DOCTYPE cross-domain-policy SYSTEM\"/xml/dtds/cross-domain-policy.dtd\">"
                            "<cross-domain-policy>"
                            "<site-control permitted-cross-domain-policies=\"all\"/>"
                            "<allow-access-from domain=\"*\"to-ports=\"*\"/>"
                            "</cross-domain-policy>";

    /* must has 4 bytes*/
    if (avail_len< 4) {
        return SUCCESS;
    }

    /*if is policy request, send response to flash client*/
    if(isparent) {
        uint32_t reallen;

        if (avail_len == 23 && !memcmp(avail_data, policy, 23)) {
            //DEBUG_LOG("REQ POLICY FILE %d", fd);
            net_send(fd, response, sizeof(response));
            return SUCCESS;
        }

        /*get real package length*/
        const cli_login_pk_header* clipkg = reinterpret_cast<const cli_login_pk_header*>(avail_data);
        reallen = ntohl(clipkg->length);
        //DEBUG_LOG("RECV CLIENT %d %d %d", avail_len, reallen, fd);
        if (reallen < sizeof(cli_login_pk_header)|| reallen > MAX_LENGTH) {
            ERROR_LOG("invalid length %u from fd %d", reallen, fd);
            return CLOSE_CONN;
        }

        return reallen;

    } else {
        return process_pkg_len(fd, avail_data, avail_len);
    }
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
    if (fd == mainlogin_dbproxysvr_fd) {
        handle_db_return(reinterpret_cast<svr_proto_t*>(data), len);
    } else {
        process_serv_return(fd, data, len);
    }
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
    usr_info_t* p = get_usr_by_fd(fd);
    clear_usr_info(p);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_fd_closed(int fd)
{
    if (fd == mainlogin_dbproxysvr_fd) {
        DEBUG_LOG("DB PROXY CONNECTION CLOSED\t[fd=%d]", mainlogin_dbproxysvr_fd);
#ifndef TW_VER
        send_warning(0, 5, inet_addr(g_ds_ini.mainlogin_dbproxy_ip));
#endif
        mainlogin_dbproxysvr_fd = -1;
    } else {
        process_serv_fd_closed(fd);
    }
}

