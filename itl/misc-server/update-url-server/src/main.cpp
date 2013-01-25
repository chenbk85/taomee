/**
 * =====================================================================================
 *       @file  update_url_server.cpp
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  12/13/2010 05:10:08 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#include <signal.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <set>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <arpa/inet.h>

extern "C" {
#include <libtaomee/log.h>
}

#include "utils.h"
#include "i_config.h"

using namespace std;

typedef struct {
    int continue_working;
    int multicast_fd;
    int send_fd;
    int udp_port;
    char *update_url;
    set<string> *p_host_set;
} thread_arg_t;

/**
 * @struct 接受命令协议包
 */
typedef struct {
    uint16_t pkg_len;
    uint16_t cmd_id;    /**<命令ID*/
    char cmd[0];        /**<命令内容*/
} __attribute__((__packed__)) multicast_cmd_t;

const char g_version[] = "1.0.0";

const uint16_t g_request_id = 1001;
const uint16_t g_response_id = 1002;

// 配置文件列表
const static char g_config_file_list[][PATH_MAX] = {
    "../conf/url_server.ini"   
};

// 配置文件个数
const static int g_config_file_count = sizeof(g_config_file_list) / sizeof(*g_config_file_list);

const static string g_red_clr = "\e[1m\e[31m";
const static string g_grn_clr = "\e[1m\e[32m";
const static string g_end_clr = "\e[m";

volatile static sig_atomic_t g_got_sig_term = 0;
volatile static sig_atomic_t g_got_sig_int  = 0;
volatile static sig_atomic_t g_got_sig_quit = 0;

/**
 * @brief 信号处理函数
 * @param sig 触发的信号
 */
static void signal_handler(int sig)
{
    switch (sig) {
        case SIGTERM:
            g_got_sig_term = 1;
            DEBUG_LOG("got sig term");
            break;
        case SIGINT:
            g_got_sig_int = 1;
            break;
        case SIGQUIT:
            g_got_sig_quit = 1;
            break;
        default:
            ERROR_LOG("ERROR: it should never come here!");
            break;
    }
}

/**
 * @brief 创建并初始化配置实例
 * @param config_file_list 配置文件列表
 * @param config_file_count 配置文件个数
 * @param pp_config 配置接口
 * @return
 */
static int create_and_init_config_instance(const char (*config_file_list)[PATH_MAX],
                                           int config_file_count,
                                           i_config **pp_config
                                          )
{
    if (config_file_list == NULL || pp_config == NULL) {
        return -1;
    }

    //创建配置接口的实例
    if (create_config_instance(pp_config) != 0) {
        return -1;
    }

    // 初始化配置接口
    if ((*pp_config)->init(config_file_list, config_file_count) != 0) {
        return -1;
    }

    return 0;
}

/**
 * @brief 初始化日志模块
 * @param p_config 配置接口实例
 * @return 成功返回0，失败返回-1
 */
static int init_log(const i_config *p_config)
{
    if (p_config == NULL) {
        return -1;
    }

    istringstream iss;
    char buffer[1024] = {0};
    char log_dir[PATH_MAX] = {0};
    char log_prefix[NAME_MAX] = {0};
    int log_lvl = 0;
    uint32_t log_size = 0;
    int log_count = 0;

    if (p_config->get_config("log", "log_dir", log_dir, sizeof(log_dir)) != 0) {
        cerr << "ERROR: p_config->get_config(\"log\", \"log_dir\", log_dir, sizeof(log_dir))."
            << endl;
        return -1;
    }

    if (p_config->get_config("log", "log_prefix", log_prefix, sizeof(log_prefix)) != 0) {
        cerr << "ERROR: p_config->get_config(\"log\", \"log_prefix\", log_dir, sizeof(log_dir))."
            << endl;
        return -1;
    }

    memset(buffer, 0, sizeof(buffer));
    if (p_config->get_config("log", "log_count", buffer, sizeof(buffer)) != 0) {
        cerr << "ERROR: p_config->get_config(\"log\", \"log_count\", buffer, sizeof(buffer))."
            << endl;
        return -1;
    }

    iss.str(string(iss.str()).append(buffer).append(" "));
    memset(buffer, 0, sizeof(buffer));
    if (p_config->get_config("log", "log_lvl", buffer, sizeof(buffer)) != 0) {
        cerr << "ERROR: p_config->get_config(\"log\", \"log_lvl\", buffer, sizeof(buffer))."
            << endl;
        return -1;
    }
    iss.str(string(iss.str()).append(buffer).append(" "));
    memset(buffer, 0, sizeof(buffer));
    if (p_config->get_config("log", "log_size", buffer, sizeof(buffer)) != 0) {
        cerr << "ERROR: p_config->get_config(\"log\", \"log_size\", buffer, sizeof(buffer))."
            << endl;
        return -1;
    }
    iss.str(string(iss.str()).append(buffer));

    iss >> dec >> log_count;
    if (!iss) {
        cerr << "ERROR: log: log_count." << endl;
        return -1;
    }
    iss >> dec >> log_lvl;
    if (!iss) {
        cerr << "ERROR: log: log_lvl." << endl;
        return -1;
    }
    iss >> dec >> log_size;
    if (!iss) {
        cerr << "ERROR: log: log_size." << endl;
        return -1;
    }

    enable_multi_thread();
    if (log_init(log_dir, (log_lvl_t)log_lvl, log_size, log_count, log_prefix) != 0) {
        cerr << "log_init error." << endl;
        return -1;
    }
    enable_multi_thread();
    set_log_dest(log_dest_file);

    return 0;
}

