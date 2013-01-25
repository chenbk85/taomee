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

extern "C"
{
#include <libtaomee/inet/inet_utils.h>
}


#include "db.h"
#include "proto.h"
#include "collect_interface.h"
#include "control.h"
#include "db_mgr_interface.h"


using namespace taomee;

c_server g_db_server;

c_server * g_db = &g_db_server;

int init_connect_to_db()
{

    // 一开始db要自动重连的
    // 获取到配置之后，才关闭重连，关闭连接
    g_db->set_auto_reconnect(CONNECT_TO_SERVER_INTERVAL);
    g_db->connect(config_get_strval("db_ip", NULL), config_get_intval("db_port", 0));

    return 0;
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

    // c_node * p_node = find_node_by_ip(node_ip);
    // if (NULL == p_node || p_node->m_node_id != uid)
    // {
    // ERROR_LOG("miss node, ip: %s, id: %u, pkg->id: %u",
    // long2ip(node_ip), 
    // p_node ? p_node->m_node_id : 0,
    // uid);
    // return -1;
    // }

    if (DB_SUCC != ret)
    {
        ERROR_LOG("db ret: %u, cmd: %u, id: %u", ret, cmd, uid);
        return -1;
    }


    const cmd_proto_t * p_cmd = find_db_cmd(cmd);
    if (NULL == p_cmd)
    {
        ERROR_LOG("cmdid not existed: %u", cmd);
        return -1;
    }


    uint32_t body_len = len - sizeof(db_proto_t);

    p_cmd->p_out->init();
    bool read_ret = p_cmd->p_out->read_from_buf_ex(pkg->body, body_len);
    if (!read_ret) 
    {
        ERROR_LOG("read_from_buf_ex error cmd=%u, u=%u", cmd, uid);
        return -1;
    }

    int cmd_ret = p_cmd->func(fd, p_cmd->p_in, p_cmd->p_out);
    if (DB_SUCC == cmd_ret)
    {
        // 完成后关闭db连接
        g_db->disconnect();
    }

    return 0;
}




int db_get_node_head_config(const char * node_ip)
{
    db_p_get_node_head_config_in in;
    STRNCPY(in.node_ip, node_ip, sizeof(in.node_ip));

    db_proto_t head;

    uint32_t id = g_node_id; 
    uint32_t seq = g_node_ip;
    uint32_t cmd = db_p_get_node_head_config_cmd;

    init_db_proto_header(&head, sizeof(db_proto_t), seq, cmd, 0, id);

    return g_db->send(&head, &in);
}



int db_p_get_node_head_config_cb(DEFAULT_ARG)
{
    db_p_get_node_head_config_out * p_out = P_OUT;

    g_node_id = p_out->node_id;
    STRNCPY(g_server_tag, p_out->host_tag, SERVER_TAG_LEN);

    uint16_t head_port = p_out->head_addr.port;
    char * head_ip = p_out->head_addr.ip;



    if (g_head->is_connected())
    {
        // 检查配置中itl_head的ip/port是否改变了
        // 改变了则断开已有的连接并重连
        if (g_head->get_port() != head_port
                || ip2long(g_head->get_ip()) != ip2long(head_ip))
        {
            g_head->disconnect();
            g_head->connect(head_ip, head_port);
        }
    }
    else
    {
        g_head->connect(head_ip, head_port);
    }

    return -1;
}




int db_get_node_control_config(const char * node_ip)
{
    db_p_get_node_control_config_in in;
    STRNCPY(in.node_ip, node_ip, sizeof(in.node_ip));

    db_proto_t head;

    uint32_t id = g_node_id; 
    uint32_t seq = g_node_ip;
    uint32_t cmd = db_p_get_node_control_config_cmd;

    init_db_proto_header(&head, sizeof(db_proto_t), seq, cmd, 0, id);

    return g_db->send(&head, &in);
}



