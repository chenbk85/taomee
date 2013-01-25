/** 
 * ========================================================================
 * @file db_mgr.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-22
 * Modify $Date: 2012-10-30 11:39:16 +0800 (二, 30 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_DB_MGR_H_2012_10_22
#define H_DB_MGR_H_2012_10_22


#include "itl_common.h"
#include "db_mgr_common.h"




// 检查common段的合法性
bool is_legal_common_info(const db_mgr_proto_t * pkg);

// ip转换
char * db_mgr_long2ip(uint32_t ip_num, char * ip_str);
uint32_t db_mgr_ip2long(const char * ip);

// 生成应答包中的common段
int gen_common_info(db_mgr_proto_t * p_head);

char * decode(const void * p_code, void * p_buf, uint32_t len);

// 断开连上的mysql连接
int disconnect_to_mysql();



/** 查询库/表/字段信息 */
int db_mgr_get_db_base_info(int fd, const char *buf, uint32_t len);

/** 查询全局/库/表/字段的权限信息 */
int db_mgr_get_privilege_info(int fd, const char *buf, uint32_t len);

/** 创建数据库账户 */
int db_mgr_create_db_user(int fd, const char *buf, uint32_t len);

/** 删除数据库账户 */
int db_mgr_delete_db_user(int fd, const char *buf, uint32_t len);

/** 检测数据库账户是否存在 */
int db_mgr_check_db_user(int fd, const char *buf, uint32_t len);

/** 获取数据库所有账户名 */
int db_mgr_get_db_user(int fd, const char *buf, uint32_t len);

/** 检测数据库账户密码是否正确 */
int db_mgr_check_user_password(int fd, const char *buf, uint32_t len);

/** 给用户加权限 */
int db_mgr_add_user_privilege(int fd, const char * buf, uint32_t len);

/** 修改数据库账户权限 */
int db_mgr_update_user_privilege(int fd, const char *buf, uint32_t len);

/** show grants for xxx@xxx */
int db_mgr_show_grants(int fd, const char * buf, uint32_t len);

/** 执行sql语句 */
int db_mgr_exec_sql(int fd, const char * buf, uint32_t len);

#endif