int load_host_list(set<string> &host_set, char *conf_dir)
{
    host_set.clear();         

    DIR *dir = NULL;
    struct dirent *de = NULL;
    dir = opendir(conf_dir); 
    if(dir == NULL)
    {    
        ERROR_LOG("Dir %s open failed,Error:%s");
        return -1;
    }    

    while((de = readdir(dir)) != NULL)
    {    
        if(strcmp(de->d_name,".") == 0 || strcmp(de->d_name, "..") == 0)
        {    
            continue;
        }    

        host_set.insert(de->d_name);
    }    
    closedir(dir);

    return 0;
}

static void *deal_multicast(void *arg)
{
    thread_arg_t *p_thread_arg = (thread_arg_t *)arg;
    set<string> *p_host_set = p_thread_arg->p_host_set;
    char *update_url = p_thread_arg->update_url;
    int multicast_fd = p_thread_arg->multicast_fd;
    int udp_port = p_thread_arg->udp_port;
    int send_fd = p_thread_arg->send_fd;

    char data_buf[512] = {0};
    struct sockaddr_in peer_addr;
    socklen_t sock_len = sizeof(peer_addr);
    int rcvlen;
    while (p_thread_arg->continue_working) {
        rcvlen = recvfrom(multicast_fd, data_buf, sizeof(data_buf), 0, (struct sockaddr *)&peer_addr, &sock_len);
        if (rcvlen < 0)
        {
            ERROR_LOG("ERROR: recvfrom(...) failed: %s", strerror(errno));
            continue;
        }

        DEBUG_LOG("recv from multicast,len:%d", rcvlen);

        multicast_cmd_t *cmd = (multicast_cmd_t *)data_buf; 
        if (cmd->pkg_len != rcvlen) {
            ERROR_LOG("ERROR: packet len:%d is not equal to recv len:%d", cmd->pkg_len, rcvlen); 
            continue;
        }
        
        if (cmd->cmd_id == g_request_id) {  // 广播的是请求更新服务器的url地址

            char peer_ip[16];
            if (inet_ntop(AF_INET, &peer_addr.sin_addr, peer_ip, sizeof(peer_ip)) == NULL) {
                ERROR_LOG("ERROR: get peer ip."); 
                continue;
            }
            DEBUG_LOG("recv a update request, peer ip is %s.", peer_ip);

            if (p_host_set->find(peer_ip) == p_host_set->end()) {  // 对端ip不在更新服务处理的范围内
                DEBUG_LOG("host:%s is not expected to update from this server.", peer_ip); 
                continue;
            }

            memset(data_buf, 0, sizeof(data_buf));
            multicast_cmd_t *response = (multicast_cmd_t *)data_buf; 

            strcpy(response->cmd, update_url); 
            response->cmd_id = g_response_id;
            response->pkg_len = sizeof(multicast_cmd_t) + strlen(response->cmd);

            peer_addr.sin_port = htons(udp_port);
            DEBUG_LOG("response url %s to [%s:%d].", update_url, peer_ip, htons(peer_addr.sin_port));

            if (sendto(send_fd, response, response->pkg_len, 0, (struct sockaddr *)&peer_addr, sock_len) < 0)
            {   
                ERROR_LOG("send data to multicast[%s:%d] failed!", peer_ip, peer_addr.sin_port);
            }  
        }
    }

    DEBUG_LOG("child thread leave main loop.");
    return (void *)0;
}

