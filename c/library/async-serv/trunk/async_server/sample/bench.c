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
    return 0;
}

int plugin_fini(int type)
{
    DEBUG_LOG("FINI...");
    return 0;
}

void time_event()
{
    //DEBUG_LOG("time out");
}

int get_pkg_len_cli(const char *buf, uint32_t len)
{
    if (len < 4)
        return 0;

    return *(int*)(buf);
}

int get_pkg_len_ser(const char *buf, uint32_t len)
{
    if (len < 4)
        return 0;

    return *(int*)(buf);
}

int check_open_cli(uint32_t ip, uint16_t port)
{
    DEBUG_LOG("OPEN CLI ip:%s port:%d", inet_ntoa(*(in_addr *)&ip), htons(port));
    //test firewall
    //return -1;

    return 0;
}

int select_channel(int fd, const char *buf, uint32_t len, uint32_t ip, uint32_t work_num)
{
    //DEBUG_LOG("SELECT ip:%s", inet_ntoa(*(in_addr *)&ip));
    //for ip
    //return ip % work_num;
    //for fd
    return fd % work_num;
}

void proc_pkg_cli(int fd, const char *buf, uint32_t len)
{
    DEBUG_LOG("HANDLE CLI fd:%d, len:%u", fd, len);
    net_send_cli(fd, buf, len);
}

void proc_pkg_ser(int fd, const char *buf, uint32_t len)
{
    DEBUG_LOG("HANDLE SER fd:%d, len:%u", fd, len);
    net_send_ser(fd, buf, len);
}

void link_up_cli(int fd, uint32_t ip)
{
    DEBUG_LOG("LINKUP CLI ip:%s fd:%d", inet_ntoa(*(in_addr *)&ip), fd);
}

void link_down_cli(int fd)
{
    DEBUG_LOG("LINKDOWN CLI fd:%d", fd);
}

void link_down_ser(int fd)
{
    DEBUG_LOG("LINKDOWN SER fd:%d", fd);
}

}
