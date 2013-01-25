/** 
 * ========================================================================
 * @file db_mgr.cpp
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-11-02
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "proto.h"
#include "db.h"
#include "db_mgr.h"
#include "proxy.h"

struct db_mgr_common_t
{
    char veri_code[32];     /**<命令验证码*/
    uint32_t timestamp;     /**<命令发送时的时间戳*/
    uint32_t serial_no1;    /**<序列号1：与序列号2联合唯一区分DB命令*/
    uint32_t serial_no2;    /**<*/
    char body[];
} __attribute__((packed));


using namespace taomee;

int dispatch_db_mgr_to_node(int fd, const char * buf, uint32_t len)
{
    node_proto_t * pkg = (node_proto_t *)(buf);
    db_mgr_common_t * p_common = (db_mgr_common_t *)(pkg->body);

    int index = 0;
    uint32_t node_ip = 0;
    unpack(p_common->body, node_ip, index);

    DEBUG_LOG("dispatch mysql mgr to node: %s", long2ip(node_ip));

    c_node * p_node = find_node_by_ip(node_ip);
    if (NULL != p_node)
    {
        return dispatch_to_node(fd, p_node, buf, len);
    }
    else
    {
        pkg->ret = 1;


        switch (pkg->cmd)
        {
            case 0x1030:
                {
                    // 跳过port/dba_user/dba_pass，定位到value_id
                    index += sizeof(uint16_t) + 64 * 2;

                    uint32_t value_id = 0;
                    unpack_h(p_common->body, value_id, index);

                    uint32_t sql_len = 0;
                    unpack_h(p_common->body, sql_len, index);

                    char * p_sql = p_common->body + index;
                    p_sql[sql_len] = 0;

                    char err[128];
                    snprintf(err, sizeof(err), "NODE is not installed on %s", long2ip(node_ip));
                    db_store_db_mgr_result(p_common->serial_no1, value_id, 1, pkg->ret, p_sql, err);

                }
        }
        net_send_cli(fd, buf, sizeof(node_proto_t) + sizeof(db_mgr_common_t));
        net_close_cli(fd);
        return 0;
    }
}
