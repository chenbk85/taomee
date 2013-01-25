/**
 * =====================================================================================
 *       @file  mysql_metric.cpp
 *      @brief  
 *
 *    收集mysql服务器监控项
 *
 *   @internal
 *     Created  08/04/2011 05:26:10 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <vector>
#include <map>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <limits.h>
#include <pthread.h>

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/utils.h>
#include <libtaomee/inet/inet_utils.h>
}
#include "mysql_metric.h"
#include "mysql_res_auto_ptr.h"
#include "instance.h"

using namespace std;

const unsigned short     MAX_UINT16 = 0XFFFF;
const short              MAX_INT16  = 0X7FFF;
const unsigned int       MAX_UINT32 = 0XFFFFFFFF;
const int                MAX_INT32  = 0X7FFFFFFF;
const unsigned long long MAX_UINT64 = (~0);
const long long          MAX_INT64  = (~(1LL<<63));


int update_metric_value(c_metric * p_metric, const char * value_string);
int update_common_metric_data(c_instance * p_instance);
int update_processlist_info(c_instance * p_instance);
int update_error_log_info(c_instance * p_instance);
int update_user_host_priv_info(c_instance * p_instance);
int update_innodb_deadlock_info(c_instance * p_instance);
int update_slave_metric(c_instance * p_instance);
int process_calculate_metric(c_instance * p_instance);






/**
 * @brief  获取行
 * @param  str 字符串
 * @return NULL or UNNULL
 */
const char * get_line(const char * str, char * buf, int len)
{
    if (NULL == str || 0 == *str || NULL == buf || len <= 0)
    {
        ERROR_LOG("ERROR: parameter wrong.");
        return NULL;
    }

    int i = 0;
    const char * p = str;
    while(*p != 0 && *p != '\n' && i < len)
    {
        if (isascii(*p) && !iscntrl(*p))
        {
            buf[i++] = *p;
        }
        p++;
    }

    if (i < len)
    {
        buf[i] = 0;
    }
    return buf;
}

/**
 * @brief  移动到下一行开始的位置
 * @param  str 字符串
 * @return NULL or UNNULL
 */
const char * move_to_next_line(const char* str)
{
    if (NULL == str || 0 == *str)
    {
        return NULL;
    }
    const char * begin_line = index(str, '\n');
    if (NULL != begin_line)
    {
        return 0 == begin_line[1] ? NULL : (begin_line + 1);
    }
    else
    {
        return NULL;
    }
}



int get_instance_alive_value(uint32_t port, c_value * p_v)
{
    c_instance * p_instance = find_instance(port);
    int32_t v = 0;
    if (NULL == p_instance)
    {
        v = 2;
    }
    else
    {
        v = p_instance->m_alive ? 1 : 2;
    }

    *p_v = v;
    return 0;
}


