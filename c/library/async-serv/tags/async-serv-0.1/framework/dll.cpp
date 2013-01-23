/**
 * @file dll.cpp
 * @brief 读取动态连接库函数指针类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-31
*/

#include <dlfcn.h>

#include "dll.h"
#include "log.h"

dll::~dll()
{
    if (m_p_handler_) {
        ::dlclose(m_p_handler_);
        m_p_handler_ = NULL;
    }
}

int dll::register_plugin()
{
    m_p_handler_ = ::dlopen(m_dll_path_.c_str(), RTLD_NOW);
    if (!m_p_handler_) {
        ERROR_LOG("dlopen failed, err: %s", ::dlerror());
        return -1;
    }

    init_service = (init_service_t)::dlsym(m_p_handler_, "init_service");
    fini_service = (fini_service_t)::dlsym(m_p_handler_, "fini_service");

    proc_udp_pkg_from_client =
        (proc_pkg_from_client_t)::dlsym(m_p_handler_, "proc_udp_pkg_from_client");
    proc_udp_pkg_from_serv =
        (proc_udp_pkg_from_serv_t)::dlsym(m_p_handler_, "proc_udp_pkg_from_serv");
    on_udp_serv_conn_complete =
        (on_udp_serv_conn_complete_t)::dlsym(m_p_handler_, "on_udp_serv_conn_complete");

    if (!(get_pkg_len =
                (get_pkg_len_t)::dlsym(m_p_handler_, "get_pkg_len")))
        goto err;
    if (!(proc_pkg_from_client =
                (proc_pkg_from_client_t)::dlsym(m_p_handler_, "proc_pkg_from_client")))
        goto err;
    if (!(proc_pkg_from_serv =
                (proc_udp_pkg_from_serv_t)::dlsym(m_p_handler_, "proc_pkg_from_serv")))
        goto err;
    if (!(on_serv_conn_complete =
                (on_serv_conn_complete_t)::dlsym(m_p_handler_, "on_serv_conn_complete")))
        goto err;
    if (!(on_serv_conn_failed =
                (on_serv_conn_failed_t)::dlsym(m_p_handler_, "on_serv_conn_failed")))
        goto err;
    if (!(on_client_conn_closed =
                (on_client_conn_closed_t)::dlsym(m_p_handler_, "on_client_conn_closed")))
        goto err;
    if (!(on_serv_conn_closed =
                (on_serv_conn_closed_t)::dlsym(m_p_handler_, "on_serv_conn_closed")))
        goto err;

    return 0;
err:
    ERROR_LOG("dlsym err: %s", ::dlerror());
    return -1;
}
