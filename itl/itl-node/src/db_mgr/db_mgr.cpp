/** 
 * ========================================================================
 * @file db_mgr.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-22
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "db_mgr.h"
#include "../db.h"
#include "db_mgr_common.h"
#include "global.h"
#include "database.h"
#include "table.h"
#include "column.h"
#include "priv.h"
#include "instance.h"


using namespace taomee;




char g_sql[SQL_BUFF_LEN];

char g_pkg_buf[PKG_BUF_SIZE];
db_mgr_proto_t * g_out_head = reinterpret_cast< db_mgr_proto_t * >(g_pkg_buf);
db_mgr_common_t * g_out_common = reinterpret_cast< db_mgr_common_t * >(g_pkg_buf + sizeof(db_mgr_proto_t));

c_mysql_iface * g_mysql = &(SINGLETON(c_mysql_iface));


#define GET_SOCK(port, statement) \
    const c_mysql_instance * p_instance = find_mysql_instance(port); \
    if (!p_instance) { \
        result = DB_MGR_ERR_SOCK; \
        ERROR_LOG("fail to find the sock of port: %u", (port)); \
        statement; } \
    do {} while(0)



/** 查询库/表/字段信息 */
int db_mgr_get_db_base_info(int fd, const char * buf, uint32_t len)
{

    int index = SIZEOF_DB_MGR_HEAD;
    // 复制包头
    memcpy(g_pkg_buf, buf, index);

    db_mgr_instance_t db_instance;
    UNPACK_DB_INSTANCE(buf, db_instance, index);

    uint8_t level;
    unpack_h(buf, level, index);

    char user_name[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, user_name, index);

    uint32_t host_ip;
    unpack_h(buf, host_ip, index);

    char db_name[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, db_name, index);
    char table_name[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, table_name, index);
    char column_name[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, column_name, index);


    char host_ip_str[INET_ADDRSTRLEN] = {0};
    db_mgr_long2ip(host_ip, host_ip_str);

    // 解包完毕




    uint32_t result = DB_MGR_ERR_OK;
    int pack_index = SIZEOF_DB_MGR_HEAD;




    do
    {

        GET_SOCK(db_instance.port, break);

        if (db_name[0] == 0)
        {
            GEN_SQLSTR(g_sql, "SELECT SCHEMA_NAME FROM information_schema.SCHEMATA");
        }
        else if (table_name[0] == 0)
        {
            GEN_SQLSTR(g_sql, "SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_SCHEMA = \'%s\'", db_name);
        }
        else if (column_name[0] == 0)
        {
            GEN_SQLSTR(g_sql, "SELECT COLUMN_NAME FROM information_schema.COLUMNS WHERE TABLE_SCHEMA = \'%s\' AND TABLE_NAME = \'%s\'", db_name, table_name);
        }
        else
        {
            result = DB_MGR_ERR_REQ;
            break;
        }



        if (0 != g_mysql->init(p_instance->m_sock, DB_MGR_NAME, DB_MGR_USER, DB_MGR_PASS, "utf8"))
        {
            result = g_mysql->get_last_errno();
            ERROR_LOG("connect to mysql failed: %s", g_mysql->get_last_errstr());
            STORE_CONNECT_FAIL(buf);
            break;
        }


        MYSQL_ROW row;
        int ret = g_mysql->select_first_row(&row, "%s", g_sql);
        if (ret < 0)
        {
            ERROR_LOG("exec sql failed: %s", g_mysql->get_last_errstr());
            result = g_mysql->get_last_errno();
            break;
        }


        uint32_t count = 0;
        int count_index = pack_index;
        pack_h(g_pkg_buf, count, pack_index);
        int idx_name = 0;
        while (NULL != row)
        {
            char name[DB_MGR_NAME_LEN];
            DB_MGR_STRCPY(name, row[idx_name]);
            pack(g_pkg_buf, name, sizeof(name), pack_index);


            count++;


            row = g_mysql->select_next_row(true);
        }

        pack_h(g_pkg_buf, count, count_index);

    }
    while (false);

    g_out_head->len = pack_index;
    g_out_head->ret = result;
    gen_common_info(g_out_head);

    net_send_cli(fd, g_pkg_buf, pack_index);

    return 0;
}