int get_value_by_metric_name(uint32_t port, const char * metric_name, c_value * p_v)
{
    if (NULL == metric_name)
    {
        return -1;
    }

    if (0 == strcmp(metric_name, "instance_alive"))
    {
        return get_instance_alive_value(port, p_v);
    }

    c_instance * p_instance = find_instance(port);
    if (NULL == p_instance)
    {
        return -1;
    }

    if (!p_instance->is_connected())
    {
        connect_instance(p_instance);
        if (!p_instance->is_connected())
        {
            return -2;
        }


    }

    c_metric * p_metric = find_metric(p_instance, metric_name);
    if (NULL == p_metric)
    {
        return -1;
    }

    if (p_metric->m_new)
    {
        *p_v = p_metric->m_value;
        p_metric->m_new = false;
        return 0;
    }

    // 要更新metric
    if (!strcmp(metric_name, "last_locked_processlist") || 
        !strcmp(metric_name, "max_used_connections") || 
        !strcmp(metric_name, "have_locked_processlist"))
    {
        DEBUG_LOG("starting to update processlist info[%u: %s]", port, metric_name);
        if (0 != update_processlist_info(p_instance))
        {
            ERROR_LOG("get instance %u's processlist info failed", port);
            return -1;
        }
    }
    else if (!strcmp(metric_name, "last_log_error") || 
            !strcmp(metric_name, "error_log_size") || 
            !strcmp(metric_name, "error_log_access")) 
    {
        DEBUG_LOG("starting to update log error info[%u: %s]", port, metric_name);
        if (0 != update_error_log_info(p_instance)) 
        {
            ERROR_LOG("get instance %u's error log info failed", port);
            return -1;
        }
    }
    else if (!strcmp(metric_name, "have_user_priv_root_grant") ||
            !strcmp(metric_name, "user_host_priv") || 
            !strcmp(metric_name, "have_user_priv_host_wildcard"))
    {
        DEBUG_LOG("starting to update user_host_priv info[%u: %s]", port, metric_name);
        if (0 != update_user_host_priv_info(p_instance))
        {
            ERROR_LOG("get instance %u's user host priv info failed", port);
            return -1;
        }
    } 
    else if (!strcmp(metric_name, "innodb_deadlock_event")) 
    {
        DEBUG_LOG("starting to update innodb_deadlock info[%u: %s]", port, metric_name);
        if (0 != update_innodb_deadlock_info(p_instance))
        {
            ERROR_LOG("get instance %u's innodb deadlock info failed", port);
            return -1;
        }
    } 
    else if (!strcmp(metric_name, "exec_master_log_pos") ||
            !strcmp(metric_name, "master_host") || 
            !strcmp(metric_name, "master_log_file") || 
            !strcmp(metric_name, "master_port") || 
            !strcmp(metric_name, "read_master_log_pos") || 
            !strcmp(metric_name, "relay_log_space") || 
            !strcmp(metric_name, "seconds_behind_master") || 
            !strcmp(metric_name, "slave_io_running") || 
            !strcmp(metric_name, "slave_sql_running"))
    {
        DEBUG_LOG("starting get slave metrics[%u: %s]", port, metric_name);
        if (0 != update_slave_metric(p_instance))
        {
            ERROR_LOG("update slave instance %u's metrics failed", port);
            return -1;
        }
    }
    else
    {
        DEBUG_LOG("starting to update common metrics info[%u: %s]", port, metric_name);
        if (0 != update_common_metric_data(p_instance))
        {
            ERROR_LOG("get instance %u's common metric failed", port);
            return -1;
        }
    }


    if (p_metric->m_new)
    {
        *p_v = p_metric->m_value;
        p_metric->m_new = false;
        return 0;
    }
    else
    {
        return -1;
    }

}


void refine_value_string(c_metric * p_metric, const char ** p_value)
{
    if (ITL_VALUE_STRING != p_metric->m_type)
    {
        // 是数值类型
        if (0 == strncasecmp(*p_value, "on", strlen("on")))
        {
            *p_value = "1";
        }
        else if (0 == strncasecmp(*p_value, "off", strlen("off")))
        {
            *p_value = "0";
        }
        else if (0 == strncasecmp(*p_value, "yes", strlen("yes")))
        {
            *p_value = "1";
        }
        else if (0 == strncasecmp(*p_value, "no", strlen("no")))
        {
            *p_value = "0";
        }
        else if (0 == strncasecmp(*p_value, "null", strlen("null")))
        {
            *p_value = "0";
        }
        else if (0 == strncasecmp(*p_value, "none", strlen("none")))
        {
            *p_value = "0";
        }
    }
}


