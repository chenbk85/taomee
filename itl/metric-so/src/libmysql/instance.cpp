/** 
 * ========================================================================
 * @file instance.cpp
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-09-07
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/inet/inet_utils.h>
}
#include "instance.h"



#define DEFAULT_MONITOR_USER    "monitor"
#define DEFAULT_MONITOR_PASS    "monitor@tmpwd"


c_instance * alloc_instance(uint32_t port, const char * sock)
{
    c_instance * p_instance = new (std::nothrow) c_instance;
    if (NULL == p_instance)
    {
        return NULL;
    }
    p_instance->m_conn = NULL;
    p_instance->m_port = port;
    STRNCPY(p_instance->m_sock, sock, sizeof(p_instance->m_sock));
    snprintf(p_instance->m_host, sizeof(p_instance->m_host), "localhost");
    snprintf(p_instance->m_user, sizeof(p_instance->m_user), DEFAULT_MONITOR_USER);
    snprintf(p_instance->m_pass, sizeof(p_instance->m_pass), DEFAULT_MONITOR_PASS);

    p_instance->m_alive = false;
    p_instance->m_last_connect_time = 0;
    p_instance->m_metric_map = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free_metric);

    instance_mgr.insert_object(port, p_instance);

    return p_instance;
}



/**
 * @brief  初始化一个MYSQL对象指针并返回
 * @return unnull-succ, null-fail
 */
MYSQL * init_mysql()
{
    MYSQL * mysql_conn = NULL;

    mysql_conn = mysql_init(NULL);
    if (mysql_conn == NULL)
    {
        ERROR_LOG("mysql_init(NULL) failed.");
        return NULL;
    }

    ///设置自动重连标志
    my_bool is_reconnect = 1;
    if (mysql_options(mysql_conn, MYSQL_OPT_RECONNECT, &is_reconnect))
    {
        ERROR_LOG("mysql_options(MYSQL_OPT_RECONNECT) failed: %s", mysql_error(mysql_conn));
        mysql_close(mysql_conn);
        return NULL;
    }

    ///全部统一用域套接口来和服务器做连接
    unsigned int protocol_type = MYSQL_PROTOCOL_SOCKET;
    if (mysql_options(mysql_conn, MYSQL_OPT_PROTOCOL, (const char *)&protocol_type))
    {
        ERROR_LOG("mysql_options(MYSQL_OPT_PROTOCOL) failed: %s", mysql_error(mysql_conn));
        mysql_close(mysql_conn);
        return NULL;
    }

    return mysql_conn;
}



int connect_instance(c_instance * p_instance)
{
    time_t now = time(NULL);
    if (now < p_instance->m_last_connect_time + 1)
    {
        return -1;
    }

    if (p_instance->is_connected())
    {
        mysql_close(p_instance->m_conn);
    }

    p_instance->m_conn = init_mysql();
    if (NULL == p_instance->m_conn)
    {
        ERROR_LOG("Can not create mysql object.");
        return -1;
    }

    const char * sock = p_instance->m_sock;
    const char * host = p_instance->m_host;
    uint32_t port = p_instance->m_port;
    const char * user = p_instance->m_user;
    const char * pass = p_instance->m_pass;
    MYSQL * conn = p_instance->m_conn;
    const char * db = "information_schema";
    uint64_t flag = 
        CLIENT_INTERACTIVE | 
        CLIENT_MULTI_STATEMENTS | 
        CLIENT_REMEMBER_OPTIONS;
    if (NULL == mysql_real_connect(conn, host, user, pass, db, port, sock, flag))
    {
        ERROR_LOG("connect to mysql failed: %s", 
                mysql_error(conn));
        ERROR_LOG("host: %s:%u, sock: %s, user: %s, pass: %s",
                host, port, sock, user, pass);
        p_instance->disconnect();
        return -1;
    }

    if (0 != mysql_set_character_set(conn, "utf8"))
    {
        ERROR_LOG("mysql_set_character_set(utf8) failed[%s]", mysql_error(conn));
        p_instance->disconnect();
        return -1;
    }

    p_instance->m_alive = true;
    p_instance->m_last_connect_time = now;

    DEBUG_LOG("mysql connected, host: %s:%u, sock: %s, user: %s, pass: %s", 
            host, port, sock, user, pass);

    return 0;
}