/** 查询全局/库/表/字段的权限信息 */
int db_mgr_get_privilege_info(int fd, const char * buf, uint32_t len)
{

    int index = SIZEOF_DB_MGR_HEAD;
    // 复制包头
    memcpy(g_pkg_buf, buf, index);

    db_mgr_instance_t db_instance;
    UNPACK_DB_INSTANCE(buf, db_instance, index);

    uint8_t level;
    unpack_h(buf, level, index);

    db_priv_t priv;
    UNPACK_STRING(buf, priv.user, index);

    uint32_t host_ip;
    unpack_h(buf, host_ip, index);
    db_mgr_long2ip(host_ip, priv.host);

    UNPACK_STRING(buf, priv.db, index);
    UNPACK_STRING(buf, priv.table, index);
    UNPACK_STRING(buf, priv.column, index);



    // 解包完毕




    uint32_t port = db_instance.port;
    uint32_t result = DB_MGR_ERR_OK;
    int pack_index = SIZEOF_DB_MGR_HEAD;




    std::vector< db_priv_t > priv_vec;
    priv_vec.clear();

    result = get_priv(port, priv.user, priv.host, priv_vec);

    if (DB_MGR_ERR_OK != result)
    {
        db_store_db_mgr_result(g_out_common->serial_no1, 
                g_out_common->serial_no2, SQL_NO_START, result, 
                g_sql, g_mysql->get_last_errstr());
    }
    else
    {
        uint32_t count = 0;
        int count_index = pack_index;
        pack_h(g_pkg_buf, count, pack_index);
        vector_for_each(priv_vec, it)
        {
            db_priv_t * p_priv = &(*it);

            bool pack_flag = false;

            switch (level)
            {
                case LEVEL_GLOBAL:
                    // 全局权限
                    // 要求db/table/column都是空
                    if (!p_priv->db[0] && !p_priv->table[0] && !p_priv->column[0])
                    {
                        pack_flag = true;
                    }
                    break;
                case LEVEL_TOTAL:
                    if (!priv.db[0])
                    {
                        // 所有的db权限
                        // 要求db不为空，table和column都是空
                        if (p_priv->db[0] && !p_priv->table[0] && !p_priv->column[0])
                        {
                            pack_flag = true;
                        }
                    }
                    else if (!priv.table[0])
                    {
                        // 给定db下的，所有table权限
                        if (!strcmp(p_priv->db, priv.db) && p_priv->table[0] && !p_priv->column[0])
                        {
                            pack_flag = true;

                        }
                    }
                    else if (!priv.column[0])
                    {
                        // 给定db和table下的，所有column权限
                        if (!strcmp(p_priv->db, priv.db) && !strcmp(p_priv->table, priv.table) && p_priv->column[0])
                        {
                            pack_flag = true;
                        }
                    }
                    else
                    {
                        // impossible here
                    }
                    break;
                case LEVEL_EXACT:
                    if (!priv.db[0])
                    {
                        // impossible here
                    }
                    else if (!priv.table[0])
                    {
                        // 给定db的权限
                        if (!strcmp(p_priv->db, priv.db) && !p_priv->table[0] && !p_priv->column[0])
                        {
                            pack_flag = true;
                        }
                    }
                    else if (!priv.column[0])
                    {
                        // 给定table的权限
                        if (!strcmp(p_priv->db, priv.db) && !strcmp(p_priv->table, priv.table) && !p_priv->column[0])
                        {
                            pack_flag = true;
                        }
                    }
                    else 
                    {
                        // 给定column的权限
                        if (!strcmp(p_priv->db, priv.db) && !strcmp(p_priv->table, priv.table) && !strcmp(p_priv->column, priv.column))
                        {
                            pack_flag = true;
                        }
                    }
                    break;
                case LEVEL_TABLE:
                    // 所有table的权限
                    if (p_priv->db[0] && p_priv->table[0] && !p_priv->column[0])
                    {
                        pack_flag = true;
                    }
                    break;
                case LEVEL_COLUMN:
                    // 所有column的权限
                    if (p_priv->db[0] && p_priv->table[0] && p_priv->column[0])
                    {
                        pack_flag = true;
                    }
                    break;
                default:
                    break;
            }

            if (pack_flag)
            {
                PACK_DB_PRIV(g_pkg_buf, *p_priv, pack_index);
                count++;
            }
        }

        pack_h(g_pkg_buf, count, count_index);

    }

    g_out_head->len = pack_index;
    g_out_head->ret = result;
    gen_common_info(g_out_head);
    net_send_cli(fd, g_pkg_buf, pack_index);

    return 0;
}

