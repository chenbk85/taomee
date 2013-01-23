#include <time.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include <async_server.h>
#include <log.h>

struct cp_message_t {
    uint16_t len;
    uint16_t channel_id;
    uint32_t file_id;
    uint32_t seq_no;
    uint32_t type;
    uint32_t timestamp;
    char data[];
}__attribute__((__packed__));

struct pc_message_t {
    uint32_t type;
    uint32_t file_id;
    uint32_t seq_no;
    uint16_t channel_id;
}__attribute__((__packed__));

extern "C" {

int plugin_init(int type)
{
    DEBUG_LOG("INIT...");
    srand(get_work_id() + time(NULL));
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
    if (len < sizeof(uint16_t))
        return 0;

    return *(uint16_t *)buf;
}

int get_pkg_len_ser(const char *buf, uint32_t len)
{
//    INFO_LOG("get_pkg_len_ser %u", len);
    return len;
}

void link_up_cli(int fd, uint32_t ip)
{
    DEBUG_LOG("LINKUP CLI ip:%s fd:%d", inet_ntoa(*((struct in_addr *)&ip)), fd);
}

void link_down_cli(int fd)
{
    DEBUG_LOG("LINKDOWN CLI fd:%d", fd);
}

void proc_pkg_cli(int fd, const char *buf, uint32_t len)
{
//    usleep(rand() % 500000); 

    DEBUG_LOG("HANDLE CLI fd:%d, len:%u", fd, len);
    cp_message_t *cp_message = (cp_message_t *)buf;
    if (cp_message->len != len) {
        ERROR_LOG("wrong length fd:%d, len:%u", fd, len);
        return;
    }

    pc_message_t pc_message;
    pc_message.type = cp_message->type;
    pc_message.file_id = cp_message->file_id;
    pc_message.seq_no = cp_message->seq_no;
    pc_message.channel_id = cp_message->channel_id;

    net_send_cli(fd, (char *)&pc_message, sizeof(pc_message));
}

void link_down_ser(int fd)
{
//    DEBUG_LOG("LINKDOWN SER fd:%d", fd);
}

void proc_pkg_ser(int fd, const char *buf, uint32_t len)
{
//    DEBUG_LOG("HANDLE SER fd:%d, len:%u", fd, len);
    net_send_cli(fd, buf, len);
}

}

