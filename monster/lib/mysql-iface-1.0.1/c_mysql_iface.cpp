/**
 * =====================================================================================
 *       @file  c_mysql_iface.cpp
 *      @brief  Mysql 数据库接口
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

#include <string.h>
#include <iostream>
using namespace std;

#include "c_mysql_iface.h"

/**
 * @brief  构造函数
 * @param  无
 * @return 无
 */
c_mysql_iface::c_mysql_iface()
    : m_hdb(NULL), m_res(NULL), m_row(NULL), m_port(0), m_errno(0)
{
    strcpy(m_errstr, "Operation success.");
}

/**
 * @brief  析构函数
 * @param  无
 * @return 无
 */
c_mysql_iface::~c_mysql_iface()
{
    uninit();
}

/**
@brief 初始化对象属性参数

@param host 数据库主机
@param port 数据库端口
@param db_name 数据库里的库名
@param user 数据库账户名
@param passwd 数据库密码
@param charset 本次链接数据库使用的字符集

@return 成功返回0， 失败返回－1
*/
int c_mysql_iface::init(const char* host, unsigned short port, const char* db_name, const char* user, const char* passwd, const char* charset)
{
    if (m_hdb != NULL) {
        uninit();
    }

    m_hdb = mysql_init(NULL);
    if (m_hdb == NULL) {
        set_err(MYSQL_IFACE_E_MYSQL_API, "Mysql_init error: %s", mysql_error(m_hdb));
        return -1;
    }

    my_bool is_auto_reconnect = 1;
    if (mysql_options(m_hdb, MYSQL_OPT_RECONNECT, &is_auto_reconnect)) {
        set_err(MYSQL_IFACE_E_MYSQL_API, "Set mysql_options reconnect error: %s.", mysql_error(m_hdb));
        return -1;
    }

    if (mysql_real_connect(m_hdb, host, user, passwd, db_name, port, NULL, CLIENT_INTERACTIVE) == NULL) {
        set_err(MYSQL_IFACE_E_MYSQL_API, "Mysql_real_connect to %s(%u) on %s with user %s(%s) error: %s.", host, port, db_name, user, passwd, mysql_error(m_hdb));
        return -1;
    }

    if (mysql_set_character_set(m_hdb, charset)) {
        set_err(MYSQL_IFACE_E_MYSQL_API, "Mysql_set_character_set to %s error: %s.", charset, mysql_error(m_hdb));
        return -1;
    }

    strcpy(m_host, host);
    m_port = port;
    return 0;
}

/**
* @brief 反初始化
* @return 0
*/
int c_mysql_iface::uninit()
{
    if (m_res != NULL) {
        mysql_free_result(m_res);
        m_res = NULL;
    }

    if (m_hdb != NULL) {
        mysql_close(m_hdb);
        m_hdb = NULL;
    }

    return 0;
}

/**
 *@brief 释放本对象占用的资源
 *@return 0
*/
int c_mysql_iface::release()
{
    delete this;
    return 0;
}

/**
 *@brief 获取数据库链接

 *@return MYSQL指针
*/
MYSQL* c_mysql_iface::get_conn()
{
    if (m_hdb == NULL) {
        set_err(MYSQL_IFACE_E_NOT_INITED, "Db uninited while get connection.");
        return NULL;
    }

    /// ping
    if (mysql_ping(m_hdb)) {
        set_err(MYSQL_IFACE_E_MYSQL_API, "mysql_ping error: %s", mysql_error(m_hdb));
        return NULL;
    }

    return m_hdb;
}

/**
 *@brief 从数据库内查询第一行数据, 建议只使用SELECT语句，传入非SELECT语句是不安全的。

 *@param row
 *@param sql_fmt

 *@return */