/** 创建数据库账户，包括修改密码 */
int db_mgr_create_db_user(int fd, const char * buf, uint32_t len)
{
    int index = SIZEOF_DB_MGR_HEAD;
    memcpy(g_pkg_buf, buf, index);

    db_mgr_instance_t db_instance;
    UNPACK_DB_INSTANCE(buf, db_instance, index);

    uint8_t do_sql = 0;
    unpack_h(buf, do_sql, index);

    char dba_user[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, dba_user, index);
    char dba_pass[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, dba_pass, index);
    decode(dba_pass, dba_pass, sizeof(dba_pass));

    char new_user[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, new_user, index);
    char new_pass[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, new_pass, index);

    // 解包完毕



    uint32_t result = DB_MGR_ERR_OK;
    int pack_index = SIZEOF_DB_MGR_HEAD;

    // 先直接返回报文，处理结果待会儿入库
    g_out_head->len = pack_index;
    g_out_head->ret = result;
    gen_common_info(g_out_head);
    net_send_cli(fd, g_pkg_buf, pack_index);

    do
    {
        GET_SOCK(db_instance.port, break);
        if (0 != g_mysql->init(p_instance->m_sock, DB_MGR_NAME, dba_user, dba_pass, "utf8"))
        {
            result = g_mysql->get_last_errno();
            ERROR_LOG("connect to mysql failed: %s", g_mysql->get_last_errstr());
            STORE_CONNECT_FAIL(buf);
            break;
        }
        uint8_t host_count = 0;
        unpack_h(buf, host_count, index);
        for (uint32_t i = 0; i < host_count; i++)
        {
            uint32_t host_id = 0;
            unpack_h(buf, host_id, index);
            uint32_t host_ip = 0;
            unpack_h(buf, host_ip, index);
            char host_ip_str[INET_ADDRSTRLEN];
            db_mgr_long2ip(host_ip, host_ip_str);

            GEN_SQLSTR(g_sql, "GRANT USAGE ON *.* TO '%s'@'%s' IDENTIFIED BY PASSWORD '%s'",
                    new_user, host_ip_str, new_pass);
            int ret = g_mysql->execsql("%s", g_sql);
            if (ret < 0)
            {
                ERROR_LOG("exec sql failed: %s", g_mysql->get_last_errstr());
                result = g_mysql->get_last_errno();
                db_store_db_mgr_result(g_out_common->serial_no1, 
                        host_id, SQL_NO_START, result, 
                        g_sql, g_mysql->get_last_errstr());
            }
            else
            {
                db_store_db_mgr_result(g_out_common->serial_no1, 
                        host_id, SQL_NO_START, DB_MGR_ERR_OK, g_sql);
            }
        }

    }
    while (false);






    return 0;
}

/** 删除数据库账户 */
int db_mgr_delete_db_user(int fd, const char * buf, uint32_t len)
{
    int index = SIZEOF_DB_MGR_HEAD;
    memcpy(g_pkg_buf, buf, index);

    db_mgr_instance_t db_instance;
    UNPACK_DB_INSTANCE(buf, db_instance, index);

    uint8_t do_sql = 0;
    unpack_h(buf, do_sql, index);

    char dba_user[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, dba_user, index);
    char dba_pass[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, dba_pass, index);
    decode(dba_pass, dba_pass, sizeof(dba_pass));






    uint32_t result = DB_MGR_ERR_OK;
    int pack_index = SIZEOF_DB_MGR_HEAD;


    // 先直接返回报文，处理结果待会儿入库
    g_out_head->len = pack_index;
    g_out_head->ret = result;
    gen_common_info(g_out_head);
    net_send_cli(fd, g_pkg_buf, pack_index);

    do
    {
        GET_SOCK(db_instance.port, break);
        if (0 != g_mysql->init(p_instance->m_sock, DB_MGR_NAME, dba_user, dba_pass, "utf8"))
        {
            result = g_mysql->get_last_errno();
            ERROR_LOG("connect to mysql failed: %s", g_mysql->get_last_errstr());
            STORE_CONNECT_FAIL(buf);
            break;
        }

        uint8_t del_count = 0;
        unpack_h(buf, del_count, index);
        for (uint32_t i = 0; i < del_count; i++)
        {
            uint8_t del_id = 0;
            unpack_h(buf, del_id, index);
            char del_user[DB_MGR_NAME_LEN];
            UNPACK_STRING(buf, del_user, index);
            uint32_t del_ip = 0;
            unpack_h(buf, del_ip, index);
            char del_ip_str[INET_ADDRSTRLEN];
            db_mgr_long2ip(del_ip, del_ip_str);

            GEN_SQLSTR(g_sql, "DROP USER '%s'@'%s'", del_user, del_ip_str);
            int ret = g_mysql->execsql("%s", g_sql);
            if (ret < 0)
            {
                ERROR_LOG("exec sql failed: %s", g_mysql->get_last_errstr());
                result = g_mysql->get_last_errno();
                db_store_db_mgr_result(g_out_common->serial_no1, 
                        del_id, SQL_NO_START, result, 
                        g_sql, g_mysql->get_last_errstr());
            }
            else
            {
                db_store_db_mgr_result(g_out_common->serial_no1, 
                        del_id, SQL_NO_START, DB_MGR_ERR_OK, g_sql);
            }
        }


    }
    while (false);


    return 0;
}

