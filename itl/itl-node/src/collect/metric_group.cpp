/** 
 * ========================================================================
 * @file metric_group.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-10
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include <string.h>
#include <errno.h>
#include "metric_group.h"
#include "../proto.h"
#include "collect.h"



int init_server_metric_group(Cmessage * c_out)
{
    db_p_get_node_server_config_out * p_out = P_OUT;

    DEBUG_LOG("=== INIT SERVER METRIC GROUP CONFIG ===");

    vector_for_each(p_out->group_info, group_it)
    {
        group_config_t * p_group_config = &(*group_it);

        // group id随意定的
        uint32_t group_id = metric_group_mgr.size();
        uint32_t collect_interval = p_group_config->collect_interval;
#ifdef SHORT_COLLECT_INTERVAL
        collect_interval = 5;
#endif

        c_metric_group * p_group = new c_metric_group;
        if (NULL == p_group)
        {
            ERROR_LOG("malloc metric group failed, %s", strerror(errno));
            return -1;
        }

        p_group->m_group_id = group_id;
        p_group->m_collect_interval = collect_interval;
        if (0 == p_group->m_collect_interval)
        {
            p_group->m_collect_interval = DEFAULT_COLLECT_INTERVAL;
        }

        DEBUG_LOG("group id: %u, collect interval: %u", 
                group_id, p_group->m_collect_interval);

        vector_for_each(p_group_config->metric_config, metric_it)
        {
            metric_config_t * p_metric_config = &(*metric_it);

            uint32_t metric_id = p_metric_config->metric_id;
            c_metric * p_metric = metric_mgr.get_object(metric_id);
            if (NULL == p_metric)
            {
                p_metric = new c_metric;
                p_metric->m_metric_id = metric_id;
                STRNCPY(p_metric->m_metric_name, p_metric_config->metric_name, MAX_METRIC_NAME_LEN);
                p_metric->m_metric_type = p_metric_config->metric_type;
                p_metric->m_metric_class = ITL_TYPE_SERVER;
                if (0 != attach_so(p_metric))
                {
                    ERROR_LOG("attach so failed, metric[%u: %s]", p_metric->m_metric_id, p_metric->m_metric_name);
                    delete p_metric;
                    continue;
                }
                metric_mgr.insert_object(p_metric->get_id(), p_metric);
            }

            char * arg = p_metric_config->metric_arg;

            DEBUG_LOG("\tmetric[%u: %s], type: %u, arg: %s", 
                    p_metric->get_id(),
                    p_metric->get_name(),
                    p_metric->get_type(),
                    arg);


            if (0 == arg[0])
            {
                c_metric_arg * p_arg = add_metric_arg(p_metric, arg);
                p_group->m_metric_arg.push_back(p_arg);
            }
            else
            {
                char * save_ptr = NULL;
                char * token = strtok_r(arg, ",", &save_ptr);
                while (NULL != token)
                {
                    c_metric_arg * p_arg = add_metric_arg(p_metric, token);
                    p_group->m_metric_arg.push_back(p_arg);
                    token = strtok_r(NULL, ",", &save_ptr);
                }
            }





        }

        metric_group_mgr.insert_object(group_id, p_group);
    }

    DEBUG_LOG("=== SERVER METRIC GROUP CONFIG END ===");


    return 0;
}


int init_mysql_metric_group(Cmessage * c_out)
{
    db_p_get_node_mysql_config_out * p_out = P_OUT;


    DEBUG_LOG("=== INIT MYSQL METRIC GROUP CONFIG ===");

    vector_for_each(p_out->db_group_info, instance_it)
    {
        db_node_config_t * p_instance_config = &(*instance_it);



        vector_for_each(p_instance_config->group_info, group_it)
        {
            group_config_t * p_group_config = &(*group_it);
            uint32_t group_id = metric_group_mgr.size();
            uint32_t collect_interval = p_group_config->collect_interval;
            c_metric_group * p_group = new c_metric_group;
            if (NULL == p_group)
            {
                ERROR_LOG("malloc metric group failed, %s", strerror(errno));
                return -1;
            }

            p_group->m_group_id = group_id;
            p_group->m_collect_interval = collect_interval;
            if (0 == p_group->m_collect_interval)
            {
                p_group->m_collect_interval = DEFAULT_COLLECT_INTERVAL;
            }

            DEBUG_LOG("group id: %u, collect interval: %u", 
                    group_id, p_group->m_collect_interval);

            vector_for_each(p_group_config->metric_config, metric_it)
            {
                metric_config_t * p_metric_config = &(*metric_it);

                uint32_t metric_id = p_metric_config->metric_id;
                c_metric * p_metric = metric_mgr.get_object(metric_id);
                if (NULL == p_metric)
                {
                    p_metric = new c_metric;
                    p_metric->m_metric_id = metric_id;
                    STRNCPY(p_metric->m_metric_name, p_metric_config->metric_name, MAX_METRIC_NAME_LEN);
                    p_metric->m_metric_type = p_metric_config->metric_type;
                    p_metric->m_metric_class = ITL_TYPE_MYSQL;
                    if (0 != attach_so(p_metric))
                    {
                        ERROR_LOG("attach so failed, metric[%u: %s]", p_metric->m_metric_id, p_metric->m_metric_name);
                        delete p_metric;
                        continue;
                    }
                    metric_mgr.insert_object(p_metric->get_id(), p_metric);
                }

                // 用port作为arg
                char port_str[8];
                uint32_t port = p_instance_config->instance_info.port;
                snprintf(port_str, sizeof(port_str), "%u", port);

                c_metric_arg * p_arg = add_metric_arg(p_metric, port_str);
                p_group->m_metric_arg.push_back(p_arg);

                DEBUG_LOG("\tmetric id: %u, name: %s, type: %u, arg: %u", 
                        p_metric->get_id(),
                        p_metric->get_name(),
                        p_metric->get_type(),
                        port);


            }
            metric_group_mgr.insert_object(group_id, p_group);
        }

    }

    DEBUG_LOG("=== MYSQL METRIC GROUP CONFIG END ===");



    return 0;
}


int fini_metric_group()
{
    metric_group_mgr.uninit();
    metric_mgr.uninit();
    return 0;
}