int c_mysql_iface::select_first_row(MYSQL_ROW* row, const char* sql_fmt, ...)
{
    /// 检查参数
    if (sql_fmt == NULL) {
        set_err(MYSQL_IFACE_E_PARAMETER, "NULL sql fmt str(%p).", sql_fmt);
        return -1;
    }

    /// 得到数据库的连接
    MYSQL* db = get_conn();
    if (db == NULL) {
        return -1;
    }

    /// 构造 sql 语句
    va_list argptr;
    va_start(argptr, sql_fmt);
    vsprintf(m_sql_buf, sql_fmt, argptr);
    va_end(argptr);

    if (mysql_query(db, m_sql_buf) != 0) {
        set_err(MYSQL_IFACE_E_MYSQL_API, "Mysql query error: %s\n\tSQL: %s", mysql_error(db), m_sql_buf);
        return -1;
    }

    /// 清空上次查询的 res 指针
    if (m_res != NULL) {
        mysql_free_result(m_res);
        m_res = NULL;
    }

    m_res = mysql_store_result(db);
    if (m_res == NULL) {
        set_err(MYSQL_IFACE_E_MYSQL_API, "mysql_store_result error: %s\n\tSQL: %s", mysql_error(db), m_sql_buf);
        return -1;
    }

    int row_cnt = mysql_num_rows(m_res);
    if (row != NULL) {
        *row = NULL;

        if (row_cnt != 0) {
            *row = select_next_row();
        }
    }

    return row_cnt;
}

/**
 *@brief 获取SELECT结果的下一行数据

 *@param check_field 是否检查每个字段的指针是否为NULL

 *@return mysql对象MYSQL_ROW.
 */
MYSQL_ROW c_mysql_iface::select_next_row(bool check_field)
{
    if (m_res == NULL) {
        return NULL;
    }

    m_row = mysql_fetch_row(m_res);
    if ((m_row != NULL) && check_field) {
        unsigned int field_cnt = mysql_num_fields(m_res);
        for (unsigned int i=0; i<field_cnt; i++) {
            if (m_row[i] == NULL) {
                set_err(MYSQL_IFACE_E_NULL_FIELD, "NULL field at position %u.", i);
                return NULL;
            }
        }
    }

    if (m_row == NULL) {
        /// 获取到最后一行时释放资源
        mysql_free_result(m_res);
        m_res = NULL;
    }

    return m_row;
}

/**
 *@brief 执行SQL语句, 参数格式同printf()

 *@param sql_fmt  字符串格式
 *@param ...  变参

 *@return 失败返回－1， 成功返回SQL(UPDATE,INSERT,DELETE)语句影响的行数,或SELECT到数据的行数。
 */
int c_mysql_iface::execsql(const char* sql_fmt, ...)
{
    /// 检查参数
    if (sql_fmt == NULL) {
        set_err(MYSQL_IFACE_E_PARAMETER, "NULL sql fmt str.");
        return -1;
    }

    /// 得到数据库的连接
    MYSQL* db = get_conn();
    if (db == NULL) {
        return -1;
    }

    /// 构造 sql 语句
    va_list argptr;
    va_start(argptr, sql_fmt);
    vsprintf(m_sql_buf, sql_fmt, argptr);
    va_end(argptr);

    if (mysql_query(db, m_sql_buf) != 0) {
        set_err(MYSQL_IFACE_E_MYSQL_API, "Mysql query error: %s\n\tSQL: %s", mysql_error(db), m_sql_buf);
        return -1;
    }

    return mysql_affected_rows(db);
}

/**
 *@brief 设置本对象实例的内部错误信息

 *@param errno 错误码
 *@param msg 错误描述
 *@param ... 变参

 *@return 空
 */
void c_mysql_iface::set_err(int errno, const char* msg, ...)
{
    m_errno = errno;

    va_list argptr;
    va_start(argptr, msg);
    // original: vsprintf(m_errstr, msg, argptr);
    vsnprintf(m_errstr, MYSQL_MAX_ERROR_BUF_LEN, msg, argptr);
    va_end(argptr);
}

/**
 *@brief 获取本对象实例最近一次错误号。

 *@return 错误号
*/
int c_mysql_iface::get_last_errno() const
{
    return m_errno;
}

/**
 *@brief 获取本对象实例最近一次错误号。

 *@return 错误号
 */
const char* c_mysql_iface::get_last_errstr() const
{
    return m_errstr;
}

/**
 *@brief 创建i_mysql_iface对象实例

 *@param pp_instance 二级指针，函数成功返回后，该指针指向创建的实例的地址

 *@return 成功返回0， 失败返回－1.

 *@note 注意该函数分配的内存由调用者维护；
 */
int create_mysql_iface_instance(i_mysql_iface** pp_instance)
{
    if (pp_instance == NULL) {
        return -1;
    }

    c_mysql_iface *p_instance = new(nothrow) c_mysql_iface;
    if (p_instance == NULL) {
        return -1;
    }

    *pp_instance = dynamic_cast<i_mysql_iface*>(p_instance);
    return 0;
}
