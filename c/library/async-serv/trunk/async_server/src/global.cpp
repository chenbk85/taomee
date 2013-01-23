#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "global.h"
#include "log.h"
#include "shmq.h"
#include "config.h"


volatile int g_stop = 0;

uint32_t g_max_pkg_len = 0;
uint32_t g_max_connect = 0;
uint32_t *g_link_flags = NULL;
char g_bind_ip[16] = {0};

std::vector<work_conf_t> g_work_confs;

int load_work_file(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
        BOOT_LOG(-1, "load work file:%s", filename);

    int len = lseek(fd, 0L, SEEK_END);
    lseek(fd, 0L, SEEK_SET);

    char *data = (char *)malloc(len + 1);
    if (read(fd, data, len) == -1)
        BOOT_LOG(-1, "load work file:%s", filename);

    data[len] = '\0';
    char *start = data;
    char *end;
    while (data + len > start) {
        end = strchr(start, '\n');
        if (end)
            *end = '\0';

        if ((*start != '#')) {
            char *id;
            char *name;

            id = strtok(start, " \t");
            name = strtok(NULL, " \t");

            if (id != NULL && name != NULL) {
                if (strlen(name) >= 32)
                    return -1;

                work_conf_t wc;
                wc.id = atoi(id);
                strcpy(wc.name, name);
                g_work_confs.push_back(wc);
            }
        }

        if (end) {
            start = end + 1;
        } else {
            break;
        }
    }

    free(data);
    close(fd);

    if (g_work_confs.size() == 0)
        BOOT_LOG(-1, "load work file:%s", filename);

    BOOT_LOG(0, "load work file:%s", filename);
}

uint32_t g_work_id;
char g_work_name[32];
uint32_t g_work_channel;

extern "C" uint32_t get_work_id()
{
    return g_work_id;
}

extern "C" const char *get_work_name()
{
    return g_work_name;
}

extern "C" uint32_t get_work_num()
{
    return g_work_confs.size();
}

extern "C" uint32_t get_work_idx()
{
    return g_work_channel;
}

uint32_t g_last_idle_idx = 0;

extern "C" uint32_t get_idle_work_idx_conn()
{
    uint32_t tmp_idx = g_last_idle_idx;
    for (uint32_t i = 0; i < g_work_confs.size(); ++i)
    {
        tmp_idx++;
        tmp_idx %= g_work_confs.size();
        if (g_shm_queue_mgr.recv_queue[tmp_idx].head->idle != 0)
        {
            g_last_idle_idx = tmp_idx;
            return g_last_idle_idx;
        }
    }

    g_last_idle_idx++;
    g_last_idle_idx %= g_work_confs.size();
    return g_last_idle_idx;
}


extern "C" const char * get_bind_ip()
{
    return g_bind_ip;
}

extern "C" void set_prog_name(const char *prog_name)
{
    set_title(prog_name);
}

char *arg_start;
char *arg_end;
char *env_start;

extern void set_title(const char* fmt, ...)
{
    char title[64];
    int i, tlen;
    va_list ap;

    va_start(ap, fmt);
    vsnprintf (title, sizeof (title) - 1, fmt, ap);
    va_end (ap);

    tlen = strlen (title) + 1;
    if (arg_end - arg_start < tlen && env_start == arg_end) {
        char *env_end = env_start;
        for (i=0; environ[i]; i++) {
            if(env_end == environ[i]) {
                env_end = environ[i] + strlen (environ[i]) + 1;
                environ[i] = strdup(environ[i]);
            } else {
                break;
            }
        }
        arg_end = env_end;
        env_start = NULL;
    }

    i = arg_end - arg_start;
    if (tlen == i) {
        strcpy (arg_start, title);
    } else if (tlen < i) {
        strcpy (arg_start, title);
        memset (arg_start + tlen, 0, i - tlen);
    } else {
        stpncpy(arg_start, title, i - 1)[0] = '\0';
    }
}

