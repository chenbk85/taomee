extern "C"
{
#include <libtaomee/log.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}
#include "proto_head.hpp"
#include "ip_dict.hpp"
#include "card_sender.hpp"

CCardSender *g_sender = NULL;

extern "C" void proc_events()
{
    g_sender->checkTimeout();
}

extern "C" int proc_pkg_from_client(void* pkg, int pkglen, fdsession_t* fdsess)
{
    g_sender->onProcPkgCli(pkg, pkglen, fdsess);
    return 0;
}

extern "C" void proc_pkg_from_serv(int fd, void* pkg, int pkglen)
{
    g_sender->onProcPkgSer(fd, pkg, pkglen);
}

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
        g_sender = new CCardSender();

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
    if ((len > 1024) || (len < ONLINE_PROTO_HEAD_LEN))
    {
        ERROR_LOG("invalid len=%d from fd=%d", len, fd);
        return -1;
    }
    
    return len;
}

