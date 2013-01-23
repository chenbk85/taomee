/**
 * =====================================================================================
 *       @file  db_operator.cpp
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/13/2009 01:09:42 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <mysql/mysql.h>

#include "collector_config.h"
#include "mysql_res_auto_ptr.h"
#include "db_operator.h"

MYSQL *g_pmysql_connect = NULL;
extern s_config_variable        g_config_variable;

int32_t mysql_db_init()//初始化数据库连接
{
#ifdef  __DEBUG__
    printf("\n%sconnecting mysql database server, please waiting...%s\n", GRN_TIP, END_CLR);
#endif
    g_pmysql_connect = mysql_init(NULL);
    if(NULL == g_pmysql_connect)
    {
#ifdef  __DEBUG__
        printf("%smysql_init(NULL) failed! %s\n", RED_TIP, mysql_error(g_pmysql_connect));
#endif
        ERROR_LOG("mysql_init(NULL) failed! %s", mysql_error(g_pmysql_connect));
        return -1;
    }
    my_bool is_auto_reconnect = 1;
    if(mysql_options(g_pmysql_connect, MYSQL_OPT_RECONNECT,&is_auto_reconnect))
    {
#ifdef  __DEBUG__
        printf("%smysql_options(...) failed! %s.\n", RED_TIP, mysql_error(g_pmysql_connect));
#endif
        ERROR_LOG("mysql_options(...) failed! %s.", mysql_error(g_pmysql_connect));
        return -1;
    }
    if(NULL == mysql_real_connect(g_pmysql_connect, g_config_variable.db_host,
                g_config_variable.db_user, g_config_variable.db_pass,
                g_config_variable.db_name, g_config_variable.db_port, NULL, CLIENT_INTERACTIVE))
    {
#ifdef  __DEBUG__
        printf("%smysql_real_connect(...) failed! %s.\n", RED_TIP, mysql_error(g_pmysql_connect));
        printf("%sDatabase server:%s, port:%u, db name:%s, user:%s\n", RED_TIP,
                g_config_variable.db_host, g_config_variable.db_port,
                g_config_variable.db_name, g_config_variable.db_user);
#endif
        ERROR_LOG("mysql_real_connect(...) failed! %s.", mysql_error(g_pmysql_connect));
        ERROR_LOG("Database server:%s, port:%u, db name:%s, user:%s\n",
                g_config_variable.db_host, g_config_variable.db_port,
                g_config_variable.db_name, g_config_variable.db_user);
        return -1;
    }
    if (!mysql_set_character_set(g_pmysql_connect, "utf8"))
    {
        //DEBUG_LOG("New client character set: %s OK!", mysql_character_set_name(g_pmysql_connect));
    }
    else
    {
#ifdef  __DEBUG__
        printf("%smysql_query(...) failed! %s.\n", RED_TIP, mysql_error(g_pmysql_connect));
#endif
        ERROR_LOG("mysql_query(...) failed! %s.", mysql_error(g_pmysql_connect));
        return -1;
    }

    return 0;
}

int32_t mysql_db_finish()//断开数据库连接
{
    if(g_pmysql_connect != NULL)
    {
        mysql_close(g_pmysql_connect);
        g_pmysql_connect = NULL;
    }
    return 0;
}

inline bool make_table_name(const uint32_t mimi_number, char table_name[], const uint16_t name_len)
{
    assert(name_len > 32);
    uint16_t table_index = mimi_number % g_config_variable.table_count;
    int len = sprintf(table_name, "%s%u", g_config_variable.table_prefix, table_index);
    return len > 0;
}

inline bool format_ip(const uint32_t origin_ip, char formatted_ip[16])//将整形的ip地址转化成字符串类型
{
    strcpy(formatted_ip, "0.0.0.0");
    struct in_addr tmp_addr;
    char tmp_ip[16]={'\0'};
    tmp_addr.s_addr = origin_ip;
    socklen_t len = 16; 
    const char *p = inet_ntop(AF_INET, &tmp_addr, tmp_ip, len);
    if(NULL == p) 
    {
        ERROR_LOG("inet_ntop(AF_INET, &tmp_addr, tmp_ip, len) failed !!");
        return false;
    }
    sprintf(formatted_ip, "%s", tmp_ip);
    return true;
}

inline bool make_string_time(const time_t login_time, char string_time[32])//将整形的时间转化成字符串类型
{
    const time_t t = login_time;
    struct tm *p = localtime(&t); /*取得当地时间*/
    int32_t len = sprintf(string_time, "%u-%u-%u %02u:%02u:%02u", (1900+p->tm_year), (p->tm_mon+1), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    return len > 0;
}

bool db_process_login_info(const s_login_info &info)
{
    if(!db_insert_login_info(info))
    {
        return db_update_login_info(info);
    }
    return true;
}