int free_instance(c_instance * p_instance, void * param)
{
    if (NULL != p_instance->m_metric_map)
    {
        g_hash_table_destroy(p_instance->m_metric_map);
        p_instance->m_metric_map = NULL;
    }

    if (p_instance->is_connected())
    {
        p_instance->disconnect();
    }

    return 0;
}


int dealloc_instance(c_instance * p_instance)
{
    instance_mgr.remove_object(p_instance->m_port, free_instance, NULL);
    return 0;

}


c_instance * find_instance(uint32_t port)
{
    c_instance * p_instance = instance_mgr.get_object(port);
    if (NULL == p_instance)
    {
        ERROR_LOG("no instance for port[%u]", port);
        return NULL;
    }

    if (!p_instance->is_connected())
    {
        if (0 != connect_instance(p_instance))
        {
            return NULL;
        }
        else
        {
            return p_instance;
        }
    }
    else
    {
        if (0 != mysql_ping(p_instance->m_conn))
        {
            ERROR_LOG("mysql_ping failed: %s", 
                    mysql_error(p_instance->m_conn));
            p_instance->m_alive = false;
            p_instance->disconnect();
            return p_instance;
        }
        else
        {
            return p_instance;
        }
    }

}


int init_instances(std::vector< mysql_so_param_t > * p_vec)
{
    if (NULL == p_vec)
    {
        return -1;
    }

    vector_for_each(*p_vec, it)
    {
        mysql_so_param_t * p_config = &(*it);
        uint32_t port = p_config->port;
        DEBUG_LOG("instance: port[%u], sock[%s]", 
                port, p_config->sock);

        if (!is_legal_port(port))
        {
            ERROR_LOG("illegal port[%u]", port);
            continue;
        }

        c_instance * p_instance = alloc_instance(port, p_config->sock);
        if (NULL == p_instance)
        {
            continue;
        }

        // connect_instance(p_instance);

        vector_for_each(p_config->metric_vec, metric_it)
        {
            const char * metric_name = (*metric_it).c_str();
            DEBUG_LOG("\tmysql metric: %s", metric_name);
            for (uint32_t i = 0; i < METRIC_NUM; i++)
            {
                if (0 == strncmp(metric_name, g_metrics_info[i].name, MAX_METRIC_NAME_LEN))
                {
                    uint32_t metric_type = g_metrics_info[i].value_type;
                    c_metric * p_metric = alloc_metric(p_instance, metric_name, metric_type);
                    if (NULL == p_metric)
                    {
                        continue;
                    }

                    p_metric->m_slope = g_metrics_info[i].slope;
                    break;
                }
            }

            if (0 == strncmp(metric_name, "key_buffer_read_hit_rate", MAX_METRIC_NAME_LEN))
            {
                // 把相关的metric加进去
                alloc_metric(p_instance, "key_read_requests", ITL_VALUE_DOUBLE);
                alloc_metric(p_instance, "key_reads", ITL_VALUE_DOUBLE);

            }
            else if (0 == strncmp(metric_name, "key_buffer_write_hit_rate", MAX_METRIC_NAME_LEN))
            {
                // 把相关的metric加进去
                alloc_metric(p_instance, "key_write_requests", ITL_VALUE_DOUBLE);
                alloc_metric(p_instance, "key_writes", ITL_VALUE_DOUBLE);
            }

        }
    }

    return 0;
}


int fini_instances()
{
    instance_mgr.uninit(free_instance, NULL);
    return 0;
}