int update_common_metric_data(c_instance * p_instance)
{
    if (NULL == p_instance)
    {
        return -1;
    }


    const char * show_sql = 
        "SHOW GLOBAL STATUS WHERE \
        variable_name='binlog_cache_disk_use'\
        OR variable_name='binlog_cache_use'\
        OR variable_name='bytes_received'\
        OR variable_name='bytes_sent'\
        OR variable_name='com_call_procedure'\
        OR variable_name='com_delete'\
        OR variable_name='com_insert'\
        OR variable_name='com_select'\
        OR variable_name='com_update'\
        OR variable_name='created_tmp_disk_tables'\
        OR variable_name='created_tmp_files'\
        OR variable_name='created_tmp_tables'\
        OR variable_name='have_query_cache'\
        OR variable_name='innodb_buffer_pool_pages_data'\
        OR variable_name='innodb_buffer_pool_pages_dirty'\
        OR variable_name='innodb_buffer_pool_pages_free'\
        OR variable_name='innodb_buffer_pool_pages_total'\
        OR variable_name='innodb_data_fsyncs'\
        OR variable_name='innodb_data_pending_fsyncs'\
        OR variable_name='innodb_data_reads'\
        OR variable_name='innodb_data_writes'\
        OR variable_name='long_query_time'\
        OR variable_name='open_files'\
        OR variable_name='open_tables'\
        OR variable_name='opened_tables'\
        OR variable_name='qcache_hits'\
        OR variable_name='query_cache_type'\
        OR variable_name='questions'\
        OR variable_name='slave_open_temp_tables'\
        OR variable_name='slow_launch_threads'\
        OR variable_name='slow_queries'\
        OR variable_name='sort_range'\
        OR variable_name='sort_rows'\
        OR variable_name='sort_scan'\
        OR variable_name='table_locks_immediate'\
        OR variable_name='table_locks_waited'\
        OR variable_name='threads_cached'\
        OR variable_name='threads_connected'\
        OR variable_name='threads_created'\
        OR variable_name='threads_running'\
        OR variable_name='uptime'\
        OR variable_name='uptime_since_flush_status'\
        OR variable_name='key_read_requests'\
        OR variable_name='key_reads'\
        OR variable_name='key_write_requests'\
        OR variable_name='key_writes';\
        SHOW GLOBAL VARIABLES WHERE \
        variable_name='max_connections'\
        OR variable_name='version'\
        OR variable_name='protocol_version'\
        OR variable_name='socket'\
        OR variable_name='have_query_cache'\
        OR variable_name='query_cache_type'\
        OR variable_name='qcache_hits'\
        OR variable_name='long_query_time'\
        OR variable_name='log_slow_queries';";

    MYSQL * conn = p_instance->m_conn;
    if (0 != mysql_query(conn, show_sql))
    {
        ERROR_LOG("execute sql[%s] failed[%s]", 
                show_sql, mysql_error(conn));
        return -1;
    }

    DEBUG_LOG("begin to update common metrics");
    do
    {
        MYSQL_RES * res = mysql_store_result(conn);
        if (NULL == res)
        {
            ERROR_LOG("mysql_store_result failed[%s]", mysql_error(conn));
            return -1;
        }

        c_mysql_res_auto_ptr_t mysql_auto_ptr(res);

        MYSQL_ROW row;
        while (NULL != (row = mysql_fetch_row(res)))
        {
            const char * metric_name = row[0];
            if (NULL == metric_name)
            {
                continue;
            }
            const char * value_string = (NULL == row[1] ? "" : row[1]);

            c_metric * p_metric = find_metric(p_instance, metric_name);
            if (NULL == p_metric)
            {
                continue;
            }

            p_metric->m_new = true;

            refine_value_string(p_metric, &value_string);

            update_metric_value(p_metric, value_string);
        }
    }
    while (0 == (mysql_next_result(conn)));

    // calculate metric
    process_calculate_metric(p_instance);

    return 0;
}