/** 检测数据库账户是否存在 */
int db_mgr_check_db_user(int fd, const char * buf, uint32_t len)
{
    int index = SIZEOF_DB_MGR_HEAD;
    memcpy(g_pkg_buf, buf, index);

    db_mgr_instance_t db_instance;
    UNPACK_DB_INSTANCE(buf, db_instance, index);

    uint8_t do_sql = 0;
    unpack_h(buf, do_sql, index);

    char user_name[DB_MGR_NAME_LEN];
    UNPACK_STRING(buf, user_name, index);
    uint32_t host_ip = 0;
    unpack_h(buf, host_ip, index);
    char ip_str[INET_ADDRSTRLEN] = {0};
    db_mgr_long2ip(host_ip, ip_str);

    // 解包完了

    int pack_index = SIZEOF_DB_MGR_HEAD;
    uint32_t result = DB_MGR_ERR_OK;

    GEN_SQLSTR(g_sql, "SELECT User, Host FROM mysql.user WHERE User = '%s' AND Host = '%s'", user_name, ip_str);

    if (do_sql)
    {
        // 要执行sql语句
        do
        {
            GET_SOCK(db_instance.port, break);
            if (0 != g_mysql->init(p_instance->m_sock, DB_MGR_NAME, DB_MGR_USER, DB_MGR_PASS, "utf8"))
            {
                // 没连上
                result = g_mysql->get_last_errno();
                ERROR_LOG("connect to mysql failed: %s", g_mysql->get_last_errstr());
                STORE_CONNECT_FAIL(buf);
                break;
            }



            uint32_t sql_no = SQL_NO_START;
            int ret = g_mysql->select_first_row(NULL, "%s", g_sql);
            if (ret < 0)
            {
                // 查询失败
                ERROR_LOG("exec sql failed: %s", g_mysql->get_last_errstr());
                result = g_mysql->get_last_errno();
                db_store_db_mgr_result(g_out_common->serial_no1, 
                        sql_no, sql_no, result, 
                        g_sql, g_mysql->get_last_errstr());
                break;
            }

            uint8_t user_exist = 0;

            if (0 == ret)
            {
                // 没有该用户
                user_exist = 0;
            }
            else if (1 == ret)
            {
                user_exist = 1;
            }
            else
            {
                // 不止一个结果
                // 不可能出现，因为user和host是mysql.user的主键
            }
            pack_h(g_pkg_buf, user_exist, pack_index);

        }
        while (false);

    }
    else
    {
        // 只是构造sql语句
        pack(g_pkg_buf, g_sql, strlen(g_sql) + 1, pack_index);
    }


    g_out_head->len = pack_index;
    g_out_head->ret = result;
    gen_common_info(g_out_head);

    net_send_cli(fd, g_pkg_buf, pack_index);

    return 0;
}

/** 获取数据库所有账户名 */
int db_mgr_get_db_user(int fd, const char * buf, uint32_t len)
{
    int index = SIZEOF_DB_MGR_HEAD;
    memcpy(g_pkg_buf, buf, index);


    db_mgr_instance_t db_instance;
    UNPACK_DB_INSTANCE(buf, db_instance, index);


    // 解包完了


    int pack_index = SIZEOF_DB_MGR_HEAD;
    uint32_t result = DB_MGR_ERR_OK;

    GEN_SQLSTR(g_sql, "SELECT User, Host FROM mysql.user;");

    // 要执行sql语句
    do
    {
        GET_SOCK(db_instance.port, break);
        if (0 != g_mysql->init(p_instance->m_sock, DB_MGR_NAME, DB_MGR_USER, DB_MGR_PASS, "utf8"))
        {
            // 没连上
            result = g_mysql->get_last_errno();
            ERROR_LOG("connect to mysql failed: %s", g_mysql->get_last_errstr());
            STORE_CONNECT_FAIL(buf);
            break;
        }


        uint32_t sql_no = SQL_NO_START;
        MYSQL_ROW row;
        int ret = g_mysql->select_first_row(&row, g_sql);
        if (ret < 0)
        {
            // 查询失败
            ERROR_LOG("exec sql failed: %s", g_mysql->get_last_errstr());
            result = g_mysql->get_last_errno();
            db_store_db_mgr_result(g_out_common->serial_no1, 
                    sql_no, sql_no, result, 
                    g_sql, g_mysql->get_last_errstr());
            break;
        }

        uint32_t count = 0;
        int count_index = pack_index;
        pack_h(g_pkg_buf, count, pack_index);
        int idx_user = 0;
        int idx_host = 1;
        while (NULL != row)
        {
            char user_name[DB_MGR_NAME_LEN];
            char host[INET_ADDRSTRLEN];

            DB_MGR_STRCPY(user_name, row[idx_user]);
            DB_MGR_STRCPY(host, row[idx_host]);
            PACK_STRING(g_pkg_buf, user_name, pack_index);
            PACK_STRING(g_pkg_buf, host, pack_index);

            count++;

            row = g_mysql->select_next_row(true);
        }


        pack_h(g_pkg_buf, count, count_index);
    } 
    while(false);

    g_out_head->len = pack_index;
    g_out_head->ret = result;
    gen_common_info(g_out_head);

    net_send_cli(fd, g_pkg_buf, pack_index);

    return 0;
}

