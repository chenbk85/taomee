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



c_server g_db_server;
c_server * g_db = &g_db_server;

int init_connect_to_db()
{
    g_db->set_auto_reconnect(CONNECT_TO_SERVER_INTERVAL);

    g_db->connect(config_get_strval("db_ip", NULL), config_get_intval("db_port", 0));

    return 0;
}



int send_to_db(c_node * p_node, uint16_t cmd, Cmessage * p_out)
{

    db_proto_t head;

    uint32_t id = p_node ? p_node->m_node_id : 0;
    uint32_t seq = p_node ? p_node->m_node_ip : 0;

    init_db_proto_header(&head, sizeof(db_proto_t), seq, cmd, 0, id);
    

    return g_db->send(&head, p_out);
}



int send_metric_to_db(c_metric_arg * p_arg)
{
    int index = sizeof(db_proto_t);

    c_metric * p_metric = p_arg->m_metric;
    c_node * p_node = p_metric->m_node;

    pack_node_to_db(g_pkg_buf, p_node, index);

    pack_metric_to_db(g_pkg_buf, p_arg, index);

    init_db_proto_header(g_pkg_buf, index, p_node->m_node_ip, db_p_store_collect_data_cmd, 0, p_node->m_node_id);

    return g_db->send(g_pkg_buf, index);
}



// int dispatch_db(int fd, const char * buf, uint32_t len)
// {
    // const db_proto_t * pkg = reinterpret_cast<const db_proto_t *>(buf);

    // uint16_t cmd = pkg->cmd;
    // uint32_t uid = pkg->id;
    // uint32_t seq = pkg->seq;
    // uint32_t node_ip = seq;


    // TRACE_LOG("dispatch db[%u] sender=%u, fd=%u, seq=%u, len=%u",
            // cmd, uid, fd, seq, len);

    // c_node * p_node = find_node_by_ip(node_ip);
    // if (NULL == p_node || p_node->m_node_id != uid)
    // {
        // ERROR_LOG("miss node, ip: %s, id: %u, pkg->id: %u",
                // long2ip(node_ip), 
                // p_node ? p_node->m_node_id : 0,
                // uid);
        // return -1;
    // }
    // const cmd_proto_t * p_cmd = find_db_cmd(cmd);
    // if (NULL == p_cmd)
    // {
        // ERROR_LOG("cmdid not existed: %u", cmd);
        // return 0;
    // }


    // uint32_t body_len = len - sizeof(db_proto_t);

    // bool read_ret = p_cmd->p_out->read_from_buf_ex(pkg->body, body_len);
    // if (!read_ret) 
    // {
        // ERROR_LOG("read_from_buf_ex error cmd=%u, u=%u", cmd, uid);
        // return -1;
    // }

    // int cmd_ret = p_cmd->func(p_node, p_cmd->p_in, p_cmd->p_out);
    // if (DB_SUCC == cmd_ret)
    // {

    // }

    // return 0;
// }




int db_p_store_collect_data_cb(DEFAULT_ARG)
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
    init_db_proto_header(&head, sizeof(db_proto_t), p_node->m_node_ip, cmd, 0, p_node->m_node_id);

    return g_db->send(&head, &in);
}

int db_p_store_host_status_cb(DEFAULT_ARG)
{
    return 0;
}
