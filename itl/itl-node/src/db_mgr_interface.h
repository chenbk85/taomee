/** 
 * ========================================================================
 * @file db_mgr_interface.h
 * @brief 数据库管理模块
 * @author tonyliu
 * @version 1.0.0
 * @date 2012-10-17
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_DB_MGR_INTERFACE_H_20121017
#define H_DB_MGR_INTERFACE_H_20121017


#include <stdint.h>
#include "db_mgr/instance.h"


enum
{
    db_mgr_p_get_db_base_info_cmd = 0x1001,
    db_mgr_p_get_privilege_info_cmd,

    db_mgr_p_create_db_user_cmd = 0x1011,
    db_mgr_p_delete_db_user_cmd,
    db_mgr_p_check_db_user_cmd,
    db_mgr_p_get_db_user_cmd,
    db_mgr_p_check_user_password_cmd,

    db_mgr_p_add_user_privilege_cmd = 0x1021,
    db_mgr_p_update_user_privilege_cmd = 0x1022,
    db_mgr_p_show_grants_cmd = 0x1024,
    
    db_mgr_p_exec_sql_cmd = 0x1030,
};


int dispatch_db_mgr(int fd, const char * buf, uint32_t len);


#endif