int update_processlist_info(c_instance * p_instance)
{
    if (NULL == p_instance)
    {
        return -1;
    }


    const char * show_sql = "SHOW FULL PROCESSLIST";

    MYSQL * conn = p_instance->m_conn;
    if (0 != mysql_query(conn, show_sql))
    {
        ERROR_LOG("execute sql[%s] failed[%s]", 
                show_sql, mysql_error(conn));
        return -1;
    }

    DEBUG_LOG("begin to update processlist info metrics");
    MYSQL_RES * res = mysql_store_result(conn);
    if (NULL == res)
    {
        ERROR_LOG("mysql_store_result failed[%s]", mysql_error(conn));
        return -1;
    }

    c_mysql_res_auto_ptr_t mysql_auto_ptr(res);
    my_ulonglong row_count = mysql_num_rows(res);

    c_metric * p_max_used_connections = find_metric(p_instance, "max_used_connections");
    if (NULL != p_max_used_connections)
    {
        p_max_used_connections->m_new = true;
        uint64_t count = row_count;
        p_max_used_connections->m_value = count;
    }
    c_metric * p_have_locked_processlist = find_metric(p_instance, "have_locked_processlist");
    if (NULL != p_have_locked_processlist)
    {
        p_have_locked_processlist->m_new = true;
        p_have_locked_processlist->m_value.reset();
    }
    c_metric * p_last_locked_processlist = find_metric(p_instance, "last_locked_processlist");
    if (NULL != p_last_locked_processlist)
    {
        p_last_locked_processlist->m_new = true;
        p_last_locked_processlist->m_value.reset();
    }
    MYSQL_ROW row;
    int idx_id = 0;
    int idx_user = 1;
    int idx_host = 2;
    int idx_time = 5;
    int idx_state = 6;
    int idx_info = 7;
    while (NULL != (row = mysql_fetch_row(res)))
    {
        const char * state = row[idx_state];
        const char * time = row[idx_time];
        if (state && time
                && !strcasecmp(state, "locked") && strcasecmp(time, "0"))
        {
            if (NULL != p_have_locked_processlist)
            {
                p_have_locked_processlist->m_value = 1;

            }

            if (NULL != p_last_locked_processlist)
            {
                p_last_locked_processlist->m_value.sprintf(
                        "%s;%s;%s;%s;%s", 
                        row[idx_id],
                        row[idx_user],
                        row[idx_host],
                        row[idx_time],
                        row[idx_info]);
            }

            break;
        }
    }

    return 0;
}




int update_error_log_info(c_instance * p_instance)
{
    if (NULL == p_instance)
    {
        return -1;
    }


    char error_log[PATH_MAX] = {"/var/log/mysql/mysql.err"};
    const char * show_sql = "SHOW GLOBAL VARIABLES WHERE variable_name IN ('datadir', 'log_error');";

    MYSQL * conn = p_instance->m_conn;
    if (0 != mysql_query(conn, show_sql))
    {
        ERROR_LOG("execute sql[%s] failed[%s]", 
                show_sql, mysql_error(conn));
        return -1;
    }

    DEBUG_LOG("begin to update processlist info metrics");
    MYSQL_RES * res = mysql_store_result(conn);
    if (NULL == res)
    {
        ERROR_LOG("mysql_store_result failed[%s]", mysql_error(conn));
        return -1;
    }
    if (mysql_num_rows(res) < 2)
    {
        ERROR_LOG("the number of rows < 2, error: %s", mysql_error(conn));
        return -1;
    }

    c_mysql_res_auto_ptr_t mysql_auto_ptr(res);

    c_metric * p_error_log_access = find_metric(p_instance, "error_log_access");
    if (NULL != p_error_log_access)
    {
        p_error_log_access->m_new = true;
        p_error_log_access->m_value.reset();
    }
    c_metric * p_error_log_size = find_metric(p_instance, "error_log_size");
    if (NULL != p_error_log_size)
    {
        p_error_log_size->m_new = true;
        p_error_log_size->m_value.reset();
    }
    c_metric * p_last_log_error = find_metric(p_instance, "last_log_error");
    if (NULL != p_last_log_error)
    {
        p_last_log_error->m_new = true;
        p_last_log_error->m_value.reset();
    }

    MYSQL_ROW row[2] = {NULL};
    row[0] = mysql_fetch_row(res);
    row[1] = mysql_fetch_row(res);

    if (row[0] && row[1] && row[0][1] && row[1][1]
            && strlen(row[0][1]) > 0 && strlen(row[1][1]) > 0) 
    {
        if (row[1][1][0] == '.' && row[1][1][1] == '/')
        {
            snprintf(error_log, sizeof(error_log), "%s/%s", row[0][1], row[1][1] + 2);
        }
        else if (row[1][1][0] == '/')
        {
            snprintf(error_log, sizeof(error_log), "%s", row[1][1]);
        }
        else
        {
            snprintf(error_log, sizeof(error_log), "%s/%s", row[0][1], row[1][1]);
        }
    }

    // 以上获取到了error_log文件路径

    FILE * fp = fopen(error_log, "r");
    if (NULL == fp)
    {
        ERROR_LOG("open file[%s] failed, error: %s", error_log, strerror(errno));
        if (ENOENT == errno)
        {
            // 文件不存在
            p_error_log_access->m_value = 3;
        }
        else if (EACCES == errno)
        {
            // error_log文件存在但无法读取
            p_error_log_access->m_value = 2;
        }
        else
        {
            // 其他问题
            p_error_log_access->m_value = 4;
        }
        return 0;
    }
    else
    {
        p_error_log_access->m_value = 1;
    }

    int ret = fseek(fp, 0, SEEK_END);
    if (-1 == ret)
    {
        return -1;
    }

    p_error_log_size->m_value = ftell(fp);
    rewind(fp);

    uint32_t last_line_no = p_last_log_error->m_last_value.get_uint32_t();
    char line_buf[1024] = {0};
    char * end_line = NULL;
    struct tm tm;
    uint32_t line_no = 1;
    while (fgets(line_buf, sizeof(line_buf) - 1, fp))
    {
        if (NULL != (end_line = index(line_buf, '\n')))
        {
            *end_line = 0;
        }

        if (line_no > last_line_no 
        && (strcasestr(line_buf, "errcode") || strcasestr(line_buf, "error")))
        {
            strptime(line_buf, "%y%m%d %H:%M:%S", &tm);
            uint32_t time = mktime(&tm);
            p_last_log_error->m_value.sprintf(
                    "%u:%u:%s", 
                    line_no,
                    time, 
                    line_buf + 16);
            p_last_log_error->m_last_value = line_no;
            // 只处理一次
            break;
        }

        line_no++;
    }
    fclose(fp);

    return 0;
}



