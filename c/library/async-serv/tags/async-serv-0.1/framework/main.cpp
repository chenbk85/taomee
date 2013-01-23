/**
 * @file main.cpp
 * @brief main函数
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-01
 */

#include <signal.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include <iostream>
#include <list>
#include <new>
#include <vector>

#include "log.h"
#include "acceptor.hpp"
#include "reactor.h"
#include "sock_acceptor.h"
#include "sock_dgram.h"
#include "connection.hpp"
#include "inet_address.h"
#include "connections_pool.hpp"
#include "vpipe_sockpair.h"
#include "http_serv.h"
#include "http_inspector.hpp"

#include "async_serv.h"
#include "dll.h"
#include "daemon.h"
#include "bind_conf.h"
#include "i_ini_file.h"
#include "udp_io_event.h"
#include "tcp_io_event.h"

typedef struct {
    u_int id;
    pid_t worker_pid;
    connection<vpipe_sockpair>* p_sockpair;
} worker_proc_t;

char** g_saved_argv = NULL;
int g_saved_argc = 0;
char* g_p_current_dir = NULL;
char* g_prog_name = NULL;
i_ini_file* g_p_ini_file = NULL;
dll g_dll_inst;
int g_work_num = 0;

volatile sig_atomic_t g_stop = 0;
volatile sig_atomic_t g_restart = 0;

std::list<acceptor<connection<sock_stream>, sock_acceptor>* > g_tcp_acceptor_list;
std::list<acceptor<connection<sock_dgram>, sock_dgram>* > g_udp_acceptor_list;
std::vector<worker_proc_t> g_worker_proc_vec;

