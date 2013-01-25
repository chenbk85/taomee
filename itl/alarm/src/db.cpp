/** 
 * ========================================================================
 * @file db.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-12
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <libtaomee/inet/inet_utils.h>


#include "db.h"
#include "proto.h"
#include "itl_common.h"
#include "alarm.h"
#include "metric_alarm.h"

#define MAX_DATA_LEN (1024) 

using namespace taomee;


c_server g_db_server;
c_server * g_db = &g_db_server;


int init_connect_to_db()
{

    g_db->set_auto_reconnect(CONNECT_TO_SERVER_INTERVAL);
    g_db->connect(config_get_strval("db_ip", NULL), config_get_intval("db_port", 0));

    return 0;
}


int send_to_db(const c_node * p_node, uint16_t cmd, Cmessage * p_out)
{
    if (!g_db->is_connected())
    {
        if (0 != g_db->connect())
        {
            ERROR_LOG("connect to db failed");
            return -1;
        }
    }

    db_proto_t head;

    init_db_proto_header(&head, sizeof(db_proto_t), 0, cmd, 0, p_node->m_node_id);


    return g_db->send(&head, p_out);
}



int dispatch_db(int fd, const char * buf, uint32_t len)
{
    const db_proto_t * pkg = reinterpret_cast<const db_proto_t *>(buf);

    uint16_t cmd = pkg->cmd;
    uint32_t uid = pkg->id;
    uint32_t seq = pkg->seq;
    uint32_t ret = pkg->ret;


    TRACE_LOG("dispatch db[%u] sender=%u, fd=%u, seq=%u, ret =%u, len=%u",
            cmd, uid, fd, seq, ret, len);

    uint32_t node_id = uid;

    c_node * p_node = find_node(node_id);
    if (NULL == p_node)
    {
        return -1;
    }

    if (DB_SUCC != ret)
    {
        ERROR_LOG("db ret: %u, cmd: %u, id: %u", ret, cmd, uid);
        p_node->clear_waitcmd();
        return -1;
    }


    const cmd_proto_t * p_cmd = find_db_cmd(cmd);
    if (NULL == p_cmd)
    {
        ERROR_LOG("cmdid not existed: %u", cmd);
        p_node->clear_waitcmd();
        return 0;
    }


    uint32_t body_len = len - sizeof(db_proto_t);

    p_cmd->p_out->init();
    bool read_ret = p_cmd->p_out->read_from_buf_ex(pkg->body, body_len);
    if (!read_ret) 
    {
        ERROR_LOG("read_from_buf_ex error cmd=%u, u=%u", cmd, uid);
        p_node->clear_waitcmd();
        return -1;
    }

    int cmd_ret = p_cmd->func(p_node, p_cmd->p_in, p_cmd->p_out);
    if (DB_SUCC == cmd_ret)
    {

    }

    return 0;
}


int db_get_node_server_alarm_config(c_node * p_node)
{
    db_p_get_node_alarm_config_in in;

    in.node_id = p_node->m_node_id;
    in.service_type = ITL_TYPE_SERVER;

    int ret = send_to_db(p_node, db_p_get_node_alarm_config_cmd, &in);
    if (-1 == ret)
    {
        p_node->clear_waitcmd();
    }

    return ret;

}



int db_get_node_mysql_alarm_config(c_node * p_node)
{
    db_p_get_node_alarm_config_in in;

    in.node_id = p_node->m_node_id;
    in.service_type = ITL_TYPE_MYSQL;

    int ret = send_to_db(p_node, db_p_get_node_alarm_config_cmd, &in);
    if (-1 == ret)
    {
        p_node->clear_waitcmd();
    }

    return ret;

}

int db_get_node_alarm_config(c_node * p_node)
{

    return db_get_node_server_alarm_config(p_node);
}


int db_p_get_node_alarm_config_cb(DEFAULT_ARG)
{

    db_p_get_node_alarm_config_out * p_out = P_OUT;


    c_node * p_node = p;
    uint32_t service_type = p_out->service_type;
    if (ITL_TYPE_SERVER == service_type)
    {
        if (p_out->alarm_metric_list.empty())
        {
            // 没有告警配置的node，清理掉
            dealloc_node(p_node);
            return 0;
        }

        // 老的配置清理掉
        p->m_metric->uninit();

        init_server_node(p_node, c_out);

        db_get_node_mysql_alarm_config(p_node);
    }
    if (ITL_TYPE_MYSQL == service_type)
    {
        init_mysql_node(p_node, c_out);

        init_check_host_timer(p);
        p->m_inited = true;
        p->clear_waitcmd();
    }
    else
    {

    }

    return 0;
}

int db_store_alarm_event_data(const c_metric_alarm * p_alarm, const char * msg)
{
    c_node * p_node = p_alarm->m_node;
    c_metric * p_metric = p_alarm->m_metric;

    db_p_store_alarm_event_data_in in;

    in.alarm_start = p_alarm->m_alarm_start_time;
    if (p_alarm->m_normal)
    {
        // 恢复了
        in.alarm_end = get_now_tv()->tv_sec;
    }
    else
    {
        in.alarm_end = 0;
    }

    STRNCPY(in.alarm_host, long2ip(p_node->m_node_ip), IP_STRING_LEN);

    in.host_type = p_metric->m_metric_class;
    in.alarm_type = p_alarm->m_alarm_level;

    in.alarm_metric.id = p_metric->m_metric_id;
    in.alarm_metric._name_len = strlen(p_metric->m_metric_name) + 1;
    STRNCPY(in.alarm_metric.name, p_metric->m_metric_name, MAX_METRIC_NAME_LEN);

    in.alarm_arg.id = 0;
    in.alarm_arg._name_len = strlen(p_alarm->m_arg) + 1;
    STRNCPY(in.alarm_arg.name, p_alarm->m_arg, MAX_METRIC_ARG_LEN);

    in.alarm_way = p_alarm->get_crit_way();

    in._alarm_content_len = strlen(msg) + 1;
    STRNCPY(in.alarm_content, msg, in._alarm_content_len);

    return send_to_db(p_node, db_p_store_alarm_event_data_cmd, &in);
}


int db_store_alarm_event_data(c_node * p_node, const char * msg)
{
    db_p_store_alarm_event_data_in in;

    in.alarm_start = p_node->m_alarm_start_time;
    if (p_node->m_host_alive)
    {
        // 恢复了
        in.alarm_end = get_now_tv()->tv_sec;
    }
    else
    {
        in.alarm_end = 0;
    }

    STRNCPY(in.alarm_host, long2ip(p_node->m_node_ip), IP_STRING_LEN);

    in.host_type = ITL_TYPE_SERVER;
    in.alarm_type = ALARM_HOSTDOWN;

    in.alarm_metric.id = 0;
    const char * metric_name = "host_down_metric";
    in.alarm_metric._name_len = strlen(metric_name) + 1;
    STRNCPY(in.alarm_metric.name, metric_name, MAX_METRIC_NAME_LEN);

    in.alarm_arg.id = 0;
    in.alarm_arg._name_len = 0;

    in.alarm_way = p_node->get_server_alarm_way();

    in._alarm_content_len = strlen(msg) + 1;
    STRNCPY(in.alarm_content, msg, in._alarm_content_len);

    return send_to_db(p_node, db_p_store_alarm_event_data_cmd, &in);

}

int db_p_store_alarm_event_data_cb(DEFAULT_ARG)
{
    return 0;
}


//elva  
int db_store_alarm_message_data(const c_metric_alarm * p_alarm, char * msg)
{
    // DEBUG_LOG("Begin to do fun send_db_store_alarm_message");

    c_node * p_node = p_alarm->m_node;
    c_metric * p_metric = p_alarm->m_metric;

    db_p_store_alarm_message_data_in in;
    // host_ip
    STRNCPY(in.host_ip, long2ip(p_node->m_node_ip), IP_STRING_LEN);  
    in.host_type = p_metric->m_metric_class;

    // alarm_metric
    in.alarm_metric._name_len = strlen(p_metric->m_metric_name) + 1;
    in.alarm_metric.id = p_metric->m_metric_id;
    STRNCPY(in.alarm_metric.name, p_metric->m_metric_name, MAX_METRIC_NAME_LEN);

    //alarm_arg
    in.alarm_arg.id = 0;
    in.alarm_arg._name_len = strlen(p_alarm->m_arg) + 1;
    STRNCPY(in.alarm_arg.name, p_alarm->m_arg, in.alarm_arg._name_len);

    in.alarm_way = p_alarm->get_crit_way();
    in.alarm_level = p_alarm->m_alarm_level;

    // DEBUG_LOG("MSG:ip[%s]\tmetric[%s : %d] arg[%s: %d]",
    // in.host_ip, in.alarm_metric.name, in.alarm_metric.id, in.alarm_arg.name, in.alarm_arg.id);
    //contact
    STRNCPY(in.alarm_contact.email_list, p_alarm->m_email_contact, CONTACT_LIST_LEN);
    in.alarm_contact._email_list_len = strlen(p_alarm->m_email_contact)+ 1;

    STRNCPY(in.alarm_contact.mobile_list, p_alarm->m_mobile_contact, CONTACT_LIST_LEN);
    in.alarm_contact._mobile_list_len = strlen(p_alarm->m_mobile_contact) + 1;

    STRNCPY(in.alarm_contact.rtx_list, p_alarm->m_rtx_contact, CONTACT_LIST_LEN);
    in.alarm_contact._rtx_list_len = strlen(p_alarm->m_rtx_contact) + 1;


    // DEBUG_LOG("MSG:contact email[%s]\tmobile[%s]\trtx[%s]",
    // in.alarm_contact.email_list, in.alarm_contact.mobile_list, in.alarm_contact.rtx_list);

    //content 

    // DEBUG_LOG("MSG:alarm_way[%u] \t alarm_level[%u] \talarm content [%s]", 
    // in.alarm_way, in.alarm_level, msg);

    int len = strlen(msg) + 1;
    STRNCPY(in.alarm_content, msg, len);
    in._alarm_content_len = len + 1;

    return send_to_db(p_node, db_p_store_alarm_message_data_cmd, &in);

}


int db_store_alarm_message_data(const c_node * p_node, char * msg)
{
    // DEBUG_LOG("Begin to do fun send_db_store_alarm_message");


    db_p_store_alarm_message_data_in in;
    // host_ip
    STRNCPY(in.host_ip, long2ip(p_node->m_node_ip), IP_STRING_LEN);  
    in.host_type = ITL_TYPE_SERVER;

    // alarm_metric
    const char * metric_name = "host_down_metric";
    in.alarm_metric._name_len = strlen(metric_name) + 1;
    in.alarm_metric.id = 0;
    STRNCPY(in.alarm_metric.name, metric_name, MAX_METRIC_NAME_LEN);

    //alarm_arg
    in.alarm_arg.id = 0;
    in.alarm_arg._name_len = 0;

    in.alarm_way = p_node->get_server_alarm_way();
    in.alarm_level = ALARM_HOSTDOWN;

    //contact
    STRNCPY(in.alarm_contact.email_list, p_node->m_server_email, CONTACT_LIST_LEN);
    in.alarm_contact._email_list_len = strlen(p_node->m_server_email)+ 1;

    STRNCPY(in.alarm_contact.mobile_list, p_node->m_server_mobile, CONTACT_LIST_LEN);
    in.alarm_contact._mobile_list_len = strlen(p_node->m_server_mobile) + 1;

    STRNCPY(in.alarm_contact.rtx_list, p_node->m_server_rtx, CONTACT_LIST_LEN);
    in.alarm_contact._rtx_list_len = strlen(p_node->m_server_rtx) + 1;


    // DEBUG_LOG("MSG:contact email[%s]\tmobile[%s]\trtx[%s]",
    // in.alarm_contact.email_list, in.alarm_contact.mobile_list, in.alarm_contact.rtx_list);

    //content 

    // DEBUG_LOG("MSG:alarm_way[%u] \t alarm_level[%u] \talarm content [%s]", 
    // in.alarm_way, in.alarm_level, msg);

    int len = strlen(msg) + 1;
    STRNCPY(in.alarm_content, msg, len);
    in._alarm_content_len = len + 1;

    return send_to_db(p_node, db_p_store_alarm_message_data_cmd, &in);

}

int db_p_store_alarm_message_data_cb(DEFAULT_ARG)
{
    return 0;
}



int db_store_host_status(c_node * p_node, uint32_t status)
{
    db_p_store_host_status_in in;
    in.node_id = p_node->m_node_id;
    in.node_type = NODE_TYPE_HOST;
    in.node_status = status;

    uint16_t cmd = db_p_store_host_status_cmd;

    db_proto_t head;
    init_db_proto_header(&head, sizeof(db_proto_t), 0, cmd, 0, p_node->m_node_id);

    return g_db->send(&head, &in);
}


int db_p_store_host_status_cb(DEFAULT_ARG)
{
    return 0;
}