int update_user_host_priv_info(c_instance * p_instance)
{
    if (NULL == p_instance)
    {
        return -1;
    }


    const char * show_sql = "SELECT DISTINCT GRANTEE, IS_GRANTABLE FROM USER_PRIVILEGES;";

    MYSQL * conn = p_instance->m_conn;
    if (0 != mysql_query(conn, show_sql))
    {
        ERROR_LOG("execute sql[%s] failed[%s]", 
                show_sql, mysql_error(conn));
        return -1;
    }

    DEBUG_LOG("begin to update processlist info metrics");
    MYSQL_RES * res = mysql_store_result(conn);
    if (NULL == res)
    {
        ERROR_LOG("mysql_store_result failed[%s]", mysql_error(conn));
        return -1;
    }

    c_mysql_res_auto_ptr_t mysql_auto_ptr(res);

    c_metric * p_user_host_priv = find_metric(p_instance, "user_host_priv");
    if (NULL != p_user_host_priv)
    {
        p_user_host_priv->m_new = true;
        p_user_host_priv->m_value.reset();
    }
    c_metric * p_have_user_priv_host_wildcard = find_metric(p_instance, "have_user_priv_host_wildcard");
    if (NULL != p_have_user_priv_host_wildcard)
    {
        p_have_user_priv_host_wildcard->m_new = true;
        p_have_user_priv_host_wildcard->m_value.reset();
    }
    c_metric * p_have_user_priv_root_grant = find_metric(p_instance, "have_user_priv_root_grant");
    if (NULL != p_have_user_priv_root_grant)
    {
        p_have_user_priv_root_grant->m_new = true;
        p_have_user_priv_root_grant->m_value.reset();
    }
    MYSQL_ROW row;
    uint32_t count = 0;
    while (NULL != (row = mysql_fetch_row(res)))
    {
        if (NULL == row[0] || NULL == row[1])
        {
            continue;
        }

        if (strcasestr(row[0], "'@%'"))
        {
            p_have_user_priv_host_wildcard->m_value = 1;
        }

        if (NULL == strcasestr(row[0], "'root'@")
                && ! strcasestr(row[1], "YES"))
        {
            p_have_user_priv_root_grant->m_value = 1;

        }

        p_user_host_priv->m_value.sprintf(
            "%s%s%s", 
            p_user_host_priv->m_value.get_string(), 
            count++ ? ";" : "", row[0]);
    }

    return 0;
}



