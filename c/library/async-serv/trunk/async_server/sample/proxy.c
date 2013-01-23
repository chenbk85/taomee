#include <time.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <async_server.h>
#include <log.h>
#include <vector>

struct protocol_t {
    uint32_t len;
    uint32_t seq;
    uint8_t buf[];
}__attribute__((packed));

struct exchange_info_t {
    bool is_use;
    int cli_sock;
    int svr_sock;
    uint32_t old_seq;
};
std::vector<exchange_info_t> g_exchange_tbl(65536 * 8);

int backend_fd = -1;

int make_seq()
{
    static int idx = 0;
    for (uint32_t i = 0; i < g_exchange_tbl.size(); ++i) {
        if (idx >= (int)g_exchange_tbl.size())
            idx = 0;

        if (g_exchange_tbl[idx].is_use) {
            ++idx;
            continue;
        }

        return idx++;
    }

    return -1;
}

void do_exchange_cli(int fd, const char *buf, int len)
{
    protocol_t *pp = (protocol_t *)buf;

    int seq = make_seq();
    if (-1 == seq) {
        ERROR_LOG("make_seq empty");
        net_send_cli(fd, buf, len);
        return;
    }

    if (backend_fd == -1) {
        backend_fd = net_connect_ser(config_get_strval("backend_ip", ""), config_get_intval("backend_port", 0), 1000);
        if (backend_fd == -1) {
            ERROR_LOG("can't connect 10.1.1.122");
            net_send_cli(fd, buf, len);
            return;
        }
    }

    exchange_info_t *ei = &g_exchange_tbl[seq];
    ei->is_use = true;
    ei->cli_sock = fd;
    ei->svr_sock = backend_fd;
    ei->old_seq = pp->seq;
    pp->seq = seq;

    DEBUG_LOG("SEND TO SER fd:%u len:%u", backend_fd, len);
    net_send_ser(backend_fd, (const char *)buf, len);    
}

void do_exchange_svr(int fd, const char *buf, int len)
{
    protocol_t *pp = (protocol_t *)buf;

    exchange_info_t *ei = &g_exchange_tbl[pp->seq];
    if (!ei->is_use) {
        ERROR_LOG("CLI CLOSED");
        return;
    }

    ei->is_use = false;
    DEBUG_LOG("SEND TO CLI fd:%u len:%u", ei->cli_sock, len);
    pp->seq = ei->old_seq;
    net_send_cli(ei->cli_sock, buf, len);
}

void check_close_cli(int fd)
{
    for (uint32_t i = 0; i < g_exchange_tbl.size(); ++i) {
        if (!g_exchange_tbl[i].is_use)
            continue;

        if (g_exchange_tbl[i].cli_sock == fd) {
            g_exchange_tbl[i].is_use = false;
            break;
        }
    }
}

void check_close_ser(int fd)
{
    backend_fd = -1;
}

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

void proc_pkg_cli(int fd, const char *buf, uint32_t len)
{
    DEBUG_LOG("HANDLE CLI fd:%d, len:%u", fd, len);
    do_exchange_cli(fd, buf, len);
}

void proc_pkg_ser(int fd, const char *buf, uint32_t len)
{
    DEBUG_LOG("HANDLE SER fd:%d, len:%u", fd, len);
    do_exchange_svr(fd, buf, len);
}

void link_up_cli(int fd, uint32_t ip)
{
    DEBUG_LOG("LINKUP CLI ip:%s fd:%d", inet_ntoa(*(in_addr *)&ip), fd);
}

void link_down_cli(int fd)
{
    DEBUG_LOG("LINKDOWN CLI fd:%d", fd);
    check_close_cli(fd);
}

void link_down_ser(int fd)
{
    DEBUG_LOG("LINKDOWN SER fd:%d", fd);
    check_close_ser(fd);
}

}
