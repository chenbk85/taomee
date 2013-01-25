/**
 * =====================================================================================
 *       @file  main.cpp
 *      @brief   
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  09/03/2010 06:31:41 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  mason , mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <iostream>
#include <limits.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include "lib/log.h"
#include "lib/c_mysql_iface.h"
#include "defines.h"
#include "db_operator.h"
#include "collect-process-thread/collect_process_thread.h"
#include "flush-cleanup-thread/flush_cleanup_thread.h"
#include "lib/utils.h"
#include "lib/oa_popen.h"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/utilities.h>
#include <net-snmp/net-snmp-includes.h>


#define OPEN_SUID_CORE_DUMP   system("/sbin/sysctl -e -q -w fs.suid_dumpable=1")
#define CLOSE_SUID_CORE_DUMP  system("/sbin/sysctl -e -q -w fs.suid_dumpable=0")

using namespace std;
const char* g_red_clr = "\e[1m\e[31m";
const char* g_grn_clr = "\e[1m\e[32m";
const char* g_ylw_clr = "\e[1m\e[33m";
const char* g_end_clr = "\e[m";

volatile static sig_atomic_t g_got_sig_term = 0;

static int become_a_nobody(const char *username)
{
    unsigned int   rval;
    struct passwd *pw = NULL;

    pw = getpwnam(username);
    if(pw == NULL)
    {
        fprintf(stderr, "user: '%s' does not exist.\n", username);
        return -1;
    }

    rval = getuid();
    if(rval != pw->pw_uid)
    {
        if(rval != 0)
        {
            fprintf(stderr, "must be root to setuid to '%s'.\n", username);
            return  -1; 
        }
        //设置effective gid如果调用者是root那么也将设置real gid
        setgid(pw->pw_gid);
        //设置effective uid如果调用者是root那么也将设置real uid
        rval = setuid(pw->pw_uid);
        if(rval < 0)
        {
            fprintf(stderr, "exiting. setuid '%s' error.\n", username);
            return -1; 
        }
    }
    return  0;
}
static void rlimit_reset()
{
    struct rlimit rlim;
    rlim.rlim_cur = RLIM_INFINITY;
    rlim.rlim_max = RLIM_INFINITY;
    //raise open files
    setrlimit(RLIMIT_NOFILE, &rlim);
    //allow core dump
    setrlimit(RLIMIT_CORE, &rlim);
}

static void sig_handler_term(int signo)
{
    g_got_sig_term = 1;
}

static int daemon_start()
{
    struct sigaction sa;
    rlimit_reset();
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

static int check_log_config(const config_var_t* p_config)
{
    if(strlen(p_config->log_dir) <= 0) {
        fprintf(stderr, "No log dir config.\n");
        return -1;
    } else if(access(p_config->log_dir, F_OK) != 0 && 
              mkdir(p_config->log_dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
        //如果目录不存在则试图创建
        fprintf(stderr, "Log dir does not exists and create it failed,sys error:%s.\n", strerror(errno));
        return -1;
    } else {
        chmod(p_config->log_dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    }

    //把log目录交给nobody
    struct passwd *pw = NULL;
    pw = getpwnam("nobody");
    if(NULL != pw) {
        chown(p_config->log_dir, pw->pw_uid, pw->pw_gid);
    }

    return 0;
}

 /** 
 * @brief  初始化日志
 * @param  p_config 配置项
 * @return  -1failed  0success
 */
static int log_initial(const config_var_t *p_config)
{
    if(strlen(p_config->log_prefix) <= 0 || p_config->log_lvl <= 0 || 
            p_config->log_size <= 0 || p_config->log_count <= 0)
    {
        fprintf(stderr, "log config variables error.");
        return -1;
    }

    if(log_init(p_config->log_dir, (log_lvl_t)p_config->log_lvl,
                p_config->log_size, p_config->log_count, p_config->log_prefix) != 0) 
    {
        fprintf(stderr, "log_init error.");
        return -1;
    }

    enable_multi_thread();
    set_log_dest(log_dest_file);
    return 0;
}

