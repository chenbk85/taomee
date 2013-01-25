#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>

#include "lib/log.h"
#include "lib/utils.h"
#include "db_operator.h"
#include "define.h"

using namespace std;

const char* g_red_clr = "\e[1m\e[31m";
const char* g_grn_clr = "\e[1m\e[32m";
const char* g_ylw_clr = "\e[1m\e[33m";
const char* g_end_clr = "\e[m";

bool g_got_sig_term = false;

static void sig_handler_term(int signo)
{
    g_got_sig_term = true;
}

static int daemon_start()
{
    struct rlimit rlim;
    rlim.rlim_cur = RLIM_INFINITY;
    rlim.rlim_max = RLIM_INFINITY;
    //raise open files
    setrlimit(RLIMIT_NOFILE, &rlim);
    //allow core dump
    setrlimit(RLIMIT_CORE, &rlim);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler_term;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL); 
    sigaction(SIGQUIT, &sa, NULL);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    daemon(1, 1);
    return 0;
}


void check_server_status(vector<int> segment_vec)
{
    DEBUG_LOG("\n\n");
    vector<int>::iterator it = segment_vec.begin();
    char server_name[64] = {0};
    bool is_running = false;
    //oa_head服务检查
    for (;it != segment_vec.end(); ++it) {
        sprintf(server_name, "oa_head_%d", *it);
        if (0 != check_server_running(server_name, &is_running)) {
            DEBUG_LOG("check_server_running(%s) failed.", server_name);
            continue;
        }

        if (!is_running) {
            DEBUG_LOG("%s has been stopped", server_name);
            char head_path[256] = {0};
#ifdef MONITOR_INSIDE
            sprintf(head_path, "%s%d", INSIDE_HEAD_PATH, *it);
#else
            sprintf(head_path, "%s%d", OUTSIDE_HEAD_PATH, *it);
#endif
            if (0 != run_start_script(head_path)) {
                ERROR_LOG("run_start_script(%s) failed[%s]", head_path, strerror(errno));
            }

        } else {
            DEBUG_LOG("%s is still running", server_name);
        }
    }

    //告警脚本监控
    sprintf(server_name, "alarm_start.php");
    if (0 == check_server_running(server_name, &is_running)) {
        if (is_running) {
            DEBUG_LOG("%s is still running.", server_name);
        } else {
            DEBUG_LOG("%s has been stopped.", server_name);
        }
        char alarm_path[MAX_STR_LEN] = {0};
#ifdef MONITOR_INSIDE
        sprintf(alarm_path, "%s", INSIDE_ALARM_PATH);
#else
        sprintf(alarm_path, "%s", OUTSIDE_ALARM_PATH);
#endif
        if (0 != run_start_script(alarm_path)) {
            ERROR_LOG("run_start_script(%s) failed", alarm_path);
        }
    } else {
        DEBUG_LOG("check_server_running(%s) failed.", server_name);
    }
}

static int get_parameter_info(int argc, char *argv[], db_conf_t *p_db_conf, char *p_exclude_segment)
{
    char ch = 0;
    while ((ch = getopt(argc, argv, "h:p:n:u:P:c:e:")) != -1) {
        switch(ch) {
        case 'h':
            if (strlen(optarg) < 7 || strlen(optarg) >= sizeof(p_db_conf->ip)) {
                ERROR_LOG("ERROR: wrong parameter host[%s]", optarg);
                fprintf(stderr, "%sParameter host[%s] wrong%70s%s\n", g_red_clr, optarg, "[Error]", g_end_clr);
                return -1;
            }
            strcpy(p_db_conf->ip, optarg);
            break;
        case 'P'://port
            if (atoi(optarg) <= 0) {
                ERROR_LOG("ERROR: parameter port[%s] wrong", optarg);
                fprintf(stderr, "%sParameter password[%s] wrong%70s%s\n", g_ylw_clr, optarg, "[Warning]", g_end_clr);
                return -1;
            }
            p_db_conf->port = atoi(optarg);
            break;
        case 'n':
            if (strlen(optarg) >= sizeof(p_db_conf->db_name)) {
                ERROR_LOG("ERROR: parameter db_name[%s] too long", optarg);
                fprintf(stderr, "%sParameter db_name[%s] too long%70s%s\n", g_ylw_clr, optarg, "[Warning]", g_end_clr);
                return -1;
            }
            strcpy(p_db_conf->db_name, optarg);
            break;
        case 'u':
            if (strlen(optarg) >= sizeof(p_db_conf->user)) {
                ERROR_LOG("ERROR: parameter user[%s] too long", optarg);
                fprintf(stderr, "%sParameter user[%s] too long%70s%s\n", g_ylw_clr, optarg, "[Warning]", g_end_clr);
                return -1;
            }
            strcpy(p_db_conf->user, optarg);
            break;
        case 'p'://password
            if (strlen(optarg) >= sizeof(p_db_conf->passwd)) {
                ERROR_LOG("ERROR: parameter password[%s] too long", optarg);
                fprintf(stderr, "%sParameter password[%s] too long%70s%s\n", g_ylw_clr, optarg, "[Warning]", g_end_clr);
                return -1;
            }
            strcpy(p_db_conf->passwd, optarg);
            break;
        case 'c'://charset
            if (strlen(optarg) >= sizeof(p_db_conf->charset)) {
                ERROR_LOG("ERROR: parameter charset[%s] too long", optarg);
                fprintf(stderr, "%sParameter charset[%s] too long%70s%s\n", g_ylw_clr, optarg, "[Warning]", g_end_clr);
                return -1;
            }
            strcpy(p_db_conf->charset, optarg);
            break;
        case 'e'://exclude segment
            sprintf(p_exclude_segment, ",%s,", optarg);
            break;
        }
    }
    DEBUG_LOG("Database configuration:");
    DEBUG_LOG("IP[%s], Port[%d]", p_db_conf->ip, p_db_conf->port);
    DEBUG_LOG("db_name[%s], user[%s], passwd[%s]", p_db_conf->db_name, p_db_conf->user, p_db_conf->passwd);
    DEBUG_LOG("charset[%s]", p_db_conf->charset);
    DEBUG_LOG("exclude_segment_id[%s]\n", p_exclude_segment);

    return 0;
}

 /** 
 * @brief  初始化日志
 * @param  无
 * @return  -1:failed  0:success
 */
