/**
 * =====================================================================================
 *       @file  i_mysql_iface.h
 *      @brief  Mysql 数据库接口
 *
 *
 *   @internal
 *     Created  03/24/2010 05:12:57 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  jasonwang (王国栋), jasonwang@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef __I_MYSQL_IFACE_H__
#define __I_MYSQL_IFACE_H__

#include <mysql/mysql.h>


/**
 * @struct i_mysql_iface
 * @brief mysql接口
 */
struct i_mysql_iface
{
public:
    /**
     * @brief 初始化接口
     *
     * @param host 数据库主机名称
     * @param port 数据库主机端口
     * @param db_name 数据库名称
     * @param user 数据库用户名
     * @param passwd 数据库密码
     * @param charset 连接数据库时试用的字符集
     *
     * @return 0 成功; <0 失败
     */
    virtual int init(const char* host, unsigned short port, const char* db_name, const char* user, const char* passwd, const char* charset) = 0;

    /**
     * @brief 反初始化接口
     *
     * @return 0 成功; <0 失败
     */
    virtual int uninit() = 0;

    /**
     * @brief 清除资源
     *
     * @return 0 成功; <0 失败
     */
    virtual int release() = 0;

    /**
     * @brief 获取 Mysql 指针
     *
     * @return !=NULL 成功; =NULL 失败
     */
    virtual MYSQL* get_conn() = 0;

    /**
     * @brief 执行指定的 SELECT 等查询语句，并返回第一行的结果
     *
     * @param row 用于保存返回的第一行
     * @param sql_fmt SQL 语句的可变参格式字符串
     *
     * @note 这个函数用于执行需要获取数据的 SQL 语句，类似 UPDATE 及 DELETE 等更新语句请试用 execsql 函数
     *
     * @return >=0 满足 SQL 查询的记录条数; <0 失败
     */
    virtual int select_first_row(MYSQL_ROW* row, const char* sql_fmt, ...) = 0;

    /**
     * @brief 用于执行完 select_first_row 以后，获取下一行数据
     *
     * @param check_field 是否检查各个字段是否为 NULL，若 = true，则只要发现一个字段为 NULL，就会返回 NULL
     *
     * @return !=NULL 行数据; =NULL 没有可以获取的行
     */
    virtual MYSQL_ROW select_next_row(bool check_field) = 0;

    /**
     * @brief 执行指定的 UPDATE 或 DELETE 等更新语句，并返回影响的行数
     *
     * @param sql_fmt SQL 语句的可变参格式字符串
     *
     * @note 这个函数用于执行UPDATE 及 DELETE 等更新语句，类似 SELECT 等需要获取数据的情况，请试用 select_first_row 函数
     *
     * @return >=0 更新语句影响的行数; <0 失败
     */
    virtual int execsql(const char* sql_fmt, ...) = 0;

    /**
     * @brief 获取最后一次操作的错误码
     *
     * @return 最后一次操作的错误码
     */
    virtual int get_last_errno() const = 0;

    /**
     * @brief 获取最后一次操作的错误字符串
     *
     * @return 最后一次操作的错误字符串
     */
    virtual const char* get_last_errstr() const = 0;
};

/**
 * @brief 创建i_mysql_iface对象实例的函数
 *
 * @param i_mysql_iface** pp_instanc 要传入的二级指针
 * @return 成功返回0， 失败返回－1
 */
int create_mysql_iface_instance(i_mysql_iface** pp_instance);

#endif // ! __I_MYSQL_IFACE_H__
