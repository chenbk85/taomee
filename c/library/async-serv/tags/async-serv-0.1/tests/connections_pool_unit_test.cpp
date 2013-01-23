/**
 * @file connections_pool_unit_test.cpp
 * @brief 连接池单元测试
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-17
 */
#include <stdint.h>

#include <iostream>

#include "connections_pool.hpp"
#include "vpipe_sockpair.h"
#include "connection.hpp"


int main(int argc, char** argv)
{
    connections_pool<connection<vpipe_sockpair> > *p_pool = connections_pool<connection<vpipe_sockpair> >::instance();

    int i = 10;
    while (i-- > 0) {
        connection<vpipe_sockpair>* p_conn = p_pool->get_new_connection();
        uint32_t conn_id = p_conn->get_connection_id();
        std::cout << "conn_id " << conn_id << std::endl;

        p_conn = p_pool->search_connection(conn_id);

        p_pool->free_connection(p_conn);
    }

    return 0;

}