static int log_init(i_ini_file* p_ini_file);
static void master_proc_loop();
static void worker_proc_loop(int id);

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " conf_file ..." << std::endl;
        return 0;
    }

    ///创建i_ini_file实例
    if (create_ini_file_instance(&g_p_ini_file)) {
        std::cerr << "create ini_file instance failed" << std::endl;
        return -1;
    }

    if (g_p_ini_file->init(argv[1]) < 0) {
        std::cerr << "ini_file init failed, err: " << g_p_ini_file->get_last_errstr() << std::endl;
        g_p_ini_file->release();
        g_p_ini_file = NULL;
        return -1;
    }

    ///开启精灵模式
    if (daemon_start(argc, argv, g_p_ini_file) < 0) {
        return -1;
    }

    ///初始化日志系统
    if (log_init(g_p_ini_file) < 0) {
        return -1;
    }

    ///读取dll中函数接口
    char path[PATH_MAX] = {0};
    if (g_p_ini_file->read_string("WorkInfo", "dll", path, sizeof(path), NULL)) {
        ERROR_LOG("fail to read dll_path from conf_file");
        return -1;
    }

    g_dll_inst.set_dll_path(path);
    if (g_dll_inst.register_plugin() < 0) {
        ERROR_LOG("fail to register dll");
        return -1;
    }

    ///读取bind_file中的内容
    if (g_p_ini_file->read_string("WorkInfo", "bind_file", path, sizeof(path), NULL)) {
        ERROR_LOG("fail to read bind_file path from conf_file");
        return -1;
    }

    std::list<bind_conf_t> bind_conf_list;
    bind_conf bind_conf_inst(path);
    if (bind_conf_inst.load_bind_conf(bind_conf_list) < 0) {
        ERROR_LOG("fail to read info from bind_file");
        return -1;
    }

    ///初始化注册回调函数
    tcp_io_event tcp_io_event_inst;
    udp_io_event udp_io_event_inst;

    if (g_dll_inst.proc_pkg_from_client)
        tcp_io_event_inst.set_tcp_proc_callback(g_dll_inst.proc_pkg_from_client);
    if (g_dll_inst.get_pkg_len)
        tcp_io_event_inst.set_tcp_pkg_len_callback(g_dll_inst.get_pkg_len);
    if (g_dll_inst.on_client_conn_closed)
        tcp_io_event_inst.set_tcp_close_callback(g_dll_inst.on_client_conn_closed);

    if (g_dll_inst.proc_udp_pkg_from_client)
        udp_io_event_inst.set_udp_proc_callback(g_dll_inst.proc_udp_pkg_from_client);
    if (g_dll_inst.on_client_conn_closed)
        udp_io_event_inst.set_udp_close_conn_callback(g_dll_inst.on_client_conn_closed);

    ///初始化网络监听

    std::list<bind_conf_t>::iterator it = bind_conf_list.begin();
    for (; it != bind_conf_list.end(); ++it) {
            inet_address serv_addr;
#ifdef _IPV6
            if (it->ip_addr == "0:0:0:0:0:0:0:0"
                    || ip->ip_addr == "::"
                    || ip->ip_addr == "::/128") {
                serv_addr.set_port(it->port);
            } else {
                serv_addr.set_ip_addr(it->ip_addr);
                serv_addr.set_port(it->port);
            }
#else
            if (it->ip_addr == "0.0.0.0") {
                serv_addr.set_port(it->port);
            } else {
                serv_addr.set_ip_addr(it->ip_addr);
                serv_addr.set_port(it->port);
            }
#endif

        if (it->type == TYPE_TCP) {
            acceptor<connection<sock_stream>, sock_acceptor>* p_acceptor =
                new (std::nothrow) acceptor<connection<sock_stream>, sock_acceptor>(serv_addr);
            if (!p_acceptor) {
                ERROR_LOG("tcp acceptor instantiate failed");
                return -1;
            }

            p_acceptor->set_new_conn_callback(
                    boost::bind(&tcp_io_event::tcp_on_new_conn, &tcp_io_event_inst, _1));
            p_acceptor->set_on_message_callback(
                    boost::bind(&tcp_io_event::tcp_on_message, &tcp_io_event_inst, _1, _2, _3));
            //p_acceptor->set_on_write_comple_callback(
            //        boost::bind(&tcp_io_event::tcp_on_write_complete, &tcp_io_event_inst, _1));
            p_acceptor->set_on_close_callback(
                    boost::bind(&tcp_io_event::tcp_on_close_conn, &tcp_io_event_inst, _1));

            if (p_acceptor->start() < 0) {
                return -1;
            }
            g_tcp_acceptor_list.push_back(p_acceptor);

        } else {
            acceptor<connection<sock_dgram>, sock_dgram>* p_acceptor =
                new (std::nothrow) acceptor<connection<sock_dgram>, sock_dgram>(serv_addr);
            if (!p_acceptor) {
                ERROR_LOG("udp acceptor instantiate failed");
                return -1;
            }

            p_acceptor->set_new_conn_callback(
                    boost::bind(&udp_io_event::udp_on_new_conn, &udp_io_event_inst, _1));
            p_acceptor->set_on_message_callback(
                    boost::bind(&udp_io_event::udp_on_message, &udp_io_event_inst, _1, _2, _3));
            //p_acceptor->set_on_write_comple_callback(
            //        boost::bind(&udp_io_event::udp_on_write_comple, &udp_io_event_inst, _1));
            p_acceptor->set_on_close_callback(
                    boost::bind(&udp_io_event::udp_on_close_conn, &udp_io_event_inst, _1));

            if (p_acceptor->start() < 0) {
                return -1;
            }

            g_udp_acceptor_list.push_back(p_acceptor);
        }
    }


    ///创建socketpair
    g_work_num = g_p_ini_file->read_int("WorkInfo", "worker_num", 1);
    for (int i = 0; i < g_work_num; i++) {
        connections_pool<connection<vpipe_sockpair> >* p_pool =
            connections_pool<connection<vpipe_sockpair> >::instance();
        assert(p_pool);
        connection<vpipe_sockpair>* p_conn = p_pool->get_new_connection();
        assert(p_conn);
        if ((p_conn->get_ipc_conn())->open() < 0) {
            return -1;
        }

        worker_proc_t tmp;
        tmp.id = i;
        tmp.worker_pid = 0;
        tmp.p_sockpair = p_conn;

        g_worker_proc_vec.push_back(tmp);
    }

    if (g_dll_inst.init_service
            && g_dll_inst.init_service(g_saved_argc, g_saved_argv, PROC_MAIN) != 0) {
        ERROR_LOG("main proc init_service failed");
        return -1;
    }

    daemon_set_title("%s-[%s]", g_prog_name, "master");

    ///fork工作进程
    for (int i = 0; i < g_work_num; i++) {
        pid_t pid = fork();
        switch (pid) {
            case -1:
                ERROR_LOG("fork failed, err: %s", strerror(errno));
                return -1;
            case 0:
                ///子进程
                worker_proc_loop(i);
            default:
                ///master进程
                g_worker_proc_vec[i].worker_pid = pid;
                break;
        }
    }

    ///设置work进程cpu亲和性
    for (int i = 0; i < g_work_num; i++) {
        if (set_cpu_affinity(g_work_num,
                             g_worker_proc_vec[i].worker_pid,
                             g_worker_proc_vec[i].id) < 0) {
            DEBUG_LOG("set_cpu_affinity failed, err: %s", strerror(errno));
        }
    }

    master_proc_loop();
    daemon_stop();

    if (g_p_ini_file) {
        g_p_ini_file->uninit();
        g_p_ini_file->release();
        g_p_ini_file = NULL;
    }

    return 0;
}