int update_innodb_deadlock_info(c_instance * p_instance)
{

    if (NULL == p_instance)
    {
        return -1;
    }


    const char * show_sql = "SHOW ENGINE INNODB STATUS;";

    MYSQL * conn = p_instance->m_conn;
    if (0 != mysql_query(conn, show_sql))
    {
        ERROR_LOG("execute sql[%s] failed[%s]", 
                show_sql, mysql_error(conn));
        return -1;
    }

    DEBUG_LOG("begin to update processlist info metrics");
    MYSQL_RES * res = mysql_store_result(conn);
    if (NULL == res)
    {
        ERROR_LOG("mysql_store_result failed[%s]", mysql_error(conn));
        return -1;
    }

    c_mysql_res_auto_ptr_t mysql_auto_ptr(res);

    if (1 != mysql_num_rows(res))
    {
        ERROR_LOG("mysql_num_rows failed[%s]", mysql_error(conn));
        return -1;
    }

    c_metric * p_innodb_deadlock_event = find_metric(p_instance, "innodb_deadlock_event");
    if (NULL != p_innodb_deadlock_event)
    {
        p_innodb_deadlock_event->m_new = true;
        p_innodb_deadlock_event->m_value.reset();
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (NULL == row || NULL == row[0])
    {
        ERROR_LOG("mysql_fetch_row failed[%s]", mysql_error(conn));
        return -1;
    }

    const char * begin = strstr(row[0], "LATEST DETECTED DEADLOCK");
    if (NULL == begin)
    {
        // 没有死锁
        return 0;
    }

    const char * line = begin;
    for (uint32_t i = 0; i < 2; i++)
    {
        line = move_to_next_line(line);
        if (NULL == line)
        {
            ERROR_LOG("sql[%s] output invalid:\n%s", show_sql, row[0]);
            return -1;
        }
    }

    struct tm tm;
    strptime(line, "%y%m%d %H:%M:%S", &tm);
    time_t time = mktime(&tm);
    p_innodb_deadlock_event->m_value.sprintf("%ld", time == -1 ? 0 : time);

    for (uint32_t i = 1; i <= 2; i++)
    {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "*** (%u) TRANSACTION", i);

        const char * transaction = strstr(line, buf);
        if (NULL == transaction)
        {
            continue;
        }

        const char * line_start = transaction;
        for (uint32_t j = 0; j < 5; j++)
        {
            line_start = move_to_next_line(line_start);
            if (NULL == line_start)
            {
                break;
            }
        }

        char line_buf[1024] = {0};
        if (NULL == get_line(line_start, line_buf, sizeof(line_buf) - 1))
        {
            continue;
        }

        p_innodb_deadlock_event->m_value.sprintf(
            "%s:%s", 
            p_innodb_deadlock_event->m_value.get_string(), 
            line_buf);

    }
    return 0;
}