/** 检测数据库账户密码是否正确 */
int db_mgr_check_user_password(int fd, const char * buf, uint32_t len)
{
    int index = SIZEOF_DB_MGR_HEAD;
    memcpy(g_pkg_buf, buf, index);

    db_mgr_instance_t db_instance;
    UNPACK_DB_INSTANCE(buf, db_instance, index);

    uint8_t do_sql = 0;
    unpack_h(buf, do_sql, index);

    char user_name[DB_MGR_NAME_LEN];
    UNPACK_STRING(buf, user_name, index);
    uint32_t host_ip = 0;
    unpack_h(buf, host_ip, index);
    char ip_str[INET_ADDRSTRLEN];
    db_mgr_long2ip(host_ip, ip_str);
    char user_pass[DB_MGR_NAME_LEN];
    UNPACK_STRING(buf, user_pass, index);

    // 解包完了

    int pack_index = SIZEOF_DB_MGR_HEAD;
    uint32_t result = DB_MGR_ERR_OK;

    GEN_SQLSTR(g_sql, "SELECT Password FROM mysql.user WHERE User = '%s' AND Host = '%s'", user_name, ip_str);
    // INFO_LOG("sql: %s", g_sql);

    if (do_sql)
    {
        // 要执行sql语句
        do
        {
            GET_SOCK(db_instance.port, break);
            if (0 != g_mysql->init(p_instance->m_sock, DB_MGR_NAME, DB_MGR_USER, DB_MGR_PASS, "utf8"))
            {
                // 没连上
                result = g_mysql->get_last_errno();
                ERROR_LOG("connect to mysql failed: %s", g_mysql->get_last_errstr());
                STORE_CONNECT_FAIL(buf);
                break;
            }


            uint32_t sql_no = SQL_NO_START;
            MYSQL_ROW row;
            int ret = g_mysql->select_first_row(&row, "%s", g_sql);
            if (ret < 0)
            {
                // 查询失败
                ERROR_LOG("exec sql failed: %s", g_mysql->get_last_errstr());
                result = g_mysql->get_last_errno();
                db_store_db_mgr_result(g_out_common->serial_no1, 
                        sql_no, sql_no, result, 
                        g_sql, g_mysql->get_last_errstr());
                break;
            }

            uint8_t user_exist = 2;

            if (0 == ret)
            {
                // 没有该用户
                user_exist = 2;
            }
            else if (1 == ret)
            {
                // 用户存在，要检查密码
                if (0 == strncmp(user_pass, row[0], DB_MGR_NAME_LEN))
                {
                    // 密码匹配
                    user_exist = 0;
                }
                else
                {
                    // 密码不匹配
                    user_exist = 1;
                }
            }
            else
            {
                // 不止一个结果
                // 不可能出现，因为user和host是mysql.user的主键
            }

            pack_h(g_pkg_buf, user_exist, pack_index);
        }
        while (false);

    }
    else
    {
        // 只是构造sql语句
        pack(g_pkg_buf, g_sql, strlen(g_sql) + 1, pack_index);
    }


    g_out_head->len = pack_index;
    g_out_head->ret = result;
    gen_common_info(g_out_head);

    net_send_cli(fd, g_pkg_buf, pack_index);

    return 0;
}





