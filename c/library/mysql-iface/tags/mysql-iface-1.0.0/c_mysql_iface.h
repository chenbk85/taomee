/**
 * =====================================================================================
 *       @file  c_mysql_iface.h
 *      @brief  为了更方便地使用 mysql 而设立的封装类
 *
 *
 *   @internal
 *     Created  01/31/2010 12:41:06 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  jasonwang (王国栋) jasonwang@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef __MYSQL_IFACE_H__
#define __MYSQL_IFACE_H__

#include <stdarg.h>
#include <stdio.h>

#include "./i_mysql_iface.h"

#define MYSQL_MAX_HOST_BUF_LEN 128
#define MYSQL_MAX_SQL_BUF_LEN (1024 * 1024)

#define MYSQL_IFACE_E_SUCCESS 0
#define MYSQL_IFACE_E_MYSQL_API 1
#define MYSQL_IFACE_E_NOT_INITED 2
#define MYSQL_IFACE_E_PARAMETER 3
#define MYSQL_IFACE_E_NULL_FIELD 4
#define MYSQL_IFACE_E_ILLEGAL_CALL 5


/**
 * @class c_mysql_iface
 * @brief mysql接口的实现类
 */
class c_mysql_iface : public i_mysql_iface
{
public:
    c_mysql_iface();
    ~c_mysql_iface();

    int init(const char* host, unsigned short port, const char* db_name, const char* user, const char* passwd, const char* charset);
    int uninit();

    int release();

    MYSQL* get_conn();

    int select_first_row(MYSQL_ROW* row, const char* sql_fmt, ...);
    MYSQL_ROW select_next_row(bool check_field = false);
    int execsql(const char* sql_fmt, ...);

    int get_last_errno() const;
    const char* get_last_errstr() const;

private:
    void set_err(int errno, const char* msg, ...);

    MYSQL* m_hdb; /**< @brief 对象内部维护的MYSQL连接指针 */
    MYSQL_RES* m_res; /**< @brief  维护select_first_row和select_next_row查询结果的内部资源*/
    MYSQL_ROW m_row;  /**< @brief  维护select_first_row和select_next_row查询结果的内部资源*/

    char m_host[MYSQL_MAX_HOST_BUF_LEN]; /**< @brief 数据库主机地址 */
    unsigned short m_port; /**< @brief 数据库主机端口 */

    char m_sql_buf[MYSQL_MAX_SQL_BUF_LEN]; /**< @brief  接保存接口串入的SQL语句的缓存*/

    int m_errno; /**< @brief 保存本对象错误号 */
    char m_errstr[1024]; /**< @brief 保存本对象的错误描述 */
};

#endif // !__MYSQL_IFACE_H__

