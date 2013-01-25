/** 
 * ========================================================================
 * @file node.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-13
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include <string.h>
#include <errno.h>

#include "node.h"
#include "alarm.h"
#include "db.h"
#include "check_host_alive.h"



const char * g_default_rtx = config_get_strval("rtx_contact", "");
const char * g_default_email = config_get_strval("email_contact", "");
const char * g_default_mobile = config_get_strval("mobile_contact", "");
// host down的阀值，连续几次ping不通
const uint32_t g_host_down_threshold = config_get_intval("host_down_threshold", 3);
// host up的阀值，连续几次有metric数据上来
const uint32_t g_host_up_threshold = config_get_intval("host_up_threshold", 2);


int add_check_host_timer(c_node * p_node);


uint32_t c_node::get_span(uint32_t alarmed_count)
{
    if (0 == alarmed_count)
    {
        return 0;
    }
    
    
    uint32_t key = alarmed_count;
    uint32_t span = 0;
    iterator_t(*m_span_map) it = m_span_map->find(key);
    if (m_span_map->end() == it)
    {
        // 其他未指明的alarmed_count就按照0处理
        it = m_span_map->find(0);
        if (m_span_map->end() == it)
        {
            // 使用默认的
            span = 60 * 60;
        }
        else
        {
            span = it->second;
        }
    }
    else
    {
        span = it->second;
    }
    return span;

    if (0 < alarmed_count && alarmed_count <= 5)
    {
        return (5 * 60);
    }

    if (5 < alarmed_count && alarmed_count <= 10)
    {
        return (15 * 60);
    }

    return (60 * 60);
}


bool c_node::is_ban(uint32_t time)
{
    if (0 == time)
    {
        time = get_now_tv()->tv_sec;
    }

    if (0 == m_ban_start_time && 0 == m_ban_end_time)
    {
        return false;
    }

    if (0 != m_ban_start_time && 0 == m_ban_end_time)
    {
        if (m_ban_start_time <= time)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    if (0 == m_ban_start_time && 0 != m_ban_end_time)
    {
        if (time <= m_ban_end_time)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    if (m_ban_start_time <= time && time <= m_ban_end_time)
    {
        return true;
    }
    else
    {
        return false;
    }
}


int free_node(c_node * p_node, void * param);


int init_nodes()
{

    return 0;
}



int fini_nodes()
{
    node_mgr.uninit(free_node, NULL);

    return 0;
}


bool check_host_alive(c_node * p_node)
{
    if (p_node->m_node_running)
    {
        return true;
    }
    else
    {
        int ret = check_host_alive(long2ip(p_node->m_node_ip), 5);
        if (STATE_CRITICAL == ret)
        {
            return false;
        }
        else
        {
            return true;
        }

    }

}

int check_host(void * owner, void * param)
{
    c_node * p_node = reinterpret_cast< c_node * >(param);


    add_check_host_timer(p_node);

    bool host_alive = check_host_alive(p_node);
    p_node->m_node_running = false;

    if (p_node->m_host_alive)
    {
        if (host_alive)
        {
            p_node->m_up_count++;
        }
        else
        {
            p_node->m_down_count++;
            p_node->m_up_count = 0;

            if (p_node->m_down_count >= p_node->m_down_threshold)
            {
                // 切换到down状态
                p_node->m_host_alive = false;
                db_store_host_status(p_node, STATUS_HOST_DOWN);
                if (p_node->is_ban())
                {
                    WARN_LOG("host down banned, node[%u: %s]",
                            p_node->m_node_id,
                            long2ip(p_node->m_node_ip));
                }
                else
                {
                    uint32_t now = get_now_tv()->tv_sec;
                    p_node->m_alarm_start_time = now;
                    p_node->m_last_alarm_time = now;
                    p_node->m_alarm_count++;
                    send_host_alarm(p_node);
                }
            }
        }
    }
    else
    {
        if (host_alive)
        {
            p_node->m_up_count++;
            p_node->m_down_count = 0;

            if (p_node->m_up_count >= p_node->m_up_threshold)
            {
                // 切换到up状态
                p_node->m_host_alive = true;
                db_store_host_status(p_node, STATUS_HOST_UP);
                if (p_node->is_ban())
                {
                    WARN_LOG("host up banned, node[%u: %s]",
                            p_node->m_node_id,
                            long2ip(p_node->m_node_ip));

                }
                else
                {
                    send_host_alarm(p_node);
                }
                p_node->m_alarm_count = 0;
                p_node->m_last_alarm_time = 0;
                p_node->m_alarm_start_time = 0;

            }
        }
        else
        {
            p_node->m_down_count++;

            if (p_node->is_ban())
            {
                WARN_LOG("host down banned, node[%u: %s]",
                        p_node->m_node_id,
                        long2ip(p_node->m_node_ip));
            }
            else
            {
                uint32_t span = p_node->get_span(p_node->m_alarm_count);
                uint32_t now = get_now_tv()->tv_sec;
                if (now >= p_node->m_last_alarm_time + span)
                {
                    p_node->m_last_alarm_time = now;
                    p_node->m_alarm_count++;
                    send_host_alarm(p_node);
                }
            }
        }
    }


    return 0;
}


int add_check_host_timer(c_node * p_node)
{

    static uint32_t check_interval = config_get_intval("heartbeat", 60);

    p_node->m_host_timer = ADD_TIMER_EVENT(&p_node->m_event, check_host, p_node, get_now_tv()->tv_sec + check_interval);
    return 0;
}


int init_check_host_timer(c_node * p_node)
{
    add_check_host_timer(p_node);
    return 0;
}



int init_node_span(c_node * p_node, char * p_str)
{

    char * str_1 = p_str;
    char * save_ptr_1 = NULL;
    char * token_1 = NULL;
    while (NULL != (token_1 = strtok_r(str_1, ";", &save_ptr_1)))
    {
        str_1 = NULL;
        uint32_t start = 0;
        uint32_t end = 0;
        uint32_t interval = 0;

        char * str_2 = token_1;
        char * save_ptr_2 = NULL;

        // 取次数
        char * token_2 = strtok_r(str_2, ":", &save_ptr_2);
        if (NULL == token_2)
        {
            continue;
        }

        char * p = strchr(token_2, '-');
        if (NULL == p)
        {
            continue;
        }
        *p = 0;
        start = atoi(token_2);
        end = atoi(++p);



        // 取间隔
        str_2 = NULL;
        token_2 = strtok_r(str_2, "-", &save_ptr_2);
        if (NULL == token_2)
        {
            continue;
        }

        // 原始数据单位为分钟，转换成秒
        interval = atoi(token_2) * 60;

        if (0 == end)
        {
            p_node->m_span_map->insert(std::pair<uint32_t, uint32_t>(start, interval));
            p_node->m_span_map->insert(std::pair<uint32_t, uint32_t>(end, interval));
        }
        else
        {
            for (uint32_t i = start; i <= end; i++)
            {
                p_node->m_span_map->insert(std::pair<uint32_t, uint32_t>(i, interval));
            }

        }


    }

    return 0;
}





c_node * alloc_node(uint32_t node_id, int fd)
{
    c_node * p_node = new (std::nothrow) c_node;
    if (NULL == p_node)
    {
        ERROR_LOG("malloc node failed, %s", strerror(errno));
        return NULL;
    }

    p_node->m_node_id = node_id;
    p_node->m_head_fd = fd;
    p_node->m_project_id = 0;
    memset(p_node->m_project_name, 0, sizeof(p_node->m_project_name));

    p_node->m_ban_start_time = 0;
    p_node->m_ban_end_time = 0;

    p_node->m_alarm_count = 0;
    p_node->m_last_alarm_time = 0;
    p_node->m_alarm_start_time = 0;
    p_node->m_down_count = 0;
    p_node->m_up_count = 0;
    p_node->m_down_threshold = g_host_down_threshold;
    p_node->m_up_threshold = g_host_up_threshold;

    p_node->m_span_map = new (typeof(*(p_node->m_span_map)));
    char span[512] = {0};
    const char * span_config = config_get_strval("host_alarm_span", "1-5:5;6-10:15;11-0:60");
    memcpy(span, span_config, strlen(span_config));
    init_node_span(p_node, span);
    p_node->m_server_alarm_way = 0;
    p_node->m_mysql_alarm_way = 0;

    STRNCPY(p_node->m_server_mobile, g_default_mobile, sizeof(p_node->m_server_mobile));
    STRNCPY(p_node->m_server_email, g_default_email, sizeof(p_node->m_server_email));
    STRNCPY(p_node->m_server_rtx, g_default_rtx, sizeof(p_node->m_server_rtx));

    STRNCPY(p_node->m_mysql_mobile, g_default_mobile, sizeof(p_node->m_mysql_mobile));
    STRNCPY(p_node->m_mysql_email, g_default_email, sizeof(p_node->m_mysql_email));
    STRNCPY(p_node->m_mysql_rtx, g_default_rtx, sizeof(p_node->m_mysql_rtx));


    p_node->m_waitcmd = 0;
    INIT_LIST_HEAD(&(p_node->m_waitcmd_hook));
    p_node->m_pkg_queue = g_queue_new();


    p_node->m_host_timer = NULL;
    p_node->m_node_timer = NULL;

    p_node->m_host_alive = true;
    p_node->m_node_alive = true;
    p_node->m_node_running = true;
    p_node->m_inited = false;
    p_node->m_metric = new (typeof (*(p_node->m_metric)));

    node_mgr.insert_object(node_id, p_node);


    return p_node;
}

int free_node(c_node * p_node, void * param)
{
    if (!p_node->m_host_alive)
    {
        // 正在告警的，要告诉db告警结束
        p_node->m_host_alive = true;
        db_store_alarm_event_data(p_node, "");
    }
    if (NULL != p_node->m_node_timer)
    {
        do_remove_timer(p_node->m_node_timer, 1);
    }

    if (NULL != p_node->m_host_timer)
    {
        do_remove_timer(p_node->m_host_timer, 1);
    }


    cached_pkg_t * pkg = reinterpret_cast< cached_pkg_t * >(g_queue_pop_head(p_node->m_pkg_queue));
    while (pkg)
    {
        g_slice_free1(pkg->len, pkg);
        pkg = reinterpret_cast< cached_pkg_t * >(g_queue_pop_head(p_node->m_pkg_queue));
    }
    g_queue_free(p_node->m_pkg_queue);


    p_node->m_metric->uninit();
    delete p_node->m_metric;

    list_del_init(&(p_node->m_waitcmd_hook));


    delete p_node->m_span_map;



    delete p_node;

    return 0;
}

void dealloc_node(c_node * p_node)
{
    node_mgr.remove_object(p_node->m_node_id, free_node, NULL);
}





c_node * find_node(uint32_t node_id)
{
    return node_mgr.get_object(node_id);
}



const char * get_node_url(c_node * p_node, uint32_t type)
{
    static char url[1024];
    if (ITL_TYPE_SERVER == type)
    {
        snprintf(url, sizeof(url), 
                "http://itl.taomee.com/index.php?f=monitor%%26c=Node%%26a=index%%26project_id=%u%%26ip_inside=%s%%26classifyed=1%%26flag=0", 
                p_node->m_project_id, long2ip(p_node->m_node_ip));

    }
    else if (ITL_TYPE_MYSQL == type)
    {
        snprintf(url, sizeof(url), 
                "http://itl.taomee.com/index.php?f=monitor&c=Db&a=getProjectInfo&project_id=%u",
                p_node->m_project_id);

    }
    else
    {
        url[0] = 0;
    }

    return url;
}

int init_server_node(c_node * p_node, Cmessage * c_out)
{
    db_p_get_node_alarm_config_out * p_out = P_OUT;


    DEBUG_LOG("=== INIT SERVER ALARM CONFIG ===");

    p_node->m_node_ip = p_out->host_ip;
    p_node->m_project_id = p_out->project_id;
    STRNCPY(p_node->m_project_name, p_out->project_name, PROJECT_NAME_LEN);

    p_node->m_ban_start_time = p_out->node_contact.shield_time.start;
    p_node->m_ban_end_time = p_out->node_contact.shield_time.end;

    p_node->m_server_alarm_way = p_out->node_contact.alarm_way;

    if (!config_get_intval("use_default_contact_for_node", 0))
    {
        STRNCPY_LEN(p_node->m_server_mobile, &(p_out->node_contact.alarm_contact), mobile_list, CONTACT_LIST_LEN);
        STRNCPY_LEN(p_node->m_server_email, &(p_out->node_contact.alarm_contact), email_list, CONTACT_LIST_LEN);
        STRNCPY_LEN(p_node->m_server_rtx, &(p_out->node_contact.alarm_contact), rtx_list, CONTACT_LIST_LEN);

    }


    DEBUG_LOG("\tnode[%u: %s], project: [%u: %s]", 
            p_node->m_node_id, long2ip(p_node->m_node_ip), 
            p_node->m_project_id, p_node->m_project_name);

    DEBUG_LOG("\tmobile contact: %s", p_node->m_server_mobile);
    DEBUG_LOG("\temail contact: %s", p_node->m_server_email);
    DEBUG_LOG("\trtx contact: %s", p_node->m_server_rtx);


    vector_for_each(p_out->alarm_metric_list, it)
    {
        alarm_metric_t * p_alarm_config = &(*it);
        uint32_t metric_id = p_alarm_config->alarm_metric.id;
        c_metric * p_metric = p_node->m_metric->get_object(metric_id);
        if (NULL == p_metric)
        {
            p_metric = new c_metric(p_node);
            p_metric->m_metric_id = metric_id;
            STRNCPY_LEN(p_metric->m_metric_name, &(p_alarm_config->alarm_metric), name, MAX_METRIC_NAME_LEN);
            p_metric->m_metric_class = ITL_TYPE_SERVER;
            p_node->m_metric->insert_object(p_metric->m_metric_id, p_metric);
        }


        char * p_arg = p_alarm_config->alarm_arg.name;


        DEBUG_LOG("\tmetric id: %u, name: %s, arg: %s", 
                p_metric->m_metric_id, 
                p_metric->m_metric_name, 
                p_arg);



        c_metric_alarm * p_alarm = find_metric_alarm(p_metric->m_alarm, p_arg);
        if (NULL != p_alarm)
        {
            continue;
        }

        p_alarm = alloc_metric_alarm(p_metric, p_arg);
        if (NULL == p_alarm)
        {
            continue;
        }


        if (0 != init_metric_alarm(p_alarm, p_alarm_config))
        {
            ERROR_LOG("init alarm config failed, metric name: %s", p_metric->m_metric_name);
            g_hash_table_remove(p_metric->m_alarm, p_arg);
            continue;
        }

    }

    DEBUG_LOG("=== SERVER ALARM CONFIG END ===");

    return 0;
}



int init_mysql_node(c_node * p_node, Cmessage * c_out)
{
    db_p_get_node_alarm_config_out * p_out = P_OUT;


    DEBUG_LOG("=== INIT MYSQL ALARM CONFIG ===");

    p_node->m_project_id = p_out->project_id;
    STRNCPY(p_node->m_project_name, p_out->project_name, PROJECT_NAME_LEN);


    p_node->m_mysql_alarm_way = p_out->node_contact.alarm_way;

    if (!config_get_intval("use_default_contact_for_node", 0))
    {
        STRNCPY_LEN(p_node->m_mysql_mobile, &(p_out->node_contact.alarm_contact), mobile_list, CONTACT_LIST_LEN);
        STRNCPY_LEN(p_node->m_mysql_email, &(p_out->node_contact.alarm_contact), email_list, CONTACT_LIST_LEN);
        STRNCPY_LEN(p_node->m_mysql_rtx, &(p_out->node_contact.alarm_contact), rtx_list, CONTACT_LIST_LEN);

    }

    DEBUG_LOG("\tnode[%u: %s], project: [%u: %s]", 
            p_node->m_node_id, long2ip(p_node->m_node_ip), 
            p_node->m_project_id, p_node->m_project_name);




    DEBUG_LOG("\tmobile contact: %s", p_node->m_mysql_mobile);
    DEBUG_LOG("\temail contact: %s", p_node->m_mysql_email);
    DEBUG_LOG("\trtx contact: %s", p_node->m_mysql_rtx);


    vector_for_each(p_out->alarm_metric_list, it)
    {
        alarm_metric_t * p_alarm_config = &(*it);
        uint32_t metric_id = p_alarm_config->alarm_metric.id;
        c_metric * p_metric = p_node->m_metric->get_object(metric_id);
        if (NULL == p_metric)
        {
            p_metric = new c_metric(p_node);
            p_metric->m_metric_id = metric_id;
            STRNCPY_LEN(p_metric->m_metric_name, &(p_alarm_config->alarm_metric), name, MAX_METRIC_NAME_LEN);
            p_metric->m_metric_class = ITL_TYPE_MYSQL;
            p_node->m_metric->insert_object(p_metric->m_metric_id, p_metric);
        }


        char * p_arg = p_alarm_config->alarm_arg.name;


        DEBUG_LOG("\tmetric id: %u, name: %s, arg: %s", 
                p_metric->m_metric_id, 
                p_metric->m_metric_name, 
                p_arg);



        c_metric_alarm * p_alarm = find_metric_alarm(p_metric->m_alarm, p_arg);
        if (NULL != p_alarm)
        {
            continue;
        }

        p_alarm = alloc_metric_alarm(p_metric, p_arg);
        if (NULL == p_alarm)
        {
            continue;
        }


        if (0 != init_metric_alarm(p_alarm, p_alarm_config))
        {
            ERROR_LOG("init alarm config failed, metric name: %s", p_metric->m_metric_name);
            g_hash_table_remove(p_metric->m_alarm, p_arg);
            continue;
        }

    }

    DEBUG_LOG("=== MYSQL ALARM CONFIG END ===");

    return 0;
}
