#include <time.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <async_server.h>
#include <log.h>

extern "C" {

int plugin_init(int type)
{
    DEBUG_LOG("INIT...");

    if (PROC_WORK == type)
    {
        net_connect_ser("10.1.6.245", 10002, 0);

    }
    return 0;
}

int plugin_fini(int type)
{
    DEBUG_LOG("FINI...");
    return 0;
}

void time_event()
{
//    INFO_LOG("time out");
}

int get_pkg_len_cli(const char *buf, uint32_t len)
{
    //    INFO_LOG("get_pkg_len_cli %u", len);
    return len;
}

int get_pkg_len_ser(const char *buf, uint32_t len)
{
    //    INFO_LOG("get_pkg_len_ser %u", len);
    return len;
}

void link_up_cli(int fd, uint32_t ip)
{
    //DEBUG_LOG("LINKUP CLI ip:%x fd:%d", ip, fd);
    //    DEBUG_LOG("LINKUP CLI ip:%s fd:%d", inet_ntoa(*((struct in_addr *)&ip)), fd);
}

void link_down_cli(int fd)
{
    DEBUG_LOG("LINKDOWN CLI fd:%d", fd);
}

void proc_pkg_cli(int fd, const char *buf, uint32_t len)
{
    DEBUG_LOG("HANDLE CLI fd:%d, len:%u", fd, len);
    /* net_send_cli(fd, buf, len); */
}

void link_down_ser(int fd)
{
    DEBUG_LOG("LINKDOWN SER fd:%d", fd);
}

void proc_pkg_ser(int fd, const char *buf, uint32_t len)
{
    DEBUG_LOG("HANDLE SER fd:%d, len:%u", fd, len);
    /* net_send_cli(fd, buf, len); */
}

}

