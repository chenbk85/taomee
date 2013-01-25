/** 
 * ========================================================================
 * @file rrd.cpp
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


#include "proto.h"
#include "rrd.h"
#include "node.h"


using namespace taomee;

c_server g_rrd_server;
c_server * g_rrd = &g_rrd_server;

int init_connect_to_rrd()
{
    g_rrd->set_auto_reconnect(CONNECT_TO_SERVER_INTERVAL);
    g_rrd->connect(config_get_strval("rrd_ip", NULL), config_get_intval("rrd_port", 0));

    return 0;
}

int send_metric_to_rrd(c_metric_arg * p_arg)
{

    int index = sizeof(rrd_proto_t);

    c_metric * p_metric = p_arg->m_metric;
    c_node * p_node = p_metric->m_node;

    pack(g_pkg_buf, p_node->m_server_tag, SERVER_TAG_LEN, index);

    pack_metric_to_rrd(g_pkg_buf, p_arg, index);


    init_rrd_proto_header(g_pkg_buf, index, 0, rrd_p_recv_data_cmd, 0, p_node->m_node_id);



    return g_rrd->send(g_pkg_buf, index);

}
