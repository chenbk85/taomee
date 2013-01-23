/** 
 * ========================================================================
 * @file server.hpp
 * @brief 
 * @version 1.0
 * @date 2012-04-24
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */



#ifndef H_SERVER_H_2012_04_24
#define H_SERVER_H_2012_04_24


#include <string.h>
#include <libtaomee++/conf_parser/xmlparser.hpp>
extern "C"
{
#include <libtaomee/tlog/tlog.h>
}
#include "container.hpp"
#include "singleton.hpp"

struct server_data_t
{
    server_data_t()
    {
        server_id = 0;
        memset(server_ip, 0, sizeof(server_ip));
        server_port = 0;
        new_flag = 0;
    }

    
    uint32_t server_id;
    char server_ip[16];
    uint16_t server_port;
    uint16_t new_flag;
};


class c_server_data_mgr : public c_object_container<uint32_t, server_data_t>
{
    public:


};


typedef singleton_default<c_server_data_mgr> server_data_mgr;


int init_server_data(xmlNodePtr root);

#endif