/** 修改数据库账户权限 */
int db_mgr_update_user_privilege(int fd, const char * buf, uint32_t len)
{
    int index = SIZEOF_DB_MGR_HEAD;
    memcpy(g_pkg_buf, buf, index);

    db_mgr_instance_t db_instance;
    UNPACK_DB_INSTANCE(buf, db_instance, index);

    uint8_t do_sql = 0;
    unpack_h(buf, do_sql, index);

    char dba_user[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, dba_user, index);
    char dba_pass[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, dba_pass, index);
    decode(dba_pass, dba_pass, sizeof(dba_pass));

    db_priv_t priv;
    UNPACK_STRING(buf, priv.user, index);

    uint32_t result = DB_MGR_ERR_OK;
    int pack_index = SIZEOF_DB_MGR_HEAD;


    // 先直接返回报文，处理结果待会儿入库
    g_out_head->len = pack_index;
    g_out_head->ret = result;
    gen_common_info(g_out_head);
    net_send_cli(fd, g_pkg_buf, pack_index);

    do 
    {
        GET_SOCK(db_instance.port, break);
        if (0 != g_mysql->init(p_instance->m_sock, DB_MGR_NAME, dba_user, dba_pass, "utf8"))
        {
            result = g_mysql->get_last_errno();
            ERROR_LOG("connect to mysql failed: %s", g_mysql->get_last_errstr());
            STORE_CONNECT_FAIL(buf);
            break;
        }

        uint32_t count = 0;
        unpack_h(buf, count, index);
        for (uint32_t i = 0; i < count; i++)
        {
            uint32_t priv_no = 0;
            unpack_h(buf, priv_no, index);

            uint32_t host_ip = 0;
            unpack_h(buf, host_ip, index);
            db_mgr_long2ip(host_ip, priv.host);

            unpack_h(buf, priv.priv, index);

            UNPACK_STRING(buf, priv.db, index);
            UNPACK_STRING(buf, priv.table, index);
            UNPACK_STRING(buf, priv.column, index);

            char add_priv_buf[1024] = {0};
            char del_priv_buf[1024] = {0};
            char table_string[DB_MGR_NAME_LEN * 2 + 5];
            if (0 == priv.db[0])
            {
                GEN_SQLSTR(table_string, "*.*");
                gen_global_priv_string(priv.priv, add_priv_buf, sizeof(add_priv_buf));
                gen_global_priv_string(~priv.priv, del_priv_buf, sizeof(del_priv_buf));
            }
            else if (0 == priv.table[0])
            {
                GEN_SQLSTR(table_string, "`%s`.*", priv.db);
                gen_db_priv_string(priv.priv, add_priv_buf, sizeof(add_priv_buf));
                gen_db_priv_string(~priv.priv, del_priv_buf, sizeof(del_priv_buf));
            }
            else if (0 == priv.column[0])
            {
                GEN_SQLSTR(table_string, "`%s`.`%s`", priv.db, priv.table);
                gen_table_priv_string(priv.priv, add_priv_buf, sizeof(add_priv_buf));
                gen_table_priv_string(~priv.priv, del_priv_buf, sizeof(del_priv_buf));
            }
            else
            {
                GEN_SQLSTR(table_string, "`%s`.`%s`", priv.db, priv.table);
                gen_column_priv_string(&priv, add_priv_buf, sizeof(add_priv_buf));
                priv.priv = ~priv.priv;
                gen_column_priv_string(&priv, del_priv_buf, sizeof(del_priv_buf));
                priv.priv = ~priv.priv;
            }

            uint32_t sql_no = SQL_NO_START;

            if (add_priv_buf[0])
            {
                GEN_SQLSTR(g_sql, "GRANT %s ON %s TO '%s'@'%s'", 
                        add_priv_buf, 
                        table_string, 
                        priv.user,
                        priv.host);
                INFO_LOG("sql: %s", g_sql);

                int ret = g_mysql->execsql("%s", g_sql);
                if (ret < 0)
                {
                    ERROR_LOG("exec sql failed: %s", g_mysql->get_last_errstr());
                    result = g_mysql->get_last_errno();
                    db_store_db_mgr_result(g_out_common->serial_no1, 
                            priv_no, sql_no, result, 
                            g_sql, g_mysql->get_last_errstr());
                    break;
                }
                else
                {
                    db_store_db_mgr_result(g_out_common->serial_no1, 
                            priv_no, sql_no, DB_MGR_ERR_OK, g_sql);
                }

                sql_no++;
            }


            if (del_priv_buf[0])
            {
                GEN_SQLSTR(g_sql, "REVOKE %s ON %s FROM '%s'@'%s'",
                        del_priv_buf, 
                        table_string, 
                        priv.user, 
                        priv.host);

                INFO_LOG("sql: %s", g_sql);

                int ret = g_mysql->execsql("%s", g_sql);
                if (ret < 0)
                {
                    ERROR_LOG("exec sql failed: %s", g_mysql->get_last_errstr());
                    result = g_mysql->get_last_errno();
                    db_store_db_mgr_result(g_out_common->serial_no1, 
                            priv_no, sql_no, result, 
                            g_sql, g_mysql->get_last_errstr());
                    break;
                }
                else
                {
                    db_store_db_mgr_result(g_out_common->serial_no1, 
                            priv_no, sql_no, DB_MGR_ERR_OK, g_sql);
                }

                sql_no++;

            }

        }
    }
    while (false);

    return 0;
}