static int log_initial()
{
    log_conf_t log_conf = {10,  7, 10240000, "oa_head_monitor_", "../log"};
    ///如果目录不存在则试图创建
    if(access(log_conf.log_dir, F_OK) != 0 && 
            mkdir(log_conf.log_dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
        fprintf(stderr, "Log dir[%s] does not exists, but create it failed,sys error:%s.\n",
                log_conf.log_dir, strerror(errno));
        return -1;
    } else {
        chmod(log_conf.log_dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    }

    if(0 != log_init(log_conf.log_dir, (log_lvl_t)log_conf.log_lvl,
            log_conf.log_size, log_conf.log_count, log_conf.log_prefix)) {
        fprintf(stderr, "log_init error.");
        return -1;
    }

    set_log_dest(log_dest_file);
    return 0;
}

int main(int argc, char *argv[])
{
    //初始化日志
    if(0 != log_initial()) {
        fprintf(stderr, "%sInit log%70s%s\n", g_red_clr, "[failed]", g_end_clr);
        return -1;
    }

    db_conf_t db_conf = {"192.168.0.43", 3306, "db_itl", "oaadmin", "oaadmin@tm", "utf8"};
    char exclude_segment[MAX_STR_LEN] = {0};
    if (0 != get_parameter_info(argc, argv, &db_conf, exclude_segment)) {
        return -1;
    }

    c_head_db_operator db_opt;
    if (0 != db_opt.init((const db_conf_t *)&db_conf)) {
        ERROR_LOG("ERROR: c_head_db_operator init failed");
        fprintf(stderr, "%sc_head_db_operator init%70s%s\n", g_red_clr, "[Failed]", g_end_clr);
        return -1;
    }

    fprintf(stdout, "%soa_alarm_monitor: version: %s build time: %s %s%s\n",
            g_grn_clr, VERSION, __DATE__, __TIME__, g_end_clr);
    daemon_start();

    //重定向标准输入输出到/dev/null
    int null_fd = open("/dev/null", O_RDWR);
    if(null_fd == -1) {
        return -1;
    }
    dup2(null_fd, 0);
    dup2(null_fd, 1);
    dup2(null_fd, 2);
    close(null_fd);

    init_proc_title(argc, argv);
    set_proc_title("oa_alarm_monitor");

    while (!g_got_sig_term) {
        DEBUG_LOG("Segment id list:");
        vector<int> segment_vec;
        int segment_id = db_opt.get_first_segment_id();
        char tmp_segment[32] = {0};
        while (segment_id > 0) {
            sprintf(tmp_segment, ",%d,", segment_id);
            if (NULL == strstr(exclude_segment, tmp_segment)) {
                DEBUG_LOG("oa_head_%d in monitoring", segment_id);
                segment_vec.push_back(segment_id);
            } else {
                DEBUG_LOG("oa_head_%d out of monitoring", segment_id);
            }
            segment_id = db_opt.get_next_segment_id();
        }
        DEBUG_LOG("\n");

        int flush_times = 0;
        while (flush_times < MYSQL_FLUSH_TIMES) {
            ++flush_times;
            check_server_status(segment_vec);
            sleep(SERVER_CHECK_SPAN);
        }
    }

    uninit_proc_title();
    return 0;
}