static int check_rrd_config(const config_var_t* p_config)
{
    //因为在得到rrd路径之前已经setuid为nobody了，所以只能检查一下，不能做任何操作(创建，修改权限等)
    if(strlen(p_config->rrd_rootdir) <= 0 || access(p_config->rrd_rootdir, F_OK | R_OK | W_OK | X_OK) != 0)
    {
        ERROR_LOG("Can not access rrd_rootdir:%s,sys error:%s", p_config->rrd_rootdir, strerror(errno));
        return -1;
    }
    return 0;
}

static bool get_update_status(c_mysql_iface* db_conn, unsigned char *action)
{
    return db_get_update_status(db_conn, action);
}

static int set_update_status(c_mysql_iface* db_conn)
{
    return db_set_update_status(db_conn);
}

static int get_switch_info(switch_group_vec_t *switch_group_set, unsigned int switch_group_num,
 c_mysql_iface* db_conn)
{
    switch_group_vec_t tmp_switch_group_set;

    int ret = db_get_switch_info(&tmp_switch_group_set, switch_group_num, db_conn);
    if(ret == 0)
    {
        //清空原来保存switch group的vector
        switch_group_set->clear();
        *switch_group_set = tmp_switch_group_set;
    }

    tmp_switch_group_set.clear();
    return ret;
}

static int get_interface_alarm_info(if_alarm_map_t *if_alarm_map, c_mysql_iface* db_conn)
{
    if_alarm_map_t tmp_if_alarm_map;

    int ret = db_get_interface_alarm_info(&tmp_if_alarm_map, db_conn);
    if(ret == 0)
    {
        if_alarm_map->clear();
        *if_alarm_map = tmp_if_alarm_map;
    }

    tmp_if_alarm_map.clear();
    return ret;
}

static int get_metric_info(metric_info_vec_t *metric_set, config_var_t *p_config, c_mysql_iface* db_conn)
{
    vector<metric_info_t> tmp_metric_set;

    int ret = db_get_metric_info(&tmp_metric_set, db_conn);
    if(ret == 0)
    {
        //清空原来保存metric的vector
        metric_set->clear();
        *metric_set = tmp_metric_set;
    }
    tmp_metric_set.clear();
    return ret;
}

static int get_default_metric_alarm_info(metric_alarm_vec_t *metric_set, c_mysql_iface* db_conn)
{
    vector<metric_alarm_t> tmp_metric_set;

    int ret = db_get_default_metric_alarm_info(&tmp_metric_set, db_conn);
    if(ret == 0)
    {
        //清空原来保存metric alarm的vector
        metric_set->clear();
        *metric_set = tmp_metric_set;
    }
    tmp_metric_set.clear();
    return ret;
}

static int get_specified_metric_alarm_info(metric_alarm_vec_t *metric_set, c_mysql_iface* db_conn)
{
    vector<metric_alarm_t> tmp_metric_set;

    int ret = db_get_default_metric_alarm_info(&tmp_metric_set, db_conn);
    if(ret == 0)
    {
        //清空原来保存metric alarm的vector
        metric_set->clear();
        *metric_set = tmp_metric_set;
    }
    tmp_metric_set.clear();
    return ret;
}

static int get_db_conf_info(switch_group_vec_t *switch_group_set, if_alarm_map_t *if_alarm_map, metric_info_vec_t *metric_set, metric_alarm_vec_t *default_alarm_conf, metric_alarm_vec_t *special_alarm_conf, config_var_t *config_ins, c_mysql_iface *db_conn, unsigned char action)
{
    int ret = 0;
    if(action == 0) {
        return ret;
    }
    if(action & SWITCH_CONFIG_UPDATE) {
        if(get_switch_info(switch_group_set, config_ins->collect_thread_num, db_conn) != 0) {
            ret = -1;
        }
    }
    if(action & INTERFACE_ALARM_STRATEGY_CONFIG_UPDATE) {
        if(get_interface_alarm_info(if_alarm_map, db_conn) != 0) {
            ret = -1;
        }
    }
    if(action & METRIC_CONFIG_UPDATE) {
        if(get_metric_info(metric_set, config_ins, db_conn) != 0) {
            ret = -1;
        }
        if(get_default_metric_alarm_info(default_alarm_conf, db_conn) != 0) {
            ret = -1;
        }
    }

    if(action & METRIC_ALARM_STRATEGY_CONFIG_UPDATE) {
        if(get_specified_metric_alarm_info(special_alarm_conf, db_conn) != 0) {
            ret = -1;
        }
    }

    return ret;
}

