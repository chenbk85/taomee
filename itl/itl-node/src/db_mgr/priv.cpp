/** 
 * ========================================================================
 * @file priv.cpp
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-10-29
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "priv.h"
#include "global.h"
#include "database.h"
#include "table.h"
#include "column.h"
#include "instance.h"




int parse_show_grant(const char * str, std::vector< db_priv_t > & ret_vec)
{
    char db[DB_MGR_NAME_LEN] = {0};
    char table[DB_MGR_NAME_LEN] = {0};
    char priv_buf[1024] = {0};

    // 先定位到点.
    const char * p_dot = strchr(str, '.');
    const char * p_on = p_dot;
    while (' ' != *p_on)
    {
        p_on--;
        if (p_on <= str)
        {
            return -1;
        }
    }
    // p_table指向*.*
    const char * p_table = p_on + 1;
    // p_on指向"on"
    p_on -= 2;
    if (NULL == p_table)
    {
        return -1;
    }
    // 计算priv段的长度，str + 6是跳过"GRANT "
    uint32_t priv_len = p_on - 1 - (str + 6);
    if (priv_len >= sizeof(priv_buf))
    {
        return -1;
    }

    sscanf(p_table, "%[^.].%[^ ]", db, table); 
    strncpy(priv_buf, str + 6, priv_len);
    priv_buf[priv_len] = 0;

    INFO_LOG("%s => [%s] on [%s].[%s]", str, priv_buf, db, table);

    db_priv_t priv;

    if ('*' == db[0])
    {
        // 全局的权限
        if (0 != parse_global_priv(priv_buf, &priv.priv))
        {
            return -1;
        }

        ret_vec.push_back(priv);
        return 0;
    }

    uint32_t db_len = snprintf(priv.db, sizeof(priv.db), "%s", db + 1);
    priv.db[db_len - 1] = 0;

    if ('*' == table[0])
    {
        // db权限
        if (0 != parse_db_priv(priv_buf, &priv.priv))
        {
            return -1;
        }

        ret_vec.push_back(priv);
        return 0;
    }


    uint32_t table_len = snprintf(priv.table, sizeof(priv.table), "%s", table + 1);
    priv.table[table_len - 1] = 0;

    // table或者column权限
    char * p = priv_buf;
    while (*p)
    {
        // column要清零
        memset(&priv.column, 0, sizeof(priv.column));
        // 权限位清零
        priv.priv = 0;

        while (' ' == *p)
        {
            p++;
        }

        if (0 == *p)
        {
            break;
        }


        char * p_priv_str = p;

        while (*p && ',' != *p && '(' != *p)
        {
            p++;
        }

        if (p_priv_str == p)
        {
            break;
        }

        if ('(' == *p)
        {
            // column权限
            *(p - 1) = 0;
            if (0 != parse_column_priv(p_priv_str, &priv.priv))
            {
                return -1;
            }

            p++;
            char * p_column_end = strchr(p, ')');
            if (NULL == p_column_end)
            {
                // 格式不正确
                return -1;
            }

            *p_column_end = 0;

            char * p_column = p;
            char * token = NULL;
            char * save_ptr = NULL;
            while (NULL != (token = strtok_r(p_column, ",", &save_ptr)))
            {
                p_column = NULL;
                while (' ' == *token)
                {
                    token++;
                }


                memset(priv.column, 0, sizeof(priv.column));
                snprintf(priv.column, sizeof(priv.column), "%s", token);
                bool found = false;
                vector_for_each(ret_vec, it)
                {
                    db_priv_t * p_priv = &(*it);
                    if (!strcmp(p_priv->db, priv.db) && !strcmp(p_priv->table, priv.table) && !strcmp(p_priv->column, priv.column))
                    {
                        p_priv->priv |= priv.priv;
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    ret_vec.push_back(priv);
                }

            }

            p = p_column_end + 2;

        }
        else
        {
            // table权限
            *p = 0;
            if (0 != parse_table_priv(p_priv_str, &priv.priv))
            {

                return -1;
            }

            bool found = false;
            vector_for_each(ret_vec, it)
            {
                db_priv_t * p_priv = &(*it);
                if (!strcmp(p_priv->db, priv.db) && !strcmp(p_priv->table, priv.table))
                {
                    p_priv->priv |= priv.priv;
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                ret_vec.push_back(priv);
            }

            p++;
        }
    }


    return 0;

}




int get_priv(uint32_t port, const char * user, const char * host, std::vector< db_priv_t > & ret_vec)
{

    const c_mysql_instance * p_instance = find_mysql_instance(port);
    if (!p_instance)
    {
        ERROR_LOG("fail to find the sock of port: %u", port);
        return DB_MGR_ERR_SOCK;
    }

    if (0 != g_mysql->init(p_instance->m_sock, DB_MGR_NAME, DB_MGR_USER, DB_MGR_PASS, "utf8"))
    {
        ERROR_LOG("connect to mysql failed: %s", g_mysql->get_last_errstr());
        return g_mysql->get_last_errno();
    }

    GEN_SQLSTR(g_sql, "SELECT User FROM mysql.user WHERE User = '%s' AND Host = '%s'", user, host);
    int ret = g_mysql->select_first_row(NULL, "%s", g_sql);
    if (ret < 0)
    {
        ERROR_LOG("exec sql failed: %s", g_mysql->get_last_errstr());
        return g_mysql->get_last_errno();
    }

    if (0 == ret)
    {
        return 0;
    }


    GEN_SQLSTR(g_sql, "SHOW GRANTS FOR '%s'@'%s'", user, host);

    MYSQL_ROW row;
    ret = g_mysql->select_first_row(&row, "%s", g_sql);
    if (ret < 0)
    {
        ERROR_LOG("exec sql failed: %s", g_mysql->get_last_errstr());
        return g_mysql->get_last_errno();
    }




    while (NULL != row)
    {
        parse_show_grant(row[0], ret_vec);

        row = g_mysql->select_next_row(true);
    }

    return 0;
}