static int log_init(i_ini_file* p_ini_file)
{
    char log_dir[PATH_MAX] = {0};
    if (p_ini_file->read_string("LogInfo", "log_dir", log_dir, PATH_MAX, NULL) != 0) {
        return -1;
    }

    int log_level = p_ini_file->read_int("LogInfo", "log_level", 8);
    int log_size = p_ini_file->read_int("LogInfo", "log_size", 1024*1024*100);
    int log_count = p_ini_file->read_int("LogInfo", "log_maxfiles", 0);
    char log_prefix[NAME_MAX] = {0};
    if (p_ini_file->read_string("LogInfo", "log_prefix", log_prefix, NAME_MAX, NULL) != 0) {
        return -1;
    }

    if (log_init(log_dir, (log_lvl_t)log_level, log_size, log_count, log_prefix) != 0) {
        return -1;
    }

    //enable_multi_thread();
    return 0;
}


static void master_proc_loop()
{
    if (reactor::instance()->reactor_init() < 0) {
        return;
    }

    std::vector<pid_t> work_pid_vec;
    ///向反应器注册socketpair
    std::vector<worker_proc_t>::iterator it = g_worker_proc_vec.begin();
    for (; it != g_worker_proc_vec.end(); ++it) {
        work_pid_vec.push_back(it->worker_pid);
        (it->p_sockpair->get_ipc_conn())->set_sockpair_type(SOCKPAIR_TYPE_1);
        if ((it->p_sockpair)->open() < 0) {
            return;
        }
    }

    ///实例化http回调接口函数对象
    http_inspector http_inspector_inst(work_pid_vec);
    http_serv* p_http_serv = NULL;

    char buf[10] = {0};
    g_p_ini_file->read_string("WorkInfo", "internal_http_serv", buf, sizeof(buf), NULL);
    if (!strncasecmp(buf, "on", 2)) {
        char http_serv_ip[16] = {0};
        u_short http_serv_port = 0;
        ///开启http监控器
        g_p_ini_file->read_string("WorkInfo",
                                  "internal_http_serv_ip",
                                  http_serv_ip,
                                  sizeof(http_serv_ip),
                                  "127.0.0.1");
        http_serv_port = g_p_ini_file->read_int("WorkInfo",
                                                "internal_http_serv_port",
                                                8080);
        inet_address serv_addr(http_serv_ip, http_serv_port);
        p_http_serv = new http_serv(serv_addr);
        assert(p_http_serv);
        p_http_serv->set_http_callback(http_inspector_inst);
        p_http_serv->start();
    }

    while (!g_stop && !g_restart) {
        reactor::instance()->reactor_wait(500);
    }

    if (p_http_serv) {
        delete p_http_serv;
        p_http_serv = NULL;
    }

    if (g_dll_inst.fini_service) {
        g_dll_inst.fini_service(PROC_MAIN);
    }

    if (connections_pool<connection<sock_stream> >::instance()) {
        connections_pool<connection<sock_stream> >::instance()->destroy_conns_pool();
        delete connections_pool<connection<sock_stream> >::instance();
    }
    if (connections_pool<connection<sock_dgram> >::instance()) {
        connections_pool<connection<sock_dgram> >::instance()->destroy_conns_pool();
        delete connections_pool<connection<sock_dgram> >::instance();
    }
    if (connections_pool<connection<vpipe_sockpair> >::instance()) {
        connections_pool<connection<vpipe_sockpair> >::instance()->destroy_conns_pool();
        delete connections_pool<connection<vpipe_sockpair> >::instance();
    }

    std::list<acceptor<connection<sock_stream>, sock_acceptor>* >::iterator tcp_it;
    for (tcp_it = g_tcp_acceptor_list.begin();
         tcp_it != g_tcp_acceptor_list.end();
         ++tcp_it) {
        delete *tcp_it;
        *tcp_it = NULL;
    }

    std::list<acceptor<connection<sock_dgram>, sock_dgram>* >::iterator udp_it;
    for (udp_it = g_udp_acceptor_list.begin();
         udp_it != g_udp_acceptor_list.end();
         ++udp_it) {
        delete *udp_it;
        *udp_it = NULL;
    }

    if (reactor::instance()) {
        reactor::instance()->reactor_fini();
        delete reactor::instance();
    }

}

