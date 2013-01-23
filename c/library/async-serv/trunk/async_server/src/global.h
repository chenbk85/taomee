#ifndef ASYNC_SERVER_GLOBAL_H
#define ASYNC_SERVER_GLOBAL_H

#include <fcntl.h>
#include <stdint.h>

#include <vector>

enum {
    PROC_MAIN = 1,
    PROC_CONN,
    PROC_WORK
};

extern volatile int g_stop;
extern uint32_t g_max_pkg_len;
extern uint32_t g_max_connect;
extern char g_bind_ip[16];
extern uint32_t *g_link_flags;

struct work_conf_t {
    uint32_t id;
    char name[32];
    pid_t pid;
};

extern std::vector<work_conf_t> g_work_confs;

int load_work_file(const char *filename);

//< for work
extern uint32_t g_work_id;
extern char g_work_name[32];
extern uint32_t g_work_channel;
//>

static inline void set_noblock(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)|O_NONBLOCK);
}

extern char *arg_start;
extern char *arg_end;
extern char *env_start;

void set_title(const char* fmt, ...);



int create_udp_socket(struct sockaddr_in* addr, const char* ip, uint16_t port);
void init_warning_system();

extern "C" void send_warning_msg(const char *warning_content, uint32_t uid, uint32_t cmdid, uint32_t ishex, const char *ip);


// 根据设备名称，获取相应的ip
int get_ip_by_name(const char * eth_name, char * ip);

#endif
