/** 
 * ========================================================================
 * @file instance.cpp
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-11-15
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "instance.h"
#include "itl_timer.h"
#include "../node.h"
#include "../db.h"
#include "../proto.h"


timer_head_t g_instance_event;
bool g_instance_inited = false;

int init_mysql_instance_timely(void  * owner, void * data)
{

    ADD_TIMER_EVENT(&g_instance_event, init_mysql_instance_timely, NULL, get_now_tv()->tv_sec + GET_NODE_CONFIG_INTERVAL);

    if (g_instance_inited)
    {
        return 0;
    }

    if (0 == g_node_ip)
    {
        return 0;
    }


    db_get_node_mysql_instance(g_node_ip_str);

    return 0;
}

int init_mysql_instance()
{
    init_mysql_instance_timely(NULL, NULL);
    return 0;
}

int init_mysql_instance(Cmessage * c_out)
{
    instance_mgr.uninit();

    db_p_get_node_mysql_instance_out * p_out = P_OUT;

    vector_for_each(p_out->db_instance_info, it)
    {
        db_instance_t * p_instance_info = &(*it);
        c_mysql_instance * p_instance = new (std::nothrow) c_mysql_instance;
        if (NULL == p_instance)
        {
            return -1;
        }
        p_instance->m_port = p_instance_info->port;
        STRNCPY(p_instance->m_sock, p_instance_info->sock, sizeof(p_instance->m_sock));

        DEBUG_LOG("mysql instance[%u: %s]", p_instance->m_port, p_instance->m_sock);

        instance_mgr.insert_object(p_instance->m_port, p_instance);
    }

    g_instance_inited = true;
    return 0;
}


int fini_mysql_instance()
{
    instance_mgr.uninit();

    g_instance_inited = false;

    return 0;
}


int refresh_mysql_instance()
{
    db_get_node_mysql_instance(g_node_ip_str);
    return 0;
}

const c_mysql_instance * find_mysql_instance(uint32_t port)
{
    c_mysql_instance * p_instance = instance_mgr.get_object(port);
    return p_instance;
}