int main(int argc, char **argv)
{
    //将所有的环境变量设为本地环境变量
    setlocale(LC_ALL, "");
    //配置项结构 赋初值
    config_var_t  config_ins =
    {
        //[log]
        //log_count,log_dir,log_lvl,log_prefix,log_size
        10, "../log", 7, "switch_monitor_", 10240000,
        //[monitor]
        //flush_interval,alarm_interval,collect_interval,queue_len,collect_thread_num,alarm_server_url,rrdrootdir
#ifdef RELEASE_VERSION
        60, 15, 60, 1048576, 16, "http://192.168.6.51/oa-auto-update/index.php", "/opt/taomee/oaadmin/rrds"
#else
        60, 15, 60, 1048576, 16, "http://10.1.1.44/oa-auto-update/index.php", "/opt/taomee/oa/rrds"
#endif
    };
    //配置数据库连接
    c_mysql_iface *db_conn = NULL;
    //数据源
    //配置库的配置项
    char    db_host[16] = {0};
    char    db_name[256] = "db_itl";
    unsigned short db_port = 3306;
    char    db_user[256] = {0};
    char    db_pass[256] = {0};
    // /dev/null fd
    int     null_fd = -1; 
    int     ch = 0;
    unsigned char config_update_action = 15;//=0x0f

    //处理命令行参数
    while(true)
    {   
        ch = getopt(argc, argv, "a:r:d:h:p:u:P:c:");
        if(ch == -1) {
            break;
        }

        unsigned short  tmp_port = 3306;
        switch(ch) {   
        case 'a': //alarm server url
            if(strlen(optarg) >= sizeof(config_ins.alarm_server_url)) {   
                fprintf(stderr, "Wrong alarm_server_url.\n");
                return -1; 
            }   
            strncpy(config_ins.alarm_server_url, optarg, sizeof(config_ins.alarm_server_url));
            break;
        case 'r':  //rrd root dir
            if(strlen(optarg) >= sizeof(config_ins.rrd_rootdir)) {   
                fprintf(stderr, "The rrd_rootdir is too long,the max length of name is:%u.\n", PATH_MAX);
                return -1; 
            }   
            strncpy(config_ins.rrd_rootdir, optarg, sizeof(config_ins.rrd_rootdir));
            break;
        case 'd':  //db name 
            strncpy(db_name, optarg, sizeof(db_name) - 1);
            break;
        case 'h':  //db host
            strncpy(db_host, optarg, sizeof(db_host) - 1);
            break;
        case 'P':  //db port
            tmp_port = atoi(optarg);
            db_port = tmp_port <= 0 ? db_port : tmp_port;
            break;
        case 'c':  //collect thread number
            if(optarg && is_integer(optarg))
                config_ins.collect_thread_num = atoi(optarg);
            break;
        case 'u':  //db user 
            strncpy(db_user, optarg, sizeof(db_user) - 1);
            break;
        case 'p':  //db passwd
            strncpy(db_pass, optarg, sizeof(db_pass) - 1);
            break;
        default: break;
        }
    }

    if(strlen(db_user) == 0 || strlen(db_pass) == 0) {
        fprintf(stderr, "Critical:No %s%s argument.\n", strlen(db_user) == 0 ? "db_user " : "",
                strlen(db_pass) == 0 ? "db_passwd" : "");
        fprintf(stderr, "Usage:switch_monitor -u<db_user> -p<db_passwd> [-a<alarm_server_url>] [-r<rrd_rootdir>] [-d<db_name>]\n [-h<db_host>] [-P<db_port>] [-c<collect_thread_num>]\n");
        return -1;
    }

    //数据库信息结构
    db_info_t  db_ins;
    memset(&db_ins, 0, sizeof(db_ins));
    strcpy(db_ins.db_host, db_host);
    db_ins.db_port = db_port;
    strcpy(db_ins.db_name, db_name);
    strcpy(db_ins.db_user, db_user);
    strcpy(db_ins.db_pass, db_pass);

    //保存所有交换机信息的hash表
    hash_t *switch_table = NULL;    
    //保存所有交换机组的vector
    switch_group_vec_t switch_group_set;
    //保存接口的告警阀值信息的map
    if_alarm_map_t if_alarm_map;
    //保存交换机收集的所有metric的信息
    metric_info_vec_t metric_set;
    //保存缺省metric报警信息
    metric_alarm_vec_t default_alarm_conf;
    //保存特殊metric报警信息
    metric_alarm_vec_t special_alarm_conf;

    //数据收集和处理线程
    c_collect_process_thread collect_process_thread;
    //写数据&清理线程
    c_flush_cleanup_thread flush_cleanup_thread;
    //rrd操作对象
    rrd_handler rrd_operator;

    fprintf(stdout, "SWITCH-MONITOR:version: %s build time: %s %s\n", VERSION, __DATE__, __TIME__);

    daemon_start();

    //把bin目录交给nobody,这个工作在安装的时候就已经做过，但是为了可靠性在程序里再做一次
    struct passwd *pw = NULL;
    pw = getpwnam("nobody");
    if(pw) {
        chown("../", pw->pw_uid, pw->pw_gid);
        chown("../bin", pw->pw_uid, pw->pw_gid);
        chown("../bin/switch_monitor", pw->pw_uid, pw->pw_gid);
    }

    //设置setuid的core dump标志位
    OPEN_SUID_CORE_DUMP;

    //setuid to nobody
    if(0 != become_a_nobody("nobody")) {
        fprintf(stderr, "Setuid error,Exit.\n");
        CLOSE_SUID_CORE_DUMP;
        return -1;
    }
    fprintf(stdout, "Going to run as \"nobody\".\n");

    // 判断是否已经运行
    if(0 != already_running()) {
        fprintf(stderr, "%sAlready running.%70s%s\n", g_red_clr, "[failed]", g_end_clr);
        return -1; 
    }   

    //检查log配置项
    if(check_log_config(&config_ins) < 0) {
        fprintf(stderr, "%sNo log directory.%70s%s\n", g_red_clr, "[failed]", g_end_clr);
        return -1;
    }

    //初始化日志
    if(log_initial(&config_ins) < 0) {
        fprintf(stderr, "Log initial failed,Exit.\n");
        return -1;
    }

    //初始化oa_popen模块
    if(oa_popen_init() < 0) {   
        fprintf(stderr, "Init oa_popen module failed,Exit.\n");
        return -1; 
    }   

    //初始snmp的库和mib配置
    init_snmp("snmplib");

    if(create_mysql_iface_instance(&db_conn) != 0) {
        ERROR_LOG("Create mysql connect instance failed.");
        return -1;
    }

    if(db_conn->init(db_host, db_port, db_name, db_user, db_pass, "utf8") != 0) {
        ERROR_LOG("Connect to db:[%s] of host:[%s] failed.", db_name, db_host);
        db_conn->release();
        return -1;
    }

    //初始化设置proc标题
    init_proc_title(argc, argv);
    //设置proc的标题，方便ps时查看进程
    set_proc_title("oa_switch_monitor");

    //重定向标准输入输出到/dev/null
    null_fd = open("/dev/null", O_RDWR);
    if(-1 == null_fd) {
        db_conn->uninit();
        db_conn->release();
        return -1;
    }
    dup2(null_fd, 0);
    dup2(null_fd, 1);
    dup2(null_fd, 2);
    close(null_fd);

    //检查rrd路径的有效性
    if(check_rrd_config(&config_ins) != 0) {
        goto starterror;
    }

    //从数据库获取所有配置信息
    if(get_db_conf_info(&switch_group_set, &if_alarm_map, &metric_set, &default_alarm_conf,
                &special_alarm_conf, &config_ins, db_conn, config_update_action) != 0) {
        goto starterror;
    }

    //获取完配置信息后将更新标志位置为false
    if(set_update_status(db_conn) != 0) {
        DEBUG_LOG("set_update_status() failed.");
    }

    switch_table = hash_create(DEFAULT_SWITCHSIZE);
    if(NULL == switch_table) {
        ERROR_LOG("Create hash table for switches failed.");
        goto starterror;
    }
    hash_set_flags(switch_table, HASH_FLAG_IGNORE_CASE);

    if(rrd_operator.init(&config_ins) != 0)
    {
        ERROR_LOG("Init the rrd_handler failed.");
        goto starterror;
    }

    if(flush_cleanup_thread.init(&config_ins, switch_table, &db_ins, &rrd_operator) != 0) 
    {
        ERROR_LOG("flush&cleanup thread init failed.");
        goto starterror;
    }
    DEBUG_LOG("flush&cleanup thread inited.");

    do
    {
        DEBUG_LOG("==============================Config Info==============================");
        DEBUG_LOG("flush_interval: %u", config_ins.flush_interval);
        DEBUG_LOG("alarm_interval: %u", config_ins.alarm_interval);
        DEBUG_LOG("collect_interval: %u", config_ins.collect_interval);
        DEBUG_LOG("collect_thread_num: %u", config_ins.collect_thread_num);
        DEBUG_LOG("alarm_server_url: %s", config_ins.alarm_server_url);
        DEBUG_LOG("rrd_dir: %s", config_ins.rrd_rootdir);
        DEBUG_LOG("=======================================================================");
        if(collect_process_thread.init(&config_ins, &switch_group_set, 
                    &if_alarm_map, &metric_set, &default_alarm_conf,
                    &special_alarm_conf, switch_table) != 0) {
            ERROR_LOG("collect&process thread init failed.");
            goto starterror;
        }
        DEBUG_LOG("collect&process thread inited.");

do_sleep:
        unsigned sec = 0;
        while(!g_got_sig_term && sec++ < REINIT_INTERVAL) {
            sleep(1);//接收信号中断
        }
        //SIGTERM信号
        if(g_got_sig_term) {
            DEBUG_LOG("Got SIGTERM.Will exiting....");
            break;
        }

        //如果更新标志位为false,继续睡眠,否则重新解析配置重新初始化
        if(!get_update_status(db_conn, &config_update_action)) {
            DEBUG_LOG("NO UPDATE go on sleep");
            goto do_sleep;
        }
        DEBUG_LOG("GOT UPDATE");

        collect_process_thread.uninit();

        if(get_db_conf_info(&switch_group_set, &if_alarm_map, &metric_set, &default_alarm_conf,
                    &special_alarm_conf, &config_ins, db_conn, config_update_action) != 0) {
            DEBUG_LOG("Get db config failed,use the previous db configs.");
        }

        //获取完配置信息后将更新标志位置为false
        if(set_update_status(db_conn) != 0) {
            DEBUG_LOG("Set the update status of switch-monitor failed.");
        }

        //if(rrd_operator.init(&config_ins) != 0)
        //{
        //    ERROR_LOG("Init the rrd_handler failed.");
        //    goto starterror;
        //}
    } while(!g_got_sig_term);

    //出错跳出或者正常停止
starterror:
    flush_cleanup_thread.uninit();
    collect_process_thread.uninit();

    if(NULL != switch_table) {
        hash_destroy(switch_table);
    }

    switch_group_set.clear();
    if_alarm_map.clear();
    metric_set.clear();
    default_alarm_conf.clear();
    special_alarm_conf.clear();

    db_conn->uninit();
    db_conn->release();

    //反初始化设置proc标题
    uninit_proc_title();
    oa_popen_uninit();

    return 0;
}
