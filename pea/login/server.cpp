/** 
 * ========================================================================
 * @file server.cpp
 * @brief 
 * @version 1.0
 * @date 2012-04-24
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include <stdio.h>
#include "server.hpp"

using namespace taomee;




int init_server_data(xmlNodePtr root)
{
    xmlNodePtr node_1 = root->xmlChildrenNode;
    while (node_1)
    {
        if (0 != xmlStrcmp(node_1->name, BAD_CAST("server")))
        {
            node_1 = node_1->next;
            continue;
        }

        server_data_t * p_data = new server_data_t();
        if (NULL == p_data)
        {
            return -1;
        }

        get_xml_prop_def(p_data->server_id, node_1, "id", 0);
        get_xml_prop_raw_str_def(p_data->server_ip, node_1, "ip", "");
        get_xml_prop_def(p_data->server_port, node_1, "port", 0);
        get_xml_prop_def(p_data->new_flag, node_1, "new_flag", 0);

        TRACE_TLOG("[server] id: %u, ip: %s, port: %u, new_flag: %u", 
                p_data->server_id,
                p_data->server_ip,
                p_data->server_port,
                p_data->new_flag);


        server_data_mgr::instance().insert_object(p_data->server_id, p_data);

        node_1 = node_1->next;


    }


    return 0;
}