int main(int argc, char **argv)
{
    cout << "update_url_query: version: " << g_version 
         << " build time: " << __DATE__" "__TIME__ << endl;

    // 处理信号
    mysignal(SIGTERM, signal_handler);
    mysignal(SIGINT, signal_handler);
    mysignal(SIGQUIT, signal_handler);

    // 作为后台程序运行，不改变根路径，不关闭标准终端
    if (daemon(1, 1) != 0) { 
        return -1;
    }    

    // 判断是否已经运行
    if (already_running() != 0) { 
        cerr << g_red_clr << setw(70) << left << "Already running." << "[failed]" << g_end_clr << endl;
        return -1;
    }    

    // 上调打开文件数的限制
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) == -1) {
        cerr << "ERROR: getrlimit." << endl;
        return -1;
    }    
    rl.rlim_cur = rl.rlim_max;
    if (setrlimit(RLIMIT_NOFILE, &rl) != 0 ) {
        cerr << "ERROR: setrlimit." << endl;
        return -1;
    }    

    // 允许产生CORE文件
    if (getrlimit(RLIMIT_CORE, &rl) != 0) {
        cerr << "ERROR: getrlimit." << endl;
        return -1;
    }
    rl.rlim_cur = rl.rlim_max;
    if (setrlimit(RLIMIT_CORE, &rl) != 0) {
        cerr << "ERROR: setrlimit." << endl;
        return -1;
    }    

    // 初始化设置proc标题
    init_proc_title(argc, argv);

    // 设置proc的标题，方便ps时查看进程
    set_proc_title("OA_UPDATE_URL_SERVER");

    // 定义创建并初始化配置接口
    i_config *p_config = NULL;
    if (create_and_init_config_instance(g_config_file_list, g_config_file_count, &p_config) != 0) { 
        cout << setw(70) << left << "ERROR: create_and_init_config_instance" 
             << g_red_clr << "[ failed ]" << g_end_clr << endl;
        if (p_config != NULL) {
            p_config->release();
            p_config = NULL;
        }    
        return -1;
    }    

    // 初始化日志模块
    if (init_log(p_config) != 0) { 
        cerr << "ERROR: init_log." << endl;
        return -1;
    }    

    DEBUG_LOG("start update url server: %s build time: %s", g_version, __DATE__" "__TIME__);

    char update_url[1024] = {0};
    char conf_dir[PATH_MAX] = {0};

    if (p_config->get_config("monitor-thread", "update_url", update_url, sizeof(update_url)) != 0) {
        ERROR_LOG("ERROR: get [monitor-thread]:[update_url] failed.");
        return -1;
    }

    if (p_config->get_config("monitor-thread", "conf_dir", conf_dir, sizeof(conf_dir)) != 0) {
        ERROR_LOG("ERROR: get [monitor-thread]:[conf_dir] failed.");
        return -1;
    }

    int conf_fd = inotify_init();
    if (conf_fd < 0) {
        cout << "inotify_init failed" << endl;
        return -1; 
    }

    int wd = inotify_add_watch(conf_fd, conf_dir, IN_CREATE | IN_DELETE);
    if (wd < 0) {
        cout << "inotify_add_watch failed" << endl;
        return -1; 
    }

    // 获得广播的ip和端口
    char multicast_ip[16] = {0};
    char port_str[10] = {0};
    if (p_config->get_config("network", "multicast_ip", multicast_ip, sizeof(multicast_ip)) != 0) {
        ERROR_LOG("ERROR: get [network]:[multicast_ip] failed.");
        return -1;
    }
    if (p_config->get_config("network", "multicast_port", port_str, sizeof(port_str)) != 0) {
        ERROR_LOG("ERROR: get [network]:[multicast_port] failed.");
        return -1;
    }
    int multicast_port = atoi(port_str);
    if (multicast_port < 0 || multicast_port > 65536) {
        ERROR_LOG("ERROR: multicast_port:%d.", multicast_port); 
        return -1;
    }

    // 添加这个更新服务器可以更新的所有机器的列表
    set<string> host_set;
    if (load_host_list(host_set, conf_dir) != 0) {
        ERROR_LOG("ERROR: load host list.");
        return -1;
    }

    // 创建广播的套接口
    struct sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(struct sockaddr_in));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(multicast_port);
    if (inet_pton(AF_INET, multicast_ip, &peer_addr.sin_addr.s_addr) <= 0)
    {   
        ERROR_LOG("Wrong multicast IP: %s", multicast_ip);
        return -1; 
    }   
    int multicast_sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (multicast_sockfd == -1) 
    {   
        ERROR_LOG("ERROR: socket(SOCK_DGRAM) failed: %s.", strerror(errno));
        return -1; 
    }   

    ///设置要加入组播的地址
    struct ip_mreq mreq;
    memcpy(&mreq.imr_multiaddr.s_addr, &peer_addr.sin_addr, sizeof(struct in_addr));
    ///设置发送组播消息的源主机的地址信息
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(multicast_sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(struct ip_mreq)) != 0)
    {   
        ERROR_LOG("setsockopt(IP_ADD_MEMBERSHIP) failed, %s:%d(IP:Port)", multicast_ip, multicast_port);
        return -1; 
    }   

    ///设置地址重用
    int reuseaddr_flag = 1;
    if (setsockopt(multicast_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_flag, sizeof(reuseaddr_flag)) != 0)
    {
        ERROR_LOG("ERRRO: setsockopt(SO_REUSEADDR) failed: %s.", strerror(errno));
        return -1;
    }

    if (bind(multicast_sockfd, (struct sockaddr *) &peer_addr, sizeof(peer_addr)) == -1)
    {
        ERROR_LOG("ERROR: bind failed: %s.", strerror(errno));
        return -1;
    }

    int send_sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (send_sockfd < 0) {
        ERROR_LOG("ERROR: create socket failed: %s.", strerror(errno));
        return -1; 
    }   

    // 用一个线程处理广播请求
    pthread_t thread_id;
    thread_arg_t thread_arg = {1, multicast_sockfd, send_sockfd, multicast_port, update_url, &host_set};
    if (pthread_create(&thread_id, NULL, deal_multicast, &thread_arg) != 0) {
        ERROR_LOG("ERROR: create thread %s.", strerror(errno)); 
        thread_id = 0;
        return -1;
    }

    cout << setw(70) << left << "OA_UPDATE_URL_SERVER START SUCCESSFULLY, VERSION: " << g_version
         << g_grn_clr << "[ ok ]" << g_end_clr << endl;


    // 重定向标准输入输出到/dev/null
    int null_fd = open("/dev/null", O_RDWR);
    if (-1 == null_fd) {
        ERROR_LOG("ERROR: open(\"/dev/null\", O_RDWR).");
        return -1; 
    }   
    dup2(null_fd, 0); 
    dup2(null_fd, 1); 
    dup2(null_fd, 2); 
    close(null_fd);

    char buffer[256];
    int len = 0;
    while (!g_got_sig_term) {
        if (g_got_sig_int) {
            g_got_sig_int = 0;
            DEBUG_LOG("got_sig_int.");
        }
        if (g_got_sig_quit) {
            g_got_sig_quit = 0;
            DEBUG_LOG("got_sig_quit.");
        }

        // 监听配置文件的变化
        if ((len = read(conf_fd, buffer, sizeof(buffer)))) {
            DEBUG_LOG("Directory:%s has change.", conf_dir);

            struct inotify_event *event = (struct inotify_event *)buffer; 
            if ((event->mask & IN_CREATE) || (event->mask & IN_DELETE)) {
                DEBUG_LOG("host is added to or delete from this server."); 

                if (load_host_list(host_set, conf_dir) != 0) {
                    ERROR_LOG("ERROR: load host list.");
                    return -1;
                }
            } else {
                DEBUG_LOG("other change.");
            }
        }
    }
    DEBUG_LOG("monitor thread leave main loop.");

    thread_arg.continue_working = 0;
    pthread_join(thread_id, NULL);

    return 0;
}