//改函数暂未调用
bool db_check_login_info_exist(const s_login_info &info)
{
    bool bget_result = false;
    MYSQL_RES *psql_res=NULL;
    //检验连接
    do  
    {   
        if(mysql_ping(g_pmysql_connect))
        {   
#ifdef  __DEBUG__
            printf("%smysql_ping() ERROR, %s!\n", RED_TIP, mysql_error(g_pmysql_connect));
#endif
            ERROR_LOG("mysql_ping() ERROR, %s!", mysql_error(g_pmysql_connect));
            bget_result = false;
            break;
        }   
        char table_name[MAX_CONFIG_STR_LEN] = {'\0'};
        if(!make_table_name(info.mimi_number, table_name, MAX_CONFIG_STR_LEN))
        {
#ifdef  __DEBUG__
            printf("%smake_table_name() ERROR, %s!\n", RED_TIP, mysql_error(g_pmysql_connect));
#endif
            ERROR_LOG("make_table_name() ERROR, %s!", mysql_error(g_pmysql_connect));
            bget_result = false;
            break;
        }
        char select_sql[MAX_STR_LEN] = {'\0'};

        sprintf(select_sql, 
                "SELECT COUNT(*) FROM %s WHERE mimi_number = %u AND login_item = %u", 
                table_name, info.mimi_number, info.login_item);
        //  
        //查询数据
        if(mysql_query(g_pmysql_connect, select_sql))
        {   
            ERROR_LOG("mysql_query() ERROR, %s!, \nSQL[%s].", mysql_error(g_pmysql_connect), select_sql);
            bget_result = false;
            break;
        }   
        //存储结果
        psql_res = mysql_store_result(g_pmysql_connect);
        if(NULL == psql_res)
        {   
            ERROR_LOG("mysql_store_result() ERROR, %s!\nSQL[%s]", mysql_error(g_pmysql_connect), select_sql);
            bget_result = false;
            break;
        }   
        c_mysql_res_auto_ptr_t mysql_auto_ptr(psql_res);

        my_ulonglong result_row_count = mysql_num_rows(psql_res);
        if(result_row_count != 1)
        {
            ERROR_LOG("ERROR, magic gift error! SQL:[%s]\nresult_row_count=%u", select_sql, result_row_count);
            bget_result = false;
            break;
        }
        //格式化数据
        MYSQL_ROW sql_row;
        sql_row = mysql_fetch_row(psql_res);
        if(NULL == sql_row)
        {
            ERROR_LOG("mysql_fetch_row() ERROR, %s!\nSQL[%s].", mysql_error(g_pmysql_connect), select_sql);
            bget_result = false;
            break;
        }
        uint16_t magic_gift_count = 0;
        if(sql_row[0] != NULL)
        {
            magic_gift_count = atoi(sql_row[0]);
        }
        else
        {
            ERROR_LOG("Database ERROR, %s!SQL[%s]", mysql_error(g_pmysql_connect), select_sql);
            bget_result = false;
            break;
        }
        if(magic_gift_count == 1)
        {
            bget_result = true;
        }
        else
        {
            ERROR_LOG("Database ERROR, gift id not unique, %s!SQL[%s]", mysql_error(g_pmysql_connect), select_sql);
            bget_result = false;
        }
    }while(0);
    return bget_result;
}

