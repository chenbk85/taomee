/** 
 * ========================================================================
 * @file instance.h
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-11-15
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_INSTANCE_H_2012_11_15
#define H_INSTANCE_H_2012_11_15

#include "itl_common.h"


class c_mysql_instance
{
    public:

        uint32_t m_port;
        char m_sock[SOCK_LEN];
};


#define instance_mgr c_single_container(uint32_t, c_mysql_instance)




int init_mysql_instance();

int init_mysql_instance(Cmessage * c_out);

int fini_mysql_instance();

int refresh_mysql_instance();

const c_mysql_instance * find_mysql_instance(uint32_t port);

#endif