int update_slave_metric(c_instance * p_instance)
{

    if (NULL == p_instance)
    {
        return -1;
    }


    const char * show_sql = "SHOW SLAVE STATUS;";

    MYSQL * conn = p_instance->m_conn;
    if (0 != mysql_query(conn, show_sql))
    {
        ERROR_LOG("execute sql[%s] failed[%s]", 
                show_sql, mysql_error(conn));
        return -1;
    }

    DEBUG_LOG("begin to update processlist info metrics");
    MYSQL_RES * res = mysql_store_result(conn);
    if (NULL == res)
    {
        ERROR_LOG("mysql_store_result failed[%s]", mysql_error(conn));
        return -1;
    }

    c_mysql_res_auto_ptr_t mysql_auto_ptr(res);

    my_ulonglong row_count = mysql_num_rows(res);
    if (1 != row_count)
    {
        ERROR_LOG("mysql_num_rows failed[%s], count: %llu", mysql_error(conn), row_count);
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (NULL == row)
    {
        ERROR_LOG("mysql_fetch_row failed[%s]", mysql_error(conn));
        return -1;
    }
    uint32_t field_count = 0;
    MYSQL_FIELD * field = NULL;
    while (NULL != (field = mysql_fetch_field(res)))
    {
        const char * metric_name = field->name;
        c_metric * p_metric = find_metric(p_instance, metric_name);
        if (NULL == p_metric)
        {
            field_count++;
            continue;
        }
        p_metric->m_new = true;
        const char * value_string = row[field_count];
        if (0 == strcasecmp(metric_name, "slave_io_running") 
                || 0 == strcasecmp(metric_name, "slave_sql_running"))
        {
            value_string = (0 == strcasecmp(value_string, "yes")) ? "1" : "2";
        }

        update_metric_value(p_metric, value_string);
        field_count++;
    }

    return 0;
}


int update_metric_value(c_metric * p_metric, const char * value_string)
{
    if (NULL == value_string)
    {
        value_string = "";
    }

    c_value v(p_metric->m_value.get_type());
    switch (p_metric->m_type)
    {
        case ITL_VALUE_STRING:
            p_metric->m_value = value_string;
            return 0;
        default:
            v.set_by_string(value_string);
            break;
    }

#define diff    (p_metric->m_value)
#define last    (p_metric->m_last_value)

#define PROCESS_VALUE(type, max) \
    do { \
        if (SLOPE_DIFF == p_metric->m_slope) { \
            uint32_t now = time(NULL); \
            if (p_metric->m_initial) { \
                diff = 0; \
                p_metric->m_initial = false; \
            } else { \
                uint32_t last_collect_time = p_metric->m_last_collect_time; \
                if (now > last_collect_time) { \
                    uint32_t interval = now - last_collect_time; \
                    if ((v.get_ ## type)() >= (last.get_ ## type)()) { \
                        diff = 1.0f * ((v.get_ ## type)() - (last.get_ ## type)()) / interval; \
                    } else { \
                        diff = 1.0f * ((v.get_ ## type)() + max - (last.get_ ## type)()) / interval; \
                    } \
                } \
            } \
            p_metric->m_last_collect_time = now; \
        } else { \
            diff = v; \
        } \
        last = v; \
    } while (0)


    switch (p_metric->m_type)
    {
        case ITL_VALUE_INT32:
            PROCESS_VALUE(int32_t, MAX_INT32);
            break;
        case ITL_VALUE_UINT32:
            PROCESS_VALUE(uint32_t, MAX_UINT32);
            break;
        case ITL_VALUE_INT64:
            PROCESS_VALUE(int64_t, MAX_INT64);
            break;
        case ITL_VALUE_UINT64:
            PROCESS_VALUE(uint64_t, MAX_UINT64);
            break;
        case ITL_VALUE_FLOAT:
            PROCESS_VALUE(float, (float)MAX_INT32);
            break;
        case ITL_VALUE_DOUBLE:
            PROCESS_VALUE(double, (double)MAX_UINT64);
            break;

        default:
            break;
    }

#undef PROCESS_VALUE
#undef diff
#undef last
    return 0;

}



int process_calculate_metric(c_instance * p_instance)
{

    c_metric * p_rate = find_metric(p_instance, "key_buffer_read_hit_rate");
    if (NULL != p_rate)
    {
        // 要求采集这个

        c_metric * p_req = find_metric(p_instance, "key_read_requests");
        if (NULL == p_req)
        {
            return -1;
        }

        c_metric * p_read = find_metric(p_instance, "key_reads");
        if (NULL == p_read)
        {
            return -1;
        }

        p_rate->m_new = true;
        if (0 == p_req->m_value.get_double())
        {
            p_rate->m_value = 0;
        }
        else
        {
            p_rate->m_value = 100 * (1.0 - p_read->m_value.get_double() / p_req->m_value.get_double());
        }


    }


    p_rate = find_metric(p_instance, "key_buffer_write_hit_rate");
    if (NULL != p_rate)
    {
        // 要求采集这个

        c_metric * p_req = find_metric(p_instance, "key_write_requests");
        if (NULL == p_req)
        {
            return -1;
        }

        c_metric * p_write = find_metric(p_instance, "key_writes");
        if (NULL == p_write)
        {
            return -1;
        }

        p_rate->m_new = true;
        if (0 == p_req->m_value.get_double())
        {
            p_rate->m_value = 0;
        }
        else
        {
            p_rate->m_value = 100 * (1.0 - p_write->m_value.get_double() / p_req->m_value.get_double());
        }
    }

    return 0;
}