#pragma pack(push)
#pragma pack(1)
typedef struct
{
    uint32_t len;
    uint32_t seq;
    uint16_t cmd;
    uint32_t ret;
    uint32_t id;
    char svr[120];
    uint32_t uid;
    uint32_t cmdid;
    uint32_t hex;
    char ip[16];
    uint32_t burst_limit;
    uint32_t warning_interval;
    char phone_numbers[256];
}warning_pkg_t;
#pragma pack(pop)

static int warning_fd = -1;
static struct sockaddr_in warning_sockaddr;
static const char *prj_name;
static const char *phone_nums;

int create_udp_socket(struct sockaddr_in* addr, const char* ip, uint16_t port)
{
    memset(addr, 0, sizeof(*addr));

    addr->sin_family = AF_INET;
    addr->sin_port   = htons(port);
    if ( inet_pton(AF_INET, ip, &(addr->sin_addr)) <= 0 )
    {
        return -1;
    }

    return socket(PF_INET, SOCK_DGRAM, 0);
}

void init_warning_system()
{
    if(config_get_strval("warning_server_ip", "0.0.0.0") == 0)
    {
        return;
    }

    warning_fd = create_udp_socket(&warning_sockaddr, config_get_strval("warning_server_ip", "0.0.0.0"), config_get_intval("warning_port", 0));
    if(warning_fd == -1)
    {
        WARN_LOG("failed to init warning sys: ip=%s, port=%d", config_get_strval("warning_server_ip", "0.0.0.0"), config_get_intval("warning_port", 0));
    }

    prj_name = config_get_strval("project_name", 0);
    if(prj_name == 0)
    {
        prj_name = "unspecified";
    }

    phone_nums = config_get_strval("phone_number", 0);
    if(phone_nums == 0)
    {
        phone_nums = "13774451574";

    }

}

extern "C" void send_warning_msg(const char *warning_content, uint32_t uid, uint32_t cmdid, uint32_t ishex, const char *ip)
{
    if(warning_fd  == -1)
    {
        return;
    }

    warning_pkg_t pkg;

    if (ip == 0) {
            ip = "";
    }

    pkg.len              = sizeof(pkg);
    pkg.seq              = 0;
    pkg.cmd              = 0xF101;
    pkg.ret              = 0;
    pkg.id               = uid;

    snprintf(pkg.svr, sizeof(pkg.svr), "%s.%s", prj_name, warning_content);
    pkg.uid              = uid;
    pkg.cmdid            = cmdid;
    pkg.hex              = ishex;
    strncpy(pkg.ip, ip, sizeof(pkg.ip));
    pkg.burst_limit      = 1;
    pkg.warning_interval = 10 * 60;
    strncpy(pkg.phone_numbers, phone_nums, sizeof(pkg.phone_numbers));

    sendto(warning_fd, &pkg, sizeof(pkg), 0, (struct sockaddr*)&warning_sockaddr, sizeof(warning_sockaddr));

    DEBUG_LOG("SEND WARNING MSG\t[content=%s ip=%s]", warning_content, ip);

}



int get_ip_by_name(const char * eth_name, char * ip)
{

    if (NULL == eth_name || NULL == ip) 
    {
        ERROR_LOG("ERROR: parameter cannot be NULL.");
        return -1;
    }

    if (0 == strlen(eth_name))
    {
        ERROR_LOG("ERROR: eth_name length cannot be zero.");
        return -1;
    }


    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sockfd) 
    {
        ERROR_LOG("ERROR: socket(AF_INET, SOCK_DGRAM, 0) failed: %s", strerror(errno));
        return -1;
    }

    struct ifreq ifr;
    strncpy(ifr.ifr_name, eth_name, sizeof(ifr.ifr_name));
    ifr.ifr_name[sizeof(ifr.ifr_name) -1] = 0;

    if (ioctl(sockfd, SIOCGIFADDR, &ifr) < 0) 
    {
        close(sockfd);
        return -1;
    }

    strcpy(ip, inet_ntoa(((struct sockaddr_in*)(&ifr.ifr_addr))->sin_addr));

    if (sockfd >= 0) 
    {
        close(sockfd);
    }

    return 0;
}
