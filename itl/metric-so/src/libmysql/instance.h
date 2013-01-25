/** 
 * ========================================================================
 * @file instance.h
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-09-07
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_INSTANCE_H_2012_09_07
#define H_INSTANCE_H_2012_09_07

#include <mysql/mysql.h>
#include <glib.h>
#include "itl_common.h"
#include "metric.h"





class c_instance
{

    public:

        inline bool is_connected()
        {
            return (NULL != m_conn);
        }

        inline void disconnect()
        {
            if (is_connected())
            {
                m_last_connect_time = 0;
                mysql_close(m_conn);
                m_conn = NULL;
            }
        }

    public:


        MYSQL * m_conn;
        char m_host[IP_STRING_LEN];
        uint32_t m_port;
        char m_sock[PATH_MAX + 1];
        char m_user[64];
        char m_pass[64];


    public:

        // 实例是否alive
        bool m_alive;
        // 上一次连接的时间
        // 防止短时间内大量地重复连接
        time_t m_last_connect_time;

    public:

        GHashTable * m_metric_map;

};


#define instance_mgr c_single_container(uint32_t, c_instance)

c_instance * find_instance(uint32_t port);
int connect_instance(c_instance * p_instance);

int init_instances(std::vector< mysql_so_param_t > * p_vec);
int fini_instances();
#endif