static void worker_proc_loop(int id)
{

    daemon_set_title("%s-[%s]", g_prog_name, "worker");

    std::vector<worker_proc_t>::iterator it;
    std::list<acceptor<connection<sock_stream>, sock_acceptor>* >::iterator tcp_iter;
    std::list<acceptor<connection<sock_dgram>, sock_dgram>* >::iterator udp_iter;

    ///清除并初始化继承下来的reactor
    if (reactor::instance()->reactor_fini() < 0
            || reactor::instance()->reactor_init() < 0) {
        goto out;
    }

    ///清理继承下来的fd
    for ( it = g_worker_proc_vec.begin();
          it != g_worker_proc_vec.end();
          ++it) {
        if ((int)it->id == id) {
            (it->p_sockpair->get_ipc_conn())->set_sockpair_type(SOCKPAIR_TYPE_2);
            ((it->p_sockpair)->get_ipc_conn())->close_fd1();
            (it->p_sockpair)->open();
        } else {
            if (it->p_sockpair) {
                (it->p_sockpair)->recycle_connection();
            }
            it->p_sockpair = NULL;
            it->worker_pid = 0;
        }
    }

    for (tcp_iter = g_tcp_acceptor_list.begin();
         tcp_iter != g_tcp_acceptor_list.end();
         ++tcp_iter) {
        if ((*tcp_iter)->enable_accepting() < 0) {
            ERROR_LOG("tcp enable_accepting failed");
            goto out;
        }
    }

    for (udp_iter = g_udp_acceptor_list.begin();
         udp_iter != g_udp_acceptor_list.end();
         ++udp_iter) {
        if ((*udp_iter)->enable_accepting() < 0) {
            ERROR_LOG("tcp enable_accepting failed");
            goto out;
        }
    }

    if (g_dll_inst.init_service
            && g_dll_inst.init_service(g_saved_argc, g_saved_argv, PROC_WORK) < 0) {
        ERROR_LOG("work proc init_service failed");
        goto out;
    }

    while (!g_stop && !g_restart) {
        reactor::instance()->reactor_wait(500);
    }

    if (g_dll_inst.fini_service) {
        g_dll_inst.fini_service(PROC_MAIN);
    }

out:
    if (connections_pool<connection<sock_stream> >::instance()) {
        connections_pool<connection<sock_stream> >::instance()->destroy_conns_pool();
        delete connections_pool<connection<sock_stream> >::instance();
    }
    if (connections_pool<connection<sock_dgram> >::instance()) {
        connections_pool<connection<sock_dgram> >::instance()->destroy_conns_pool();
        delete connections_pool<connection<sock_dgram> >::instance();
    }
    if (connections_pool<connection<vpipe_sockpair> >::instance()) {
        connections_pool<connection<vpipe_sockpair> >::instance()->destroy_conns_pool();
        delete connections_pool<connection<vpipe_sockpair> >::instance();
    }

    for (tcp_iter = g_tcp_acceptor_list.begin();
         tcp_iter != g_tcp_acceptor_list.end();
         ++tcp_iter) {
        delete *tcp_iter;
        *tcp_iter = NULL;
    }

    for (udp_iter = g_udp_acceptor_list.begin();
         udp_iter != g_udp_acceptor_list.end();
         ++udp_iter) {
        delete *udp_iter;
        *udp_iter = NULL;
    }

    if (reactor::instance()) {
        reactor::instance()->reactor_fini();
        delete reactor::instance();
    }

    char** argv = NULL;
    for (argv = g_saved_argv; *argv; ++argv) {
        free(*argv);
        *argv = NULL;
    }
    free(g_saved_argv);
    g_saved_argv = NULL;
    g_saved_argc = 0;

    free(g_prog_name);
    free(g_p_current_dir);

    if (g_p_ini_file) {
        g_p_ini_file->uninit();
        g_p_ini_file->release();
        g_p_ini_file = NULL;
    }

    exit(0);
}

void restart_worker_proc(int pid)
{
    std::vector<worker_proc_t>::iterator it;
    for (it = g_worker_proc_vec.begin();
         it != g_worker_proc_vec.end();
         ++it) {
        if (it->worker_pid == pid) {
            ///回收原来的socketpair，并创建新的socketpair
            (it->p_sockpair)->recycle_connection();

            connections_pool<connection<vpipe_sockpair> >* p_pool =
                connections_pool<connection<vpipe_sockpair> >::instance();
            assert(p_pool);

            connection<vpipe_sockpair>* p_conn = p_pool->get_new_connection();
            assert(p_conn);
            (p_conn->get_ipc_conn())->set_sockpair_type(SOCKPAIR_TYPE_1);
            (p_conn->get_ipc_conn())->open();
            p_conn->open();

            it->p_sockpair = p_conn;
            it->worker_pid = 0;
            break;
        }
    }

    pid_t worker_pid = fork();
    switch(worker_pid) {
        case -1:
            ERROR_LOG("fork failed, err: %s", strerror(errno));
            return;
        case 0:
            worker_proc_loop(it->id);
        default:
            it->worker_pid = worker_pid;
            break;
    }
}