int db_mgr_show_grants(int fd, const char * buf, uint32_t len)
{
    int index = SIZEOF_DB_MGR_HEAD;
    memcpy(g_pkg_buf, buf, index);

    db_mgr_instance_t db_instance;
    UNPACK_DB_INSTANCE(buf, db_instance, index);

    char user_name[DB_MGR_NAME_LEN];
    UNPACK_STRING(buf, user_name, index);
    uint32_t host_ip = 0;
    unpack_h(buf, host_ip, index);
    char host_ip_str[INET_ADDRSTRLEN] = {0};
    db_mgr_long2ip(host_ip, host_ip_str);


    // 解包完了

    int pack_index = SIZEOF_DB_MGR_HEAD;
    uint32_t result = DB_MGR_ERR_OK;

    GEN_SQLSTR(g_sql, "SHOW GRANTS FOR '%s'@'%s'", user_name, host_ip_str);

    do
    {
        GET_SOCK(db_instance.port, break);
        if (0 != g_mysql->init(p_instance->m_sock, DB_MGR_NAME, DB_MGR_USER, DB_MGR_PASS, "utf8"))
        {
            // 没连上
            result = g_mysql->get_last_errno();
            ERROR_LOG("connect to mysql failed: %s", g_mysql->get_last_errstr());
            STORE_CONNECT_FAIL(buf);
            break;
        }



        MYSQL_ROW row;
        int ret = g_mysql->select_first_row(&row, "%s", g_sql);
        if (ret < 0)
        {
            // 查询失败
            ERROR_LOG("exec sql failed: %s", g_mysql->get_last_errstr());
            // 查询失败的返回count = 0
            row = NULL;
        }

        uint32_t count = 0;
        int count_index = pack_index;
        pack_h(g_pkg_buf, count, pack_index);

        while (NULL != row)
        {
            const char * p_line = (const char *)row[0];
            uint32_t line_len = strlen(p_line) + 1;
            pack_h(g_pkg_buf, line_len, pack_index);
            pack(g_pkg_buf, p_line, line_len, pack_index);

            count++;
            row = g_mysql->select_next_row(true);

        }


        pack_h(g_pkg_buf, count, count_index);
    }
    while(false);

    g_out_head->len = pack_index;
    g_out_head->ret = result;
    gen_common_info(g_out_head);

    net_send_cli(fd, g_pkg_buf, pack_index);


    return 0;
}


int db_mgr_exec_sql(int fd, const char * buf, uint32_t len)
{
    int index = SIZEOF_DB_MGR_HEAD;
    memcpy(g_pkg_buf, buf, index);

    db_mgr_instance_t db_instance;
    UNPACK_DB_INSTANCE(buf, db_instance, index);

    char dba_user[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, dba_user, index);
    char dba_pass[DB_MGR_NAME_LEN] = {0};
    UNPACK_STRING(buf, dba_pass, index);
    decode(dba_pass, dba_pass, sizeof(dba_pass));

    uint32_t value_id = 0;
    unpack_h(buf, value_id, index);
    uint32_t sql_len = 0;
    unpack_h(buf, sql_len, index);

    char * p_sql_buf = (char *)malloc(sql_len + 1);
    if (NULL == p_sql_buf)
    {
        return -1;
    }

    memcpy(p_sql_buf, buf + index, sql_len);
    p_sql_buf[sql_len] = 0;

    // 保存下完整的sql，最后结果成功的话，要入库记录的
    char * p_entire_sql = strdup(p_sql_buf);
    if (NULL == p_entire_sql)
    {
        return -1;
    }

    INFO_LOG("sql: %s", p_sql_buf);

    uint32_t result = DB_MGR_ERR_OK;
    int pack_index = SIZEOF_DB_MGR_HEAD;


    // 先直接返回报文，处理结果待会儿入库
    g_out_head->len = pack_index;
    g_out_head->ret = result;
    gen_common_info(g_out_head);
    net_send_cli(fd, g_pkg_buf, pack_index);

    do
    {
        GET_SOCK(db_instance.port, break);
        if (0 != g_mysql->init(p_instance->m_sock, DB_MGR_NAME, dba_user, dba_pass, "utf8"))
        {
            // 没连上
            result = g_mysql->get_last_errno();
            db_store_db_mgr_result(g_out_common->serial_no1, 
                    value_id, 1, result, NULL, 
                    g_mysql->get_last_errstr());
            break;
        }


        uint32_t sql_no = SQL_NO_START;
        char * p = p_sql_buf;
        char * p_sql = NULL;
        char * save_ptr = NULL;
        while (NULL != (p_sql = strtok_r(p, ";", &save_ptr)))
        {
            p = NULL;

            while (' ' == *p_sql || '\n' == *p_sql)
            {
                p_sql++;
            }

            int ret = 0;
            if (!strncasecmp(p_sql, "SELECT", 6) || !strncasecmp(p_sql, "SHOW GRANTS", 11))
            {
                // 是select语句
                ret = g_mysql->select_first_row(NULL, "%s", p_sql);
            }
            else
            {
                ret = g_mysql->execsql("%s", p_sql);
            }

            if (ret < 0)
            {
                ERROR_LOG("exec sql failed: %s", g_mysql->get_last_errstr());
                result = g_mysql->get_last_errno();
                db_store_db_mgr_result(g_out_common->serial_no1, 
                        value_id, sql_no, result, 
                        p_sql, g_mysql->get_last_errstr());
                break;
            }

            sql_no++;
        }


        if (DB_MGR_ERR_OK == result)
        {
            db_store_db_mgr_result(g_out_common->serial_no1, 
                    value_id, SQL_NO_START, result, p_entire_sql);
        }

    }
    while (false);

    free(p_entire_sql);
    free(p_sql_buf);

    return 0;
}