int db_p_get_node_control_config_cb(DEFAULT_ARG)
{
    db_p_get_node_control_config_out * p_out = P_OUT;

    g_node_id = p_out->node_id;

    uint16_t control_port = p_out->control_addr.port;
    char * control_ip = p_out->control_addr.ip;

    DEBUG_LOG("control: %s:%u", control_ip, control_port);


    if (g_control->is_connected())
    {
        // 检查配置中ip/port是否改变了
        // 改变了则断开已有的连接并重连
        if (g_control->get_port() != control_port
                || ip2long(g_control->get_ip()) != ip2long(control_ip))
        {
            g_control->disconnect();
            g_control->connect(control_ip, control_port);
        }
    }
    else
    {
        g_control->connect(control_ip, control_port);
    }

    return -1;
}




int db_get_node_server_config(const char * node_ip)
{
    db_p_get_node_server_config_in in;
    STRNCPY(in.node_ip, node_ip, sizeof(in.node_ip));

    db_proto_t head;

    uint32_t id = g_node_id; 
    uint32_t seq = g_node_ip;
    uint32_t cmd = db_p_get_node_server_config_cmd;

    init_db_proto_header(&head, sizeof(db_proto_t), seq, cmd, 0, id);

    return g_db->send(&head, &in);
}


int db_p_get_node_server_config_cb(DEFAULT_ARG)
{
    db_p_get_node_server_config_out * p_out = P_OUT;

    // group info为空则表示没有采集配置
    if (p_out->group_info.empty())
    {
        // 沿用原有的配置
        return 0;

    }





    fini_metric_timer();
    fini_metric_group();
    fini_collect();

    init_server_collect(c_out);
    init_server_metric_group(c_out);



    db_get_node_mysql_config(g_node_ip_str);

    // 不能关闭连接
    return -1;
}


int db_get_node_mysql_config(const char * node_ip)
{
    db_p_get_node_mysql_config_in in;
    STRNCPY(in.node_ip, node_ip, sizeof(in.node_ip));

    db_proto_t head;
    uint32_t id = g_node_id; 
    uint32_t seq = g_node_ip;
    uint32_t cmd = db_p_get_node_mysql_config_cmd;

    init_db_proto_header(&head, sizeof(db_proto_t), seq, cmd, 0, id);

    return g_db->send(&head, &in);
}




int db_p_get_node_mysql_config_cb(DEFAULT_ARG)
{
    db_p_get_node_mysql_config_out * p_out = P_OUT;

    init_mysql_collect(c_out);
    init_mysql_metric_group(c_out);


    init_metric_timer();

    g_db->set_auto_reconnect(0);

    return 0;
}

int db_store_db_mgr_result(uint32_t manage_id, uint32_t manage_value_id, uint32_t sql_no, uint32_t status_code, const char * sql, const char * error_desc)
{
    db_p_record_db_mgr_exec_result_in in;
    in.manage_id = manage_id;
    in.manage_value_id = manage_value_id;
    in.sql_no = sql_no;
    in.status_code = status_code;


    snprintf(in.sql_sentence, sizeof(in.sql_sentence), "%s", sql ? sql : "");
    in._sql_sentence_len = 1 + strlen(in.sql_sentence);
    snprintf(in.err_desc, sizeof(in.err_desc), "%s", error_desc ? error_desc : "");
    in._err_desc_len = 1 + strlen(in.err_desc);

    db_proto_t head;
    uint32_t id = g_node_id; 
    uint32_t seq = g_node_ip;
    uint32_t cmd = db_p_record_db_mgr_exec_result_cmd;

    init_db_proto_header(&head, sizeof(db_proto_t), seq, cmd, 0, id);

    return g_db->send(&head, &in);
}


int db_p_record_db_mgr_exec_result_cb(DEFAULT_ARG)
{
    // 不要断开连接
    return -1;
}


int db_get_node_mysql_instance(const char * node_ip)
{
    db_p_get_node_mysql_instance_in in;
    STRNCPY(in.node_ip, node_ip, sizeof(in.node_ip));

    db_proto_t head;
    uint32_t id = g_node_id; 
    uint32_t seq = g_node_ip;
    uint32_t cmd = db_p_get_node_mysql_instance_cmd;

    init_db_proto_header(&head, sizeof(db_proto_t), seq, cmd, 0, id);

    return g_db->send(&head, &in);
}


int db_p_get_node_mysql_instance_cb(DEFAULT_ARG)
{
    db_p_get_node_mysql_instance_out * p_out = P_OUT;

    fini_mysql_instance();
    init_mysql_instance(c_out);
    return -1;
}
