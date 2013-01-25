/** 
 * ========================================================================
 * @file db_mgr_interface.cpp
 * @brief 
 * @author tonyliu
 * @version 1.0.0
 * @date 2012-10-17
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "db_mgr/db_mgr.h"
#include "db_mgr_interface.h"
#include "proto.h"


int dispatch_db_mgr(int fd, const char * buf, uint32_t len)
{
    const db_mgr_proto_t * pkg = reinterpret_cast< const db_mgr_proto_t * >(buf);

    uint16_t cmd = pkg->cmd;
    if (node_p_notify_mysql_instance_change_cmd == cmd)
    {
        return node_p_notify_mysql_instance_change(fd, NULL, NULL);
    }

    if (!is_legal_common_info(pkg))
    {
        // 校验不过，非法数据
        return -1;
    }

    DEBUG_LOG("cmd = x%x", cmd);

    switch (cmd)
    {
        case db_mgr_p_get_db_base_info_cmd:
            db_mgr_get_db_base_info(fd, buf, len);
            break;
        case db_mgr_p_get_privilege_info_cmd:
            db_mgr_get_privilege_info(fd, buf, len);
            break;
        case db_mgr_p_create_db_user_cmd:
            db_mgr_create_db_user(fd, buf, len);
            break;
        case db_mgr_p_delete_db_user_cmd:
            db_mgr_delete_db_user(fd, buf, len);
            break;
        case db_mgr_p_check_db_user_cmd:
            db_mgr_check_db_user(fd, buf, len);
            break;
        case db_mgr_p_get_db_user_cmd:
            db_mgr_get_db_user(fd, buf, len);
            break;
        case db_mgr_p_check_user_password_cmd:
            db_mgr_check_user_password(fd, buf, len);
            break;
        case db_mgr_p_add_user_privilege_cmd:
        case db_mgr_p_update_user_privilege_cmd:
            db_mgr_update_user_privilege(fd, buf, len);
            break;
        case db_mgr_p_show_grants_cmd:
            db_mgr_show_grants(fd, buf, len);
            break;
        case db_mgr_p_exec_sql_cmd:
            db_mgr_exec_sql(fd, buf, len);
            break;


        default:
            return -1;
    }


    disconnect_to_mysql();
    return 0;
}