//========================== 公用函数部分 ==========================

char * db_mgr_long2ip(uint32_t ip_num, char * ip_str)
{
    if (ip_num == 0)
    {
        strcpy(ip_str, "localhost");
        return ip_str;
    }
    else if (ip_num == uint32_t(~0))
    {
        strcpy(ip_str, "%");
        return ip_str;
    }   

    uint32_t ip_seg[4];
    char seg_str[4][4];
    for(int i = 0; i < 4; i++)
    {
        ip_seg[i] = ip_num & 0xFF;
        ip_num = ip_num >> 8;
        snprintf(seg_str[i], sizeof(seg_str[i]), "%u", ip_seg[i]);
    }   

    sprintf(ip_str, "%s.%s.%s.%s",
            ip_seg[3] == 255 ? "%" : seg_str[3],
            ip_seg[2] == 255 ? "%" : seg_str[2],
            ip_seg[1] == 255 ? "%" : seg_str[1],
            ip_seg[0] == 255 ? "%" : seg_str[0]);

    return ip_str;
}



uint32_t db_mgr_ip2long(const char * ip)
{
    if (strcmp(ip, "localhost") == 0)
    {
        return 0;
    }
    else if (strcmp(ip, "%") == 0)
    {
        return ~0;
    }
    else
    {
        char cip[16];
        strcpy(cip, ip);
        uint32_t aip[4];
        int index = 1;
        aip[0] = 0;
        for (uint32_t i = 0; i < sizeof(cip); i++)
        {
            if (cip[i] == '.')
            {
                aip[index++] = i+1;
                cip[i] = 0;
            }
        }
        for (uint32_t i = 0; i < sizeof(aip) / sizeof(aip[0]); i++)
        {
            if (cip[aip[i]] == '%')
            {
                aip[i] = 255;
            }
            else
            {
                aip[i] = atoi(&cip[aip[i]]);
            }
        }
        return ((aip[3]<<24) + (aip[2]<<16) + (aip[1]<<8) + aip[0]);
    }
}


int gen_common_md5(uint16_t cmd, uint32_t timestamp, uint32_t no, char * p_buf)
{
    char tmp_str[DB_MGR_STR_LEN] = {0};
    snprintf(tmp_str, sizeof(tmp_str), 
            "%s%u%u%u", 
            DB_MGR_PRIVATE_KEY,
            cmd, timestamp, no);

    return str2md5(tmp_str, p_buf);
}


bool is_legal_common_info(const db_mgr_proto_t * pkg)
{
    const db_mgr_common_t * p_common = reinterpret_cast< const db_mgr_common_t * >(pkg->body);
    char md5[33] = {0};
    gen_common_md5(pkg->cmd, p_common->timestamp, p_common->serial_no1, md5);
    if (0 == strncmp(p_common->veri_code, md5, sizeof(p_common->veri_code)))
    {
        return true;
    }
    else
    {
        memcpy(md5, p_common->veri_code, 32);
        ERROR_LOG("illegal veri_code[%s]", md5);
    }

    return false;
}



int gen_common_info(db_mgr_proto_t * p_head)
{
    db_mgr_common_t * p_common = reinterpret_cast< db_mgr_common_t * >(p_head->body);
    p_common->timestamp = time(NULL);
    char md5[33] = {0};
    gen_common_md5(p_head->cmd, p_common->timestamp, p_common->serial_no1, md5);

    memcpy(p_common->veri_code, md5, 32);
    return 0;
}



// 对dba_pass进行解码
char * decode(const void * p_code, void * p_buf, uint32_t len)
{
    const uint8_t * code = (const uint8_t *)(p_code);
    uint8_t code_len = ((code[len - 2] ^ (0x91)) - 0x30) * 10 + (code[len - 1] ^ (0x91)) - 0x30;
    code_len = code_len > len ? len-1 : code_len;
    uint8_t * buf = (uint8_t *)p_buf;
    for (uint8_t i = 0; i < code_len; i++)
    {
        buf[i] = code[i] ^ (0x91);
    }
    buf[code_len] = 0;
    // DEBUG_LOG("dba passwd %s", buf);
    return (char *)buf;
}


int disconnect_to_mysql()
{
    return g_mysql->uninit();
}
