extern "C"
{
#include <libtaomee/log.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}
#include "proto_head.hpp"
#include "ip_dict.hpp"

#include "seer_sendcard.hpp"
#include "fairy_sendcard.hpp"
#include "mole_sendcard.hpp"
#include "gongfu_sendcard.hpp"
#include "hero_sendcard.hpp"

CSendCardBase *g_sender = NULL;

extern "C" void proc_events()
{
}

extern "C" int proc_pkg_from_client(void* pkg, int pkglen, fdsession_t* fdsess)
{
    DEBUG_LOG("proc client len=%d", pkglen);
    g_sender->onProcPkgCli(pkg, pkglen, fdsess);
    return 0;
}

extern "C" void proc_pkg_from_serv(int fd, void* pkg, int pkglen)
{
    DEBUG_LOG("proc server len=%d", pkglen);
    g_sender->onProcPkgSer(fd, pkg, pkglen);
}

/*
extern "C" void proc_mcast_pkg(const void* data, int len)
{
}
*/

extern "C" void on_client_conn_closed(int fd)
{
}

extern "C" void on_fd_closed(int fd)
{
    g_sender->onLinkDown(fd);
}

extern "C" int init_service(int isparent)
{
    if (!isparent)
    {
        char* name = config_get_strval("project");
        if (name == NULL)
        {
            ERROR_LOG("bench.conf can not find 'project' item");
            return -1;
        }

        if (strcmp(name, "seer") == 0)
        {
            g_sender = new CSeerSendCard();
        }
        else if (strcmp(name, "fairy") == 0)
        {
            g_sender = new CFairySendCard();
        }
        else if (strcmp(name, "mole") == 0)
        {
            g_sender = new CMoleSendCard();
        }
        else if (strcmp(name, "gongfu") == 0)
        {
            g_sender = new CGongfuSendCard();
        }
        else if (strcmp(name, "hero") == 0)
        {
            g_sender = new CHeroSendCard();
        }
        else
        {
            ERROR_LOG("bench.conf can not support 'project' item");
            return -1;
        }

        if (!g_sender->init())
            return -1;
        
        if (!g_ipDict.init())
            return -1;
    }

    return 0;
}

extern "C" int fini_service(int isparent)
{
    if (!isparent)
    {
        delete g_sender;
    }
    return 0;
}

extern "C" int get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{
    if (avail_len < 4)
        return 0;

    uint32_t len = *(uint32_t*)(avail_data);
    if ((len > 1024) || (len < PROTO_HEAD_LEN))
    {
        ERROR_LOG("invalid len=%d from fd=%d", len, fd);
        return -1;
    }

    return len;
}

/*
extern "C" int proc_udp_pkg(int fd, const void* avail_data, int avail_len, 
                            struct sockaddr_in *remote_addr)
{
}
*/

