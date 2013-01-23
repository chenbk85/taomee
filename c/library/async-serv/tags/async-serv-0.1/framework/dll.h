/**
 * @file dll.h
 * @brief 读取动态连接库函数指针类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-31
 */
#ifndef _H_DLL_H_
#define _H_DLL_H_

#include <boost/noncopyable.hpp>
#include <string>

#include "async_serv.h"

class dll : boost::noncopyable {
public:
    proc_pkg_from_client_t proc_pkg_from_client;
    proc_pkg_from_serv_t proc_pkg_from_serv;

    on_serv_conn_complete_t on_serv_conn_complete;
    on_serv_conn_failed_t on_serv_conn_failed;

    on_client_conn_closed_t  on_client_conn_closed;
    on_serv_conn_closed_t on_serv_conn_closed;

    init_service_t init_service;
    fini_service_t fini_service;

    get_pkg_len_t get_pkg_len;

    proc_udp_pkg_from_client_t proc_udp_pkg_from_client;
    proc_udp_pkg_from_serv_t proc_udp_pkg_from_serv;
    on_udp_serv_conn_complete_t on_udp_serv_conn_complete;

public:
    dll(const std::string& dll_path)
        : m_dll_path_(dll_path),
          m_p_handler_(NULL) { }

    dll()
        : m_p_handler_(NULL) { }

    ~dll();

    int register_plugin();
    void set_dll_path(const std::string& dll_path) { m_dll_path_ = dll_path; }
private:
    std::string m_dll_path_;
    void* m_p_handler_;
};

#endif