bool db_insert_login_info(const s_login_info &info)
{
    bool bget_result = false;
    //检验连接
    do  
    {   
        if(mysql_ping(g_pmysql_connect))
        {   
#ifdef  __DEBUG__
            printf("%smysql_ping() ERROR, %s!\n", RED_TIP, mysql_error(g_pmysql_connect));
#endif
            ERROR_LOG("mysql_ping() ERROR, %s!", mysql_error(g_pmysql_connect));
            bget_result = false;
            break;
        }   
        char the_login_ip[16] = {'\0'};
        char the_login_time[32] = {'\0'};
        char table_name[MAX_CONFIG_STR_LEN] = {'\0'};
        if(!make_table_name(info.mimi_number, table_name, MAX_CONFIG_STR_LEN))
        {
#ifdef  __DEBUG__
            printf("%smake_table_name() ERROR!%s\n", RED_TIP, END_CLR);
#endif
            ERROR_LOG("make_table_name() ERROR!");
            bget_result = false;
            break;
        }
        if(!format_ip(info.login_ip, the_login_ip))
        {
#ifdef  __DEBUG__
            printf("%sformat_ip() ERROR!%s\n", RED_TIP, END_CLR);
#endif
            ERROR_LOG("format_ip() ERROR!");
            bget_result = false;
            break;
        }

        if(!make_string_time(info.login_time, the_login_time))
        {
#ifdef  __DEBUG__
            printf("%smake_string_time() ERROR! %s\n", RED_TIP, END_CLR);
#endif
            ERROR_LOG("make_string_time() ERROR!");
            bget_result = false;
            break;
        }
        if(info.login_ip == 0)
        {
            WARN_LOG("ifno warning, ip is zero, mimi number: %u, login item: %u, time: %s.",
                        info.mimi_number, info.login_item, the_login_time);
            bget_result = false;
            break;
        }

        char insert_sql[MAX_STR_LEN] = {'\0'};
        sprintf(insert_sql, 
                "INSERT INTO %s(mimi_number, login_item, login_ip, login_time) VALUES(%u, %u, \'%s\', \'%s\')", 
                table_name, info.mimi_number, info.login_item, the_login_ip, the_login_time);
#ifdef  __DEBUG__
            printf("%s[%s] %s\n", RED_TIP, insert_sql, END_CLR);
            DEBUG_LOG("%s[%s]%s\n", RED_TIP, insert_sql, END_CLR);
#endif
        if(mysql_query(g_pmysql_connect, insert_sql))
        { 
            //ERROR_LOG("mysql_query() ERROR, %s!, \nSQL[%s].", mysql_error(g_pmysql_connect), insert_sql);
            bget_result = false;
            break;
        }   

        my_ulonglong result_row_count = mysql_affected_rows(g_pmysql_connect);
        if(result_row_count < 0)
        {
            ERROR_LOG("mysql_affected_rows() ret=%llu, \n\t SQL:%s!",
                    result_row_count,  insert_sql);
            bget_result = false;
            break;
        }
        bget_result = true;
    }while(0);
    return bget_result;
}

bool db_update_login_info(const s_login_info &info)
{
    bool bget_result = false;
    //检验连接
    do  
    {   
        if(mysql_ping(g_pmysql_connect))
        {   
#ifdef  __DEBUG__
            printf("%smysql_ping() ERROR, %s!\n", RED_TIP, mysql_error(g_pmysql_connect));
#endif
            ERROR_LOG("mysql_ping() ERROR, %s!", mysql_error(g_pmysql_connect));
            bget_result = false;
            break;
        }   
        char table_name[MAX_CONFIG_STR_LEN] = {'\0'};
        char the_login_ip[16] = {'\0'};
        char the_login_time[32] = {'\0'};
        if(!make_table_name(info.mimi_number, table_name, MAX_CONFIG_STR_LEN))
        {
#ifdef  __DEBUG__
            printf("%smake_table_name() ERROR!%s\n", RED_TIP, END_CLR);
#endif
            ERROR_LOG("make_table_name() ERROR!");
            bget_result = false;
            break;
        }
        if(!format_ip(info.login_ip, the_login_ip))
        {
#ifdef  __DEBUG__
            printf("%sformat_ip() ERROR!%s\n", RED_TIP, END_CLR);
#endif
            ERROR_LOG("format_ip() ERROR!");
            bget_result = false;
            break;
        }
        if(!make_string_time(info.login_time, the_login_time))
        {
#ifdef  __DEBUG__
            printf("%smake_string_time() ERROR! %s\n", RED_TIP, END_CLR);
#endif
            ERROR_LOG("make_string_time() ERROR!");
            bget_result = false;
            break;
        }
        if(info.login_ip == 0)
        {
            WARN_LOG("ifno warning, ip is zero, mimi number: %u, login item: %u, time: %s.",
                        info.mimi_number, info.login_item, the_login_time);
            bget_result = false;
            break;
        }
        char update_sql[MAX_STR_LEN] = {'\0'};

        sprintf(update_sql, 
                "UPDATE %s SET login_ip = '%s', login_time = '%s' WHERE mimi_number=%u AND login_item=%u", 
                table_name, the_login_ip, the_login_time, info.mimi_number, info.login_item);
#ifdef  __DEBUG__
            printf("%s[%s]%s\n", RED_TIP, update_sql, END_CLR);
            DEBUG_LOG("%s[%s]%s\n", RED_TIP, update_sql, END_CLR);
#endif
        if(mysql_query(g_pmysql_connect, update_sql))
        { 
            ERROR_LOG("mysql_query() ERROR, %s!, \nSQL[%s].", mysql_error(g_pmysql_connect), update_sql);
            bget_result = false;
            break;
        }   

        my_ulonglong result_row_count = mysql_affected_rows(g_pmysql_connect);
        if(result_row_count < 0)
        {
            ERROR_LOG("mysql_affected_rows() ret=%llu, \n\t SQL:%s!",
                    result_row_count,  update_sql);
            bget_result = false;
            break;
        }
        bget_result = true;
    }while(0);
    return bget_result;
}

