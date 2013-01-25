/** 
 * ========================================================================
 * @file db_interface.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-17
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <assert.h>
#include <libtaomee/log.h>

#include "c_mysql_iface.h"
#include "itl_common.h"
#include "db_interface.h"

using namespace std;


//================================Global Variable================================
c_mysql_iface * g_db = &(SINGLETON(c_mysql_iface));

char g_sql_buff[SQL_BUFF_LEN] = {0};

uint8_t g_is_log_sql = config_get_intval("IS_LOG_SQL", 0);
extern log_node_t g_log_node;


enum {
    SWITCH_CPU_USED = 0,
    SWITCH_MEM_FREE,
    SWITCH_INTERFACE_INFO,
    SWITCH_INTERFACE_IN,
    SWITCH_INTERFACE_OUT
};
static const char *g_switch_metric_list[] = {
        "switch_cpu_used",
        "switch_mem_free",
        "switch_interface_info",
        "switch_interface_in",
        "switch_interface_out"
};
//===============================================================================

int get_node_info_by_ip();

int init_db(const char * host, uint16_t port, const char * db_name, const char * user, const char * password)
{
    if (0 != g_db->init(host, port, db_name, user, password, "utf8"))
    {
        ERROR_LOG("%s", g_db->get_last_errstr());
        return -1;
    }

    // close autocommit
    if (0 != mysql_autocommit(g_db->get_conn(), 0))
    {
        ERROR_LOG("close autocommit failed");
        return -1;
    }

    get_node_info_by_ip();

    return 0;
}



int fini_db()
{
    return g_db->uninit();
}


int rollback()
{
    return mysql_rollback(g_db->get_conn());
}


int commit()
{
    return mysql_commit(g_db->get_conn());
}


int get_node_info_by_ip()
{
    int idx_node_id = 0, idx_node_tag = 1;
    GEN_SQLSTR(g_sql_buff,
            "SELECT server_id, server_tag FROM t_server_info WHERE ip_inside = '%s'",
            g_log_node.node_ip);
    MYSQL_ROW row;
    g_db->select_first_row(&row, g_sql_buff);
    if (NULL != row)
    {
        g_log_node.node_id = atoi(row[idx_node_id]);
        ITL_STRCPY(g_log_node.node_tag, row[idx_node_tag]);
    }

    return 0;
}

int db_store_collect_data(const db_name_t *p_host, const db_name_t *p_metric, const db_name_t *p_arg, char *p_val, uint32_t value_len)
{
    if (NULL == p_host || NULL == p_metric || NULL == p_arg || NULL == p_val)
    {
        ERROR_LOG("Parameter cannot be NULL.");
        return -1;
    }
    //GEN_SQLSTR(g_sql_buff,
    //        //"SELECT metric_type FROM t_metric_info WHERE metric_id = %u AND metric_name = '%s'",
    //        "SELECT metric_type FROM t_metric_info WHERE metric_name = '%s'", p_metric->name);
    //MYSQL_ROW row;
    //int row_num = g_db->select_first_row(&row, g_sql_buff);
    //if (row_num != 1)
    //{
    //    ERROR_LOG("wrong metric[%s] record num[%d != 1], SQL: %s", p_metric->name, row_num, g_sql_buff);
    //    return -1;
    //}
    //int metric_type = atoi(row[0]);

    int ret = 0;
    if (0 == strcmp(p_metric->name, g_switch_metric_list[SWITCH_INTERFACE_INFO]))
    {//交换机接口信息特殊处理
        //if (p_host->id == 66)
        //{
        //    INFO_LOG("switch[%s] metric[%s] value[%s]", p_host->name, p_metric->name, p_val);
        //}
        //格式: if_num,if_status,if_type,if_speed,if_desc;
        char *read_pos = p_val;
        char *semi_pos = NULL;
        char *p_if_num = p_val;
        char *p_if_status = NULL;
        char *p_if_type = NULL;
        char *p_if_speed = NULL;
        char *p_if_desc = NULL;
        while (NULL != read_pos && '\0' != *read_pos)
        {
            semi_pos = index(read_pos, ';');
            if (NULL != semi_pos)
            {
                *semi_pos = '\0';
            }
            GET_IF_COMMA_INFO(read_pos, p_if_num);
            GET_IF_COMMA_INFO(read_pos, p_if_status);
            GET_IF_COMMA_INFO(read_pos, p_if_type);
            GET_IF_COMMA_INFO(read_pos, p_if_speed);
            GET_IF_COMMA_INFO(read_pos, p_if_desc);
            read_pos = (NULL == semi_pos) ? NULL : semi_pos + 1;

            if (NULL == p_if_desc)
            {
                continue;
            }
            //int if_speed = atoi(p_if_speed) / IF_SPEED_BASE;
            int if_speed = atoi(p_if_speed);
            time_t now = time(NULL);
            GEN_SQLSTR(g_sql_buff,
                    "INSERT INTO t_switch_interface_info SET switch_id = %u, if_idx='%s', if_status = '%s', if_type='%s', if_speed=%d, if_desc = '%s', add_time = %zu ON DUPLICATE KEY UPDATE if_status = '%s', if_type='%s', if_speed=%d, if_desc = '%s'", p_host->id, p_if_num, p_if_status, p_if_type, if_speed, p_if_desc, now,
                    p_if_status, p_if_type, if_speed, p_if_desc);
            STD_EXECSQL(g_db, g_sql_buff, ret);
        }
        return ret;
    }

    static char metric_value[SQL_BUFF_LEN];
    mysql_real_escape_string(g_db->get_conn(), metric_value, p_val, value_len);
#ifdef USE_NEW_DB_ITL
    GEN_SQLSTR(g_sql_buff,
        "INSERT INTO db_itl.t_metric_value SET host_id=%u, metric_id=%u, metric_arg='%s', metric_val=\'%s\', last_update_time=NOW() ON DUPLICATE KEY UPDATE metric_val=\'%s\', last_update_time=NOW()",
                p_host->id, p_metric->id, p_arg->name, metric_value, metric_value);
#else
    GEN_SQLSTR(g_sql_buff,
        "INSERT INTO db_itl.t_host_info SET host_name='%s', metric_name='%s', metric_arg='%s', metric_val=\'%s\', last_update_time=NOW() ON DUPLICATE KEY UPDATE metric_val=\'%s\', last_update_time=NOW()",
        p_host->name, p_metric->name, p_arg->name, metric_value, metric_value);
#endif

    if (mysql_real_query(g_db->get_conn(), g_sql_buff, strlen(g_sql_buff)) != 0)
    {
        ERROR_LOG("sql[%s] exec failed[%s]", g_sql_buff, mysql_error(g_db->get_conn()));
        ret = DB_ERR;
        return ret;
    }
    else
    {
        ret = DB_SUCC;
    }

#ifndef USE_NEW_DB_ITL
    uint32_t now = time(NULL);
    GEN_SQLSTR(g_sql_buff,
            "INSERT INTO db_itl.t_host_info SET host_name = '%s', metric_name = 'last_report', metric_val = %u, last_update_time = NOW() ON DUPLICATE KEY UPDATE metric_val = %u, last_update_time = NOW()",
            p_host->name, now, now);
    STD_EXECSQL(g_db, g_sql_buff, ret);
#endif



    return ret;
}


int db_store_alarm_event_data(const db_p_store_alarm_event_data_in * p_in)
{
    DEBUG_LOG("Begin to run  db_store_alarm_event_data");
    
    assert(NULL != p_in);
    
    const char * alarm_host = p_in->alarm_host;
    const uint32_t host_type = p_in->host_type;
    const db_name_t * p_alarm_arg = &p_in->alarm_arg; 
    const uint32_t  alarm_way = p_in->alarm_way;
    const db_name_t * p_alarm_metric = &p_in->alarm_metric;

    const char * alarm_type = get_alarm_type(p_in->alarm_type);
    const char * alarm_content = p_in->alarm_content;

    const uint32_t start = p_in->alarm_start;
    const uint32_t end = p_in->alarm_end;
    
    char key_str[ITL_MAX_STR_LEN];

    snprintf(key_str, ITL_MAX_STR_LEN, "%s%s%s%d", 
            alarm_host, p_alarm_arg->name, p_alarm_metric->name, start);
    DEBUG_LOG("key_str:%s", key_str);


    const char *format = "INSERT INTO db_itl.t_alarm_event_info SET alarm_key=md5('%s'), alarm_start=%u, alarm_end=%u, alarm_host='%s', host_type=%u, alarm_type='%s', alarm_metric='%s', alarm_way=%u, alarm_content='%s', alarm_arg='%s' ON DUPLICATE KEY UPDATE alarm_end=%u";
    
    GEN_SQLSTR(g_sql_buff, format,
                key_str, 
                start, end, 
                alarm_host, host_type, 
                alarm_type, p_alarm_metric->name,
                alarm_way, alarm_content,
                p_alarm_arg->name,
                end);

    DEBUG_LOG("sql_buff: %s", g_sql_buff);
    int ret = 0;
    STD_EXECSQL(g_db, g_sql_buff, ret);

    DEBUG_LOG("End to run db_store_alarm_event_data");
    return ret;
}


char* get_alarm_type(const uint32_t type)
{
    static char alarm_type[ITL_ALARM_TYPE_STR_LEN];   
    
    switch(type)
    {
        case ALARM_WARN:
            ITL_STRCPY(alarm_type, "warning");
            break;
        case ALARM_CRITICAL:
            ITL_STRCPY(alarm_type, "critical");
            break;
        case ALARM_HOSTDOWN :
            ITL_STRCPY(alarm_type, "host_down");
            break;
        case ALARM_UPFAILED:
            ITL_STRCPY(alarm_type, "up_failed");
            break;
        default:
            break;
    }

    return alarm_type;
}

/** 
 * @brief 保存发出去的告警消息到数据库
 * 
 * @param p_in
 * 
 * @return 
 */
int db_store_alarm_message_data(const db_p_store_alarm_message_data_in *p_in)
{
    DEBUG_LOG("Begin to run  db_store_alarm_message_data");
    assert(NULL != p_in); 

    const char * p_host = p_in->host_ip;
    uint32_t host_type = p_in->host_type;
    const db_name_t * p_alarm_metric = &p_in->alarm_metric;
    const db_name_t * p_alarm_arg = &p_in->alarm_arg; 
    
    const uint32_t  alarm_level = p_in->alarm_level;
    const uint32_t  alarm_way = p_in->alarm_way;
    
    const alarm_contact_t  * p_alarm_contact = &p_in->alarm_contact;
    const uint32_t alarm_content_len = p_in->_alarm_content_len;
    static char alarm_content[SQL_BUFF_LEN];
    mysql_real_escape_string(g_db->get_conn(), alarm_content, p_in->alarm_content, strlen(p_in->alarm_content));

    char * alarm_type = get_alarm_type(alarm_level);
    DEBUG_LOG("host[%s] metric[%u:%s] arg[%u:%s] level[%d] way[%d] content[%d:%s]",
            p_host,
            p_alarm_metric->id, p_alarm_metric->name,
            p_alarm_arg->id, p_alarm_arg->name, 
            alarm_level,
            alarm_way, 
            alarm_content_len, alarm_content
            );                                       

    DEBUG_LOG("alarm_contact:mobile[%s]  email[%s] rtx[%s]",
            p_alarm_contact->mobile_list,
            p_alarm_contact->email_list,
            p_alarm_contact->rtx_list);

    const char *format = "INSERT INTO db_itl.t_alarm_log_info SET alarm_host='%s', alarm_metric='%s', alarm_arg='%s', alarm_time=UNIX_TIMESTAMP(NOW()), host_type=%u, alarm_type='%s', alarm_way=%u, alarm_mail_list='%s', alarm_rtx_list='%s', alarm_msg_list='%s', alarm_content='%s'";


    GEN_SQLSTR(g_sql_buff, format, 
            p_host,  p_alarm_metric->name, p_alarm_arg->name,
            host_type, alarm_type, alarm_way, 
            p_alarm_contact->email_list,
            p_alarm_contact->rtx_list, 
            p_alarm_contact->mobile_list,
            alarm_content);

    int ret = 0;
    STD_EXECSQL(g_db, g_sql_buff, ret);
  
    DEBUG_LOG("End to run db_store_alarm_message_data");

    return ret;
}


/** 
 * @brief 获取metric_group信息
 * 
 * @param service_id    服务ID: 如t_db_info中db_id, 0-表示所有
 * @param service_type  服务类型: 1:服务器硬件 2:交换机 3:mysql 4:图片服务器
 * @param group_confs   组配置vector
 * @param so_list       保存返回的so名称字符串
 * @param so_len: so_list字符串长度
 * @param so_num: so文件的个数
 * 
 * @return 
 */
int db_get_metric_group_info(
        uint32_t service_id,
        uint32_t service_type,
        vector<group_config_t> &group_confs,
        char *so_list, uint32_t so_len, uint32_t &so_num)
{
    assert(NULL != so_list);

    int ret_code = DB_SUCC;
    ///获取metric组信息
    MYSQL_ROW row;
    int idx_group_id = 0, idx_group_name = 1, idx_collect_interval = 2;
    if (service_id > 0) 
    {
        GEN_SQLSTR(g_sql_buff, "SELECT DISTINCT TMGI.metric_group_id AS group_id, TMGI.metric_group_name AS group_name, TMGI.time_interval AS collect_interval FROM t_metric_group_service AS TMGS INNER JOIN t_metric_group_info AS TMGI ON TMGS.metric_group_id = TMGI.metric_group_id WHERE TMGS.service_id = %u AND TMGS.service_type = %u", service_id, service_type);
    }
    else
    {
        GEN_SQLSTR(g_sql_buff, "SELECT DISTINCT TMGI.metric_group_id AS group_id, TMGI.metric_group_name AS group_name, TMGI.time_interval AS collect_interval FROM t_metric_group_service AS TMGS INNER JOIN t_metric_group_info AS TMGI ON TMGS.metric_group_id = TMGI.metric_group_id WHERE TMGS.service_type = %u", service_type);
    }
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("Get metric_group info[%s]", g_sql_buff);
#endif

    int row_num = g_db->select_first_row(&row, g_sql_buff);
    if (row_num <= 0 || NULL == row)
    {
        ERROR_LOG("Database ERROR: no metric group info in SQL[%s]", g_sql_buff);
        return DB_ERR;
    }
    db_group_inter_t group_inter;
    vector<db_group_inter_t> group_inter_vec;
    while (NULL != row)
    {
        group_inter.group_id = atoi(row[idx_group_id]);
        group_inter.collect_interval = atoi(row[idx_collect_interval]);
        group_inter_vec.push_back(group_inter);
        row = g_db->select_next_row();
    }
    group_confs.clear();

    //获取组中metric信息
    string so_name;
    vector<string>::iterator so_iter;
    uint32_t group_id = 0;
    group_config_t group_conf;
    metric_config_t metric_conf;
    for (uint32_t idx = 0; idx < group_inter_vec.size(); ++idx)
    {
        group_id = group_inter_vec[idx].group_id;
        group_conf.collect_interval = group_inter_vec[idx].collect_interval;
        group_conf.metric_config.clear();

        int idx_metric_id = 0, idx_metric_name = 1, idx_metric_arg1 = 2, idx_metric_arg2 = 3;
        int idx_metric_type = 4, idx_plugin_name = 5;
        if (service_id > 0)
        {
            GEN_SQLSTR(g_sql_buff, "SELECT DISTINCT TMI.metric_id AS metric_id, TMI.metric_name AS metric_name, TMI.argument AS metric_arg1, TSMA.arg AS metric_arg2, TMI.metrictype_id AS metric_type, TPI.plugin_name AS plugin_name FROM t_metric_info AS TMI LEFT JOIN (SELECT metric_name, arg FROM t_service_metric_arg WHERE service_id = %d AND service_type = %d) AS TSMA ON TMI.metric_name = TSMA.metric_name INNER JOIN t_plugin_info AS TPI ON TPI.plugin_id=TMI.plugin_id WHERE TMI.metric_group_id = %d", service_id, service_type, group_id);
        }
        else
        {
            GEN_SQLSTR(g_sql_buff, "SELECT DISTINCT TMI.metric_id AS metric_id, TMI.metric_name AS metric_name, TMI.argument AS metric_arg1, TSMA.arg AS metric_arg2, TMI.metrictype_id AS metric_type, TPI.plugin_name AS plugin_name FROM t_metric_info AS TMI LEFT JOIN (SELECT metric_name, arg FROM t_service_metric_arg WHERE service_type = %d) AS TSMA ON TMI.metric_name = TSMA.metric_name INNER JOIN t_plugin_info AS TPI ON TPI.plugin_id=TMI.plugin_id WHERE TMI.metric_group_id = %d", service_type, group_id);
        }
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("Get metric info[%s]", g_sql_buff);
#endif
        row_num = g_db->select_first_row(&row, g_sql_buff);
        if (row_num <= 0 || NULL == row)
        {
            ERROR_LOG("Database ERROR: no metric info of group[%d] in SQL[%s]", group_id, g_sql_buff);
            ret_code = DB_ERR;
            break;
        }
        while (NULL != row)
        {

            metric_conf.metric_id = atoi(row[idx_metric_id]);
            ITL_STRCPY(metric_conf.metric_name, row[idx_metric_name]);
            if (NULL != row[idx_metric_arg2] && strlen(row[idx_metric_arg2]) > 0)
            {
                ITL_STRCPY_LEN(&metric_conf, metric_arg, row[idx_metric_arg2]);
            }
            else
            {
                ITL_STRCPY_LEN(&metric_conf, metric_arg, row[idx_metric_arg1]);
            }
            metric_conf.metric_type = atoi(row[idx_metric_type]);
            group_conf.metric_config.push_back(metric_conf);
            so_name = atoi(row[idx_plugin_name]);

            if (NULL == strstr(so_list, row[idx_plugin_name]))
            {
                if (strlen(so_list) + strlen(row[idx_plugin_name]) < so_len)
                {
                    sprintf(so_list+strlen(so_list), "%s;", row[idx_plugin_name]);
                    ++so_num;
                }
                else 
                {
                    ret_code = DB_NO_BUFF;//buff not enough
                    break;
                }
            }
            row = g_db->select_next_row();
        }
        group_confs.push_back(group_conf);
    }

    group_inter_vec.clear();
    return ret_code;
}


int get_host_info(const char * host_ip, uint32_t * p_host_id, char * p_host_tag, uint32_t tag_len)
{
    int idx_host_id = 0, idx_host_tag = 1;
    const char *format = "SELECT DISTINCT server_id, server_tag FROM t_server_info WHERE ip_inside = '%s'";
    GEN_SQLSTR(g_sql_buff, format, host_ip);
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("Get server_tag info[%s]", g_sql_buff);
#endif

    MYSQL_ROW row;
    int row_num = g_db->select_first_row(&row, g_sql_buff);
    if (row_num <= 0 || NULL == row) 
    {
        ERROR_LOG("sql err: %s", g_db->get_last_errstr());
        return DB_NO_RECORD;
    }
    else if (row_num > 1)
    {
        ERROR_LOG("Database ERROR: row_num[%d != 1], SQL: %s", row_num, g_sql_buff);
        return DB_ERR;
    }

    if (NULL != p_host_id)
    {
        *p_host_id = atoi(row[idx_host_id]);
    }

    if (NULL != p_host_tag)
    {
        snprintf(p_host_tag, tag_len, "%s", row[idx_host_tag] ? row[idx_host_tag] : "");
    }

    return DB_SUCC;
}



int db_get_node_server_config(const char *host_ip, db_p_get_node_server_config_out *p_out)
{
    assert(NULL != host_ip && NULL != p_out);
    int service_type = ITL_TYPE_SERVER;

    ///获取资产信息: host_id
    uint32_t node_id = 0;
    int ret = get_host_info(host_ip, &node_id, NULL, 0);
    if (DB_SUCC != ret)
    {
        return ret;
    }
    p_out->node_id = node_id;

    uint32_t service_id = 0;
    uint8_t is_monitor = 0;
    int idx_service_id = 0;
    int idx_is_monitor = 1;
    const char *format = "SELECT DISTINCT service_id, is_monitor FROM t_service_info WHERE machine_id = %u AND service_type = %d";
    GEN_SQLSTR(g_sql_buff, format, node_id, service_type);
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("Get service_id info[%s]", g_sql_buff);
#endif

    MYSQL_ROW row;
    int row_num = g_db->select_first_row(&row, g_sql_buff);
    if (row_num <= 0 || NULL == row) 
    {
        return DB_NO_RECORD;
    }
    else if (row_num > 1)
    {
        ERROR_LOG("Database ERROR: row_num[%d != 1], SQL: %s", row_num, g_sql_buff);
        return DB_ERR;
    }

    service_id = atoi(row[idx_service_id]);

    is_monitor = atoi(row[idx_is_monitor]);

    if (!is_monitor)
    {
        // 不监控
        return DB_SUCC;
    }

    //获取node信息: listen_port, update_interval
    // int idx_up_interval = 1, idx_listen_port = 2;
    // GEN_SQLSTR(g_sql_buff, "SELECT node_id, update_interval, listen_port FROM t_node_info WHERE node_id = %d", service_id);
    // row_num = g_db->select_first_row(&row, g_sql_buff);
    // if (row_num != 1)
    // {
        // ERROR_LOG("Database ERROR: row_num[%d != 1], SQL: %s", row_num, g_sql_buff);
        // return DB_ERR;
    // }
    // p_out->listen_port = atoi(row[idx_listen_port]);
    // p_out->update_interval = atoi(row[idx_up_interval]);

    //获取: update_url, head_addr
    // if (NULL != g_p_head_ip && ITL_BETWEEN(g_head_port, ITL_MIN_PORT, ITL_MAX_PORT))
    // {
        // p_out->head_addr.port = g_head_port;
        // ITL_STRCPY(p_out->head_addr.ip, g_p_head_ip);
    // }
    // else
    // {
        // char match_ip[16] = {0};
        // ITL_STRCPY(match_ip, host_ip);
        // char *dot_pos = rindex(match_ip, '.');
        // assert(NULL != dot_pos);
        // *dot_pos = '\0';
        // int idx_head_ip = 0, idx_head_port = 1;
        // GEN_SQLSTR(g_sql_buff, "SELECT DISTINCT TNSI.ip_inside AS head_ip, TNSI.listen_port AS head_port FROM t_network_segment_info AS TNSI WHERE segment LIKE '%s%s.%s'", "%", match_ip, "%");
        // row_num = g_db->select_first_row(&row, g_sql_buff);
        // if (row_num != 1)
        // {
            // ERROR_LOG("Database ERROR: row_num[%d != 1], SQL: %s", row_num, g_sql_buff);
            // return DB_ERR;
        // }
        // p_out->head_addr.port = atoi(row[idx_head_port]);
        // ITL_STRCPY(p_out->head_addr.ip, row[idx_head_ip]);
    // }

    //获取so和group信息
    return db_get_metric_group_info(service_id, service_type, p_out->group_info,
            p_out->so_name_list, sizeof(p_out->so_name_list), p_out->so_num);
}


int db_get_node_mysql_config(const char *host_ip, db_p_get_node_mysql_config_out *p_out)
{
    assert(NULL != host_ip && NULL != p_out);

    int ret_code = DB_SUCC;
    int service_type = ITL_TYPE_MYSQL;

    ///获取资产信息: host_id
    uint32_t node_id = 0;
    int ret = get_host_info(host_ip, &node_id, NULL, 0);
    if (DB_SUCC != ret)
    {
        return ret;
    }
    p_out->node_id = node_id;

    ///获取监控service信息
    int idx_service_id = 0;
    int idx_is_monitor = 1;
    const char *format = "SELECT DISTINCT service_id, is_monitor FROM t_service_info WHERE machine_id = %u AND service_type = %d";
    GEN_SQLSTR(g_sql_buff, format, node_id, service_type);
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("Get service_id info[%s]", g_sql_buff);
#endif
    MYSQL_ROW row;
    int row_num = g_db->select_first_row(&row, g_sql_buff);
    if (row_num <= 0 || NULL == row)
    {
        // 没有mysql要监控
        return DB_SUCC;
    }

    uint32_t service_id = 0;
    vector<uint32_t> service_id_vec;
    while (NULL != row)
    {
        uint8_t is_monitor = atoi(row[idx_is_monitor]);
        if (is_monitor)
        {
            service_id = atoi(row[idx_service_id]);
            service_id_vec.push_back(service_id);

        }

        row = g_db->select_next_row();
    }

    db_node_config_t db_group;
    group_config_t group_conf;
    metric_config_t metric_conf;

    for (uint32_t idx = 0; idx < service_id_vec.size(); ++idx)
    {
        service_id = service_id_vec[idx];
        /**role角色 1:MASTER 2:SLAVE 3: Master/Slave 4:其他*/
        int idx_port = 1, idx_sock = 2, idx_role = 3;
        GEN_SQLSTR(g_sql_buff, "SELECT db_id, port, socket, role FROM t_db_info WHERE db_id = %u", service_id);
        row_num = g_db->select_first_row(&row, g_sql_buff);
        if (row_num != 1)
        {
            ERROR_LOG("Database ERROR: row_num[%d != 1], SQL: %s", row_num, g_sql_buff);
            ret_code = DB_ERR;
            break;
        }
        int db_role = atoi(row[idx_role]);
        db_group.instance_info.type = db_role;
        db_group.instance_info.port = atoi(row[idx_port]);
        ITL_STRCPY(db_group.instance_info.sock, row[idx_sock]);
        ret = db_get_metric_group_info(service_id, service_type, db_group.group_info,
                p_out->so_name_list, sizeof(p_out->so_name_list), p_out->so_num);
        if (DB_SUCC == ret_code)
        {
            p_out->db_group_info.push_back(db_group);
        }
    }

    service_id_vec.clear();
    return ret;
}

int db_get_default_alarm_conf(int service_type, alarm_default_conf_t *p_alarm_conf)
{
    assert(NULL != p_alarm_conf);

    //先设置默认告警方式
    p_alarm_conf->warning_way = 7;
    p_alarm_conf->critical_way = 7;
    ITL_STRCPY(p_alarm_conf->no_alarm_range, "");
    ITL_STRCPY(p_alarm_conf->alarm_frequency, "");
    ITL_STRCPY(p_alarm_conf->alarm_mobile, "");
    ITL_STRCPY(p_alarm_conf->alarm_email, "");
    ITL_STRCPY(p_alarm_conf->alarm_rtx, "");

    char sevice_prefix[32] = "unkown";
    switch (service_type)
    {
        case ITL_TYPE_SERVER:
            strcpy(sevice_prefix, "server_");
            break;
        case ITL_TYPE_SWITCH:
            strcpy(sevice_prefix, "switch_");
            break;
        case ITL_TYPE_MYSQL:
            strcpy(sevice_prefix, "db_");
            break;
        default:
            ERROR_LOG("Cannot support service_type[%d] now.", service_type);
            return -1;
    }

    //获取默认告警策略配置
    int idx_default_key = 0, idx_default_value = 1;
    int idx_user_name = 2, idx_phone = 3, idx_email = 4;
    GEN_SQLSTR(g_sql_buff,
            "SELECT DISTINCT TADI.default_key AS default_key, TADI.default_value AS default_value, TUI.user_name AS user_name, TUI.cellphone AS cellphone, TUI.email AS email FROM t_alarm_default_info AS TADI LEFT JOIN t_user_info AS TUI ON TADI.default_value = TUI.user_id WHERE TADI.is_default = 1 AND TADI.default_key LIKE '%s%%';", sevice_prefix);
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("Get service_type default info[%s]", g_sql_buff);
#endif
    MYSQL_ROW row;
    int row_num = g_db->select_first_row(&row, g_sql_buff);
    if (row_num <= 0 || NULL == row)
    {
        ERROR_LOG("Database ERROR: row_num: %d, DB_NO_RECORD, SQL: %s, DESC: %s",
                row_num, g_sql_buff, g_db->get_last_errstr());
        return DB_NO_RECORD;//无默认告警策略
    }

    while (NULL != row)
    {
        if (NULL != strstr(row[idx_default_key], "expr"))
        {
            ITL_STRCPY(p_alarm_conf->alarm_frequency, row[idx_default_value]);
        }
        else if (NULL != strstr(row[idx_default_key], "receiver"))
        {
            ITL_STRCAT(p_alarm_conf->alarm_mobile, row[idx_phone]);
            ITL_STRCAT(p_alarm_conf->alarm_email, row[idx_email]);
            ITL_STRCAT(p_alarm_conf->alarm_rtx, row[idx_user_name]);
            ITL_STRCAT(p_alarm_conf->alarm_mobile, ",");
            ITL_STRCAT(p_alarm_conf->alarm_email, ",");
            ITL_STRCAT(p_alarm_conf->alarm_rtx, ",");
        }
        else if (NULL != strstr(row[idx_default_key], "shield"))
        {
            ITL_STRCPY(p_alarm_conf->no_alarm_range, row[idx_default_value]);
        }
        row = g_db->select_next_row();
    }

    return 0;
}

int db_get_all_metric_id(map<string, int> &metric_id_map)
{
    metric_id_map.clear();
    int idx_metric_id = 0, idx_metric_name = 1;
    GEN_SQLSTR(g_sql_buff, "SELECT DISTINCT metric_id, metric_name FROM t_metric_info");
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("Get all metric id info[%s]", g_sql_buff);
#endif
    MYSQL_ROW row;
    string str_metric_name;
    g_db->select_first_row(&row, g_sql_buff);
    while (NULL != row)
    {
        metric_id_map[row[idx_metric_name]] = atoi(row[idx_metric_id]);
        row = g_db->select_next_row();
    }

    return 0;
}

/** 
 * @brief 将告警阀值表达式中的metric_name替换成metric_id
 * 
 * @param p_threshold_expr  告警阀值表达式
 * @param p_buff            转换后的告警表达式
 * @param buff_len
 * @param p_metric_id_map   <metric_name, metric_id>映射表
 * 
 * @return 0:succ, -1:failed
 */
int metric_name2id(char *p_threshold_expr, char *p_buff, int buff_len, map<string, int> *p_metric_id_map)
{
    assert(NULL != p_buff && buff_len > 0);
    if (NULL == p_threshold_expr)
    {
        ERROR_LOG("p_threshold_expr is NULL");
        p_buff[0] = '\0';
        return 0;
    }

    char str_buff[ITL_MAX_STR_LEN];
    map<string, int>::iterator iter;
    char *p_pos = p_threshold_expr;
    char *p_start = NULL;
    int write_len = 0;
    do
    {
        while(*p_pos != '\0' && *p_pos != '$' && write_len < buff_len)
        {
            p_buff[write_len++] = *p_pos;
            ++p_pos;
        }

        if (write_len == buff_len)
        {
            ERROR_LOG("buff_len[%d] isnot enough!", buff_len);
            return -1;
        }

        if (*p_pos == '$')
        {
            ++p_pos;//跳过$
            p_start = p_pos;
            while (*p_pos != '\0'
                    && (*p_pos == '_' || ITL_BETWEEN(*p_pos, '0', '9') ||
                        ITL_BETWEEN(*p_pos, 'a', 'z') || ITL_BETWEEN(*p_pos, 'A', 'Z')))
            {
                ++p_pos;
            }
            int str_len = ITL_MIN(p_pos - p_start + 1, (int)sizeof(str_buff));
            snprintf(str_buff, str_len, "%s", p_start);
            iter = p_metric_id_map->find(str_buff);
            if (iter == p_metric_id_map->end())
            {
                ERROR_LOG("Cannot find metric[%s] in p_metric_id_map", str_buff);
                return -1;
            }
            int len = snprintf(p_buff + write_len, buff_len - write_len, "$%d", iter->second);
            write_len += len;
        }
    } while (*p_pos != '\0');
    int end_len = ITL_MIN(write_len, buff_len - 1);
    p_buff[end_len] = '\0';

    return 0;
}


int db_get_node_alarm_config(uint32_t host_id, uint32_t service_type, db_p_get_node_alarm_config_out *p_out)
{
    assert(NULL != p_out);
    p_out->host_id = host_id;
    p_out->service_type = service_type;

    map<string, int> metric_id_map;
    db_get_all_metric_id(metric_id_map);

    char str_buff[ITL_MAX_STR_LEN];
    MYSQL_ROW row;
    int row_num = 0;
    int len = 0;
    uint32_t left_len = 0;
    char *write_pos = NULL;

    //获取机器信息
    int idx_host_id = 0, idx_host_tag = 1, idx_project_id = 2, idx_project_name = 3, idx_host_ip = 4;
    GEN_SQLSTR(g_sql_buff,
            "SELECT DISTINCT TSVI.server_id AS host_id, TSVI.server_tag AS host_tag,\
            TPI.project_id AS project_id, TPI.project_english_name AS project_name, \
            TSVI.ip_inside AS host_ip \
            FROM t_server_info AS TSVI \
            INNER JOIN t_business_info AS TBI ON TBI.module_id = TSVI.module_id \
            INNER JOIN t_project_info AS TPI ON TPI.project_id = TBI.project_id \
            WHERE TSVI.server_id = %u", host_id);
    row_num = g_db->select_first_row(&row, g_sql_buff);
    DB_CHECK_ROW(row_num, row);

    p_out->host_ip = ip2long(row[idx_host_ip]);
    p_out->project_id = atoi(row[idx_project_id]);
    ITL_STRCPY(p_out->host_tag, row[idx_host_tag]);
    ITL_STRCPY(p_out->project_name, row[idx_project_name]);

    ///获取service_id
    GEN_SQLSTR(g_sql_buff,
            "SELECT DISTINCT service_id FROM t_service_info WHERE machine_id = %u AND service_type = %u",
            host_id, service_type);
    row_num = g_db->select_first_row(&row, g_sql_buff);
    if (row_num <= 0 || NULL == row)
    {
        // 没有要监控
        return DB_SUCC;
    }
    // DB_CHECK_ROW(row_num, row);

    char str_service_id[ITL_MAX_STR_LEN] = {0};
    left_len = sizeof(str_service_id);
    write_pos = str_service_id;
    while (NULL != row && left_len > strlen(row[0]))
    {
        len = snprintf(write_pos, left_len, "%s,", row[0]);
        left_len -= len;
        write_pos += len;
        row = g_db->select_next_row();
    }
    *(write_pos - 1) = '\0';

    ///获取数据库实例信息
    char str_port[ITL_MAX_STR_LEN] = {0};
    map<int, string> id_port_map;
    if (ITL_TYPE_MYSQL == service_type)
    {
        int idx_db_id = 0, idx_port = 1;
        GEN_SQLSTR(g_sql_buff, "SELECT DISTINCT db_id, port FROM t_db_info WHERE db_id IN (%s)", str_service_id);
        row_num = g_db->select_first_row(&row, g_sql_buff);
        DB_CHECK_ROW(row_num, row);

        left_len = sizeof(str_port);
        write_pos = str_port;
        while (NULL != row)
        {
            int db_id = atoi(row[idx_db_id]);
            id_port_map[db_id] = row[idx_port];
            len = snprintf(write_pos, left_len, "%d,", atoi(row[idx_port]));
            left_len -= len;
            write_pos += len;
            row = g_db->select_next_row();
        }
        *(write_pos - 1) = '\0';
    }


    ///获取有效屏蔽时间段
    string str_metric_arg;/**<metric_name+metric_arg字符串*/
    shield_slot_t metric_shield;
    map<string, shield_slot_t> metric_shield_map;
    int idx_metric = 0, idx_arg = 1, idx_start = 2, idx_end = 3;
    GEN_SQLSTR(g_sql_buff,
            "SELECT DISTINCT metric, arg, start_time, end_time \
            FROM t_alarm_controller_info \
            WHERE service_id IN (%s) AND service_type = %u \
            AND UNIX_TIMESTAMP(NOW()) < end_time AND start_time < end_time",
            str_service_id, service_type);
    g_db->select_first_row(&row, g_sql_buff);
    while (NULL != row)
    {
        str_metric_arg = "";
        str_metric_arg.append(row[idx_metric]);
        str_metric_arg.append(row[idx_arg]);
        metric_shield.start_time = atoi(row[idx_start]);
        metric_shield.end_time = atoi(row[idx_end]);
        metric_shield_map[str_metric_arg] = metric_shield;
        row = g_db->select_next_row();
    }

    alarm_default_conf_t alarm_default_conf = {0};
    alarm_default_conf_t *p_alarm_default_conf = &alarm_default_conf;
    db_get_default_alarm_conf(service_type, &alarm_default_conf);

    //获取默认告警配置
    int idx_metric_type = 0, idx_metric_id = 1, idx_metric_name = 2;
    int idx_warning_val = 3, idx_critical_val = 4;
    int idx_op = 5, idx_normal_interval = 6, idx_abnormal_interval = 7;
    int idx_max_attempt = 8, idx_metric_arg = 9;
    GEN_SQLSTR(g_sql_buff,
            "SELECT DISTINCT TMI.metric_type AS metric_type,\
            TMI.metric_id AS metric_id, TMI.metric_name AS metric_name,\
            TMI.warning_val AS warning_val, TMI.critical_val AS critical_val, TMI.operation AS op,\
            TMI.normal_interval AS normal_interval, TMI.retry_interval AS abnormal_interval,\
            TMI.max_attempt AS max_attempt, TSMA.arg AS metric_arg \
            FROM t_metric_info AS TMI \
            INNER JOIN t_metric_group_info AS TMGI ON TMI.metric_group_id = TMGI.metric_group_id \
            INNER JOIN t_metric_group_service AS TMGS ON TMI.metric_group_id = TMGS.metric_group_id \
            LEFT JOIN t_service_metric_arg AS TSMA ON TMGS.service_id = TSMA.service_id \
            AND TMGS.service_type = TSMA.service_type \
            AND TSMA.metric_name = TMI.metric_name \
            WHERE TMI.metrictype_id & %d <> 0 AND TMGS.service_id IN (%s) AND TMGS.service_type = %u",
            METRIC_TYPE_ALARM, str_service_id, service_type);
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("Get default alarm info[%s]", g_sql_buff);
#endif
    row_num = g_db->select_first_row(&row, g_sql_buff);
    DB_CHECK_ROW(row_num, row);

    //<metric_name+metric_arg, 告警配置>键值对
    map<string, alarm_metric_t> metric_alarm_conf_map;
    alarm_metric_t alarm_metric_info;
    map<string, shield_slot_t>::iterator shield_it;
    while (NULL != row)
    {
        alarm_metric_info.alarm_metric.id = atoi(row[idx_metric_id]);
        ITL_STRCPY_LEN(&alarm_metric_info.alarm_metric, name, row[idx_metric_name]);
        alarm_metric_info.alarm_arg.id = 0;
        metric_name2id(row[idx_warning_val], str_buff, sizeof(str_buff), &metric_id_map);
        ITL_STRCPY_LEN(&alarm_metric_info, warning_val, str_buff);
        metric_name2id(row[idx_critical_val], str_buff, sizeof(str_buff), &metric_id_map);
        ITL_STRCPY_LEN(&alarm_metric_info, critical_val, str_buff);
        alarm_metric_info.op = atoi(row[idx_op]);
        alarm_metric_info.normal_interval = atoi(row[idx_normal_interval]) / ITL_ALARM_INTERVAL;
        alarm_metric_info.abnormal_interval = atoi(row[idx_abnormal_interval]) / ITL_ALARM_INTERVAL;
        alarm_metric_info.max_attempt = atoi(row[idx_max_attempt]);


        alarm_metric_info.warning_way = p_alarm_default_conf->warning_way;
        alarm_metric_info.critical_way = p_alarm_default_conf->critical_way;
        ITL_STRCPY_LEN(&alarm_metric_info, no_alarm_range, p_alarm_default_conf->no_alarm_range);
        ITL_STRCPY_LEN(&alarm_metric_info, alarm_frequency, p_alarm_default_conf->alarm_frequency);
        ITL_STRCPY_LEN(&alarm_metric_info.alarm_contact, mobile_list, p_alarm_default_conf->alarm_mobile);
        ITL_STRCPY_LEN(&alarm_metric_info.alarm_contact, email_list, p_alarm_default_conf->alarm_email);
        ITL_STRCPY_LEN(&alarm_metric_info.alarm_contact, rtx_list, p_alarm_default_conf->alarm_rtx);


        //将arg_list分开
        char *p_arg_begin = str_buff;
        char *p_arg_end = NULL;
        if (ITL_TYPE_MYSQL == service_type)
        {
            ITL_STRCPY(str_buff, str_port);
        }
        else
        {
            ITL_STRCPY(str_buff, row[idx_metric_arg]);
        }
        while (NULL != p_arg_begin)
        {
            p_arg_end = index(p_arg_begin, ',');
            if (NULL != p_arg_end)
            {
                *p_arg_end = '\0';
            }
            ITL_STRCPY_LEN(&alarm_metric_info.alarm_arg, name, p_arg_begin);
            str_metric_arg = "";
            str_metric_arg.append(row[idx_metric_name]);
            str_metric_arg.append(p_arg_begin);

            //获取屏蔽时间段
            shield_it = metric_shield_map.find(str_metric_arg);
            if (shield_it != metric_shield_map.end())
            {
                shield_slot_t *p_slot = &(shield_it->second);
                alarm_metric_info.shield_time.start = p_slot->start_time;
                alarm_metric_info.shield_time.end = p_slot->end_time;
            }
            else
            {
                alarm_metric_info.shield_time.start = 0;
                alarm_metric_info.shield_time.end = 0;
            }

            metric_alarm_conf_map[str_metric_arg] = alarm_metric_info;
            p_arg_begin = (NULL == p_arg_end) ? p_arg_end : p_arg_end + 1;
        }

        row = g_db->select_next_row();
    }

    //获取特殊告警策略
    int idx_special_metric_name = 0, idx_special_metric_arg = 1;
    int idx_special_warning_val = 2, idx_special_critical_val = 3, idx_special_op = 4;
    int idx_special_normal_interval = 5, idx_special_abnormal_interval = 6;
    int idx_special_max_attempt = 7, idx_special_no_alarm_range = 8;
    int idx_special_warning_way = 9, idx_special_critical_way = 10;
    int idx_special_alarm_frequency = 11, idx_special_service_id = 12;
    GEN_SQLSTR(g_sql_buff,
            "SELECT DISTINCT TASI.metric_name AS metric_name, TASI.arg AS metric_arg,\
            TASI.warning_val AS warning_val, TASI.critical_val AS critical_val, TASI.operation AS op,\
            TASI.normal_interval AS normal_interval, TASI.retry_interval AS abnormal_interval,\
            TASI.max_attempt AS max_attempt, TASI.no_warning_range AS no_warning_range,\
            TASI.warning_way AS warning_way, TASI.critical_way AS critical_way,\
            TADI.default_value AS alarm_frequency, TAI.service_id AS service_id \
            FROM t_alarm_info AS TAI \
            INNER JOIN t_alarm_strategy_info AS TASI ON TASI.id = TAI.alarm_strategy_id \
            INNER JOIN t_alarm_default_info AS TADI ON TADI.default_key = TASI.alarm_expr \
            WHERE TAI.service_id IN (%s) AND TAI.service_type = %u", str_service_id, service_type);
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("Get special alarm info[%s]", g_sql_buff);
#endif
    g_db->select_first_row(&row, g_sql_buff);

    iterator_t(id_port_map) it_port;
    iterator_t(metric_alarm_conf_map) iter_metric_alarm;
    alarm_metric_t *p_alarm_metric = NULL;
    while (NULL != row)
    {
        str_metric_arg = "";
        str_metric_arg.append(row[idx_special_metric_name]);
        if (ITL_TYPE_MYSQL == service_type)
        {
            int service_id = atoi(row[idx_special_service_id]);
            it_port = id_port_map.find(service_id);
            if (it_port == id_port_map.end())
            {
                ERROR_LOG("No service_id[%d] info in id_port_map", service_id);
                row = g_db->select_next_row();
                continue;
            }
            str_metric_arg.append(it_port->second);
        }
        else
        {
            str_metric_arg.append(row[idx_special_metric_arg]);
        }

        iter_metric_alarm = metric_alarm_conf_map.find(str_metric_arg);
        if (iter_metric_alarm != metric_alarm_conf_map.end())
        {
            p_alarm_metric = &iter_metric_alarm->second;
            metric_name2id(row[idx_special_warning_val], str_buff, sizeof(str_buff), &metric_id_map);
            ITL_STRCPY_LEN(p_alarm_metric, warning_val, str_buff);
            metric_name2id(row[idx_special_critical_val], str_buff, sizeof(str_buff), &metric_id_map);
            ITL_STRCPY_LEN(p_alarm_metric, critical_val, str_buff);
            p_alarm_metric->op = atoi(row[idx_special_op]);
            p_alarm_metric->normal_interval = atoi(row[idx_special_normal_interval]) / ITL_ALARM_INTERVAL;
            p_alarm_metric->abnormal_interval = atoi(row[idx_special_abnormal_interval]) / ITL_ALARM_INTERVAL;
            p_alarm_metric->max_attempt = atoi(row[idx_special_max_attempt]);
            ITL_STRCPY_LEN(p_alarm_metric, no_alarm_range, row[idx_special_no_alarm_range]);
            ITL_STRCPY_LEN(p_alarm_metric, alarm_frequency, row[idx_special_alarm_frequency]);
            GET_ALAMR_WAY(p_alarm_metric->warning_way, row[idx_special_warning_way]);
            GET_ALAMR_WAY(p_alarm_metric->critical_way, row[idx_special_critical_way]);
        }
        else
        {
            ERROR_LOG("Database ERROR: cannot find special strategy of [%s] in metric_alarm_conf_map",
                    str_metric_arg.c_str());
        }

        row = g_db->select_next_row();
    }

    //获取特殊告警联系人信息
    vector<string> special_linkman_vec;
    int idx_link_metric_name = 0, idx_link_metric_arg = 1;
    int idx_link_rtx = 2, idx_link_mobile = 3, idx_link_email = 4;
    GEN_SQLSTR(g_sql_buff,
            "SELECT DISTINCT TAGME.metric_name AS metric_name, TAGME.arg AS metric_arg,\
            TUI.user_name AS user_name, TUI.cellphone AS mobile, TUI.email AS email \
            FROM t_alarm_group_service AS TAGS \
            INNER JOIN t_alarm_group_metric AS TAGME ON TAGS.alarm_group_id = TAGME.alarm_group_id \
            INNER JOIN t_alarm_group_user AS TAGU ON TAGU.alarm_group_id = TAGS.alarm_group_id \
            INNER JOIN t_user_info AS TUI ON TUI.user_id = TAGU.user_id \
            WHERE TAGS.service_id IN (%s) AND TAGS.service_type = %u", str_service_id, service_type);
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("Get special linkman info[%s]", g_sql_buff);
#endif
    g_db->select_first_row(&row, g_sql_buff);
    while (NULL != row)
    {
        str_metric_arg = "";
        str_metric_arg.append(row[idx_link_metric_name]);
        str_metric_arg.append(row[idx_link_metric_arg]);
        iter_metric_alarm = metric_alarm_conf_map.find(str_metric_arg);
        if (iter_metric_alarm != metric_alarm_conf_map.end())
        {
            p_alarm_metric = &iter_metric_alarm->second;
            STR_MERGE_LEN(&p_alarm_metric->alarm_contact, mobile_list, row[idx_link_mobile]);
            STR_MERGE_LEN(&p_alarm_metric->alarm_contact, email_list, row[idx_link_email]);
            STR_MERGE_LEN(&p_alarm_metric->alarm_contact, rtx_list, row[idx_link_rtx]);
        }
        else
        {
            ERROR_LOG("Database ERROR: cannot find special linkman of [%s] in metric_alarm_conf_map",
                    str_metric_arg.c_str());
        }
        //默认联系人增加特殊联系人，为host_down准备
        STR_MERGE(p_alarm_default_conf->alarm_mobile, row[idx_link_mobile]);
        STR_MERGE(p_alarm_default_conf->alarm_email, row[idx_link_email]);
        STR_MERGE(p_alarm_default_conf->alarm_rtx, row[idx_link_rtx]);

        row = g_db->select_next_row();
    }

    p_out->alarm_metric_list.clear();
    iter_metric_alarm = metric_alarm_conf_map.begin();
    while (iter_metric_alarm != metric_alarm_conf_map.end())
    {
        p_out->alarm_metric_list.push_back(iter_metric_alarm->second);
        ++iter_metric_alarm;
    }

    //down告警配置
    str_metric_arg = "host_down_metric";
    shield_it = metric_shield_map.find(str_metric_arg);
    uint32_t start_time = 0;
    uint32_t end_time = 0;
    if (shield_it != metric_shield_map.end())
    {
        shield_slot_t *p_slot = &(shield_it->second);
        start_time = p_slot->start_time;
        end_time = p_slot->end_time;
    }
    p_out->node_contact.shield_time.start = start_time;
    p_out->node_contact.shield_time.end = end_time;
    p_out->node_contact.alarm_way = 7;
    ITL_STRCPY_LEN(&p_out->node_contact.alarm_contact, mobile_list, p_alarm_default_conf->alarm_mobile);
    ITL_STRCPY_LEN(&p_out->node_contact.alarm_contact, email_list, p_alarm_default_conf->alarm_email);
    ITL_STRCPY_LEN(&p_out->node_contact.alarm_contact, rtx_list, p_alarm_default_conf->alarm_rtx);

    id_port_map.clear();
    metric_alarm_conf_map.clear();
    metric_shield_map.clear();

    return DB_SUCC;
}


int db_get_switch_node_config(uint32_t work_id, uint32_t work_num, db_p_get_node_switch_config_out *p_out)
{
    assert(NULL != p_out);
    if (work_num > 0 && work_id >= work_num)
    {
        ERROR_LOG("Switch ParamError: work_id[%u] >= work_num[%u]", work_id, work_num);
        return DB_NO_RECORD;
    }

    char str_buff[ITL_MAX_STR_LEN] = {0};
    if (work_num > 0)
    {
        sprintf(str_buff, " AND TSI.switch_id %% %u = %u", work_num, work_id);
    }
    int idx_id = 0, idx_tag = 1, idx_type = 2, idx_ip = 3, idx_community = 4;
    GEN_SQLSTR(g_sql_buff,
            "SELECT DISTINCT TSI.switch_id AS switch_id, TSI.switch_id AS switch_tag, TBTI.brand_id AS switch_type, TSI.ip_inside AS ip_inside, TSI.community AS community FROM t_switch_info AS TSI INNER JOIN t_pattern_info AS TPI ON TPI.pattern_id = TSI.pattern_id INNER JOIN t_brand_type_info AS TBTI ON TBTI.brand_id = TPI.brand_id WHERE TSI.ip_inside != '' AND LENGTH(TSI.community) > 1 %s", str_buff);
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("get switch[%u] node conf info[%s]", switch_id, g_sql_buff);
#endif

    MYSQL_ROW row;
    int row_num = g_db->select_first_row(&row, g_sql_buff);
    DB_CHECK_ROW(row_num, row);

    vector<switch_base_info_t> switch_base_info_vec;
    switch_base_info_t switch_base_info;
    while (NULL != row)
    {
        switch_base_info.switch_id = atoi(row[idx_id]);
        ITL_STRCPY(switch_base_info.switch_tag, row[idx_tag]);
        switch_base_info.switch_type = atoi(row[idx_type]);
        ITL_STRCPY(switch_base_info.switch_ip, row[idx_ip]);
        ITL_STRCPY(switch_base_info.switch_community, row[idx_community]);
        switch_base_info_vec.push_back(switch_base_info);
        row = g_db->select_next_row();
    }

    p_out->so_num = 1;
    ITL_STRCPY(p_out->so_name_list, "libswitchproto.so");

#define GEN_SWITCH_METRIC_CONF(id, name_index, type, arg, metric, group) \
    do {\
        metric.metric_id = id;\
        metric.metric_type = type;\
        ITL_STRCPY_LEN(&(metric), metric_arg, arg);\
        ITL_STRCPY(metric.metric_name, g_switch_metric_list[name_index]);\
        group.metric_config.push_back(metric);\
    } while(false)

    group_config_t bash_group;
    group_config_t string_group;
    metric_config_t metric_conf;

    bash_group.collect_interval = 60;
    bash_group.metric_config.clear();
    GEN_SWITCH_METRIC_CONF(107, SWITCH_CPU_USED, METRIC_TYPE_RRD, "", metric_conf, bash_group);
    GEN_SWITCH_METRIC_CONF(108, SWITCH_MEM_FREE, METRIC_TYPE_RRD, "", metric_conf, bash_group);

    string_group.collect_interval = 300;
    string_group.metric_config.clear();
    GEN_SWITCH_METRIC_CONF(10000, SWITCH_INTERFACE_INFO, METRIC_TYPE_MYSQL, "", metric_conf, string_group);

    group_config_t if_group;
    if_group.collect_interval = 60;
    switch_node_conf_t switch_conf;
    p_out->switch_confs.clear();
    vector_for_each(switch_base_info_vec, switch_iter)
    {
        //int ret_val = db_get_metric_group_info(switch_iter->switch_id, ITL_TYPE_SWITCH, switch_conf.group_info,
        //            p_out->so_name_list, sizeof(p_out->so_name_list), p_out->so_num);
        //if (0 != ret_val)
        //{
        //    continue;
        //    //return -1;
        //}
        switch_conf.group_info.clear();
        switch_conf.group_info.push_back(bash_group);
        switch_conf.group_info.push_back(string_group);

        //获取接口配置
        if_group.metric_config.clear();
        GEN_SQLSTR(g_sql_buff,
                "SELECT if_idx FROM t_switch_interface_info WHERE switch_id = %u AND is_selected = 1",
                switch_iter->switch_id);
#ifdef MYSQL_DETAIL_LOG
        INFO_LOG("get switch[%s] if_idx info[%s]", switch_iter->switch_ip, g_sql_buff);
#endif
        g_db->select_first_row(&row, g_sql_buff);
        while (NULL != row)
        {
            GEN_SWITCH_METRIC_CONF(100001, SWITCH_INTERFACE_IN, METRIC_TYPE_RRD, row[0], metric_conf, if_group);
            GEN_SWITCH_METRIC_CONF(100002, SWITCH_INTERFACE_OUT, METRIC_TYPE_RRD, row[0], metric_conf, if_group);
            row = g_db->select_next_row();
        }
        if (if_group.metric_config.size() > 0)
        {
            switch_conf.group_info.push_back(if_group);
        }

        switch_conf.switch_id = switch_iter->switch_id;
        ITL_STRCPY(switch_conf.switch_tag, switch_iter->switch_tag);
        switch_conf.switch_type = switch_iter->switch_type;
        ITL_STRCPY(switch_conf.switch_ip, switch_iter->switch_ip);
        ITL_STRCPY(switch_conf.switch_community, switch_iter->switch_community);

        p_out->switch_confs.push_back(switch_conf);
    }
    switch_base_info_vec.clear();

    return 0;
}

int db_get_update_notice(db_p_get_update_notice_out *p_out)
{
    int idx_node_id = 0, idx_node_type = 1, idx_collect_flag = 2, idx_alarm_flag = 3;
    GEN_SQLSTR(g_sql_buff,
            "SELECT node_id, node_type, collect_update_flag, alarm_update_flag \
            FROM t_update_notice \
            WHERE collect_update_flag = 1 OR alarm_update_flag = 1");

    MYSQL_ROW row;
    int row_num = g_db->select_first_row(&row, g_sql_buff);
    update_notice_t update_notice;
    while (NULL != row)
    {
        update_notice.node_id = atoi(row[idx_node_id]);
        update_notice.node_type = atoi(row[idx_node_type]);
        update_notice.collect_update_flag = atoi(row[idx_collect_flag]);
        update_notice.alarm_update_flag = atoi(row[idx_alarm_flag]);
        p_out->update_node.push_back(update_notice);
        row = g_db->select_next_row();
    }


    if (row_num > 0)
    {
        int ret = 0;
        GEN_SQLSTR(g_sql_buff,
                "UPDATE t_update_notice SET collect_update_flag = 0, alarm_update_flag = 0 \
                WHERE collect_update_flag = 1 OR alarm_update_flag = 1");
        STD_EXECSQL(g_db, g_sql_buff, ret);
    }

    return 0;
}

/** 
 * @brief 保存服务器状态
 * 
 * @param p_in
 * 
 * @return 
 */
int db_store_host_status(uint32_t node_id, uint32_t node_type, uint32_t node_status)
{
    DEBUG_LOG("node_id[%u] node_type[%u] node_status[%u]",
            node_id, node_type, node_status);                                       
    if (node_type == NODE_TYPE_SWITCH)//暂不支持
    {
        return 0;
    }

    int idx_tag = 0, idx_ip = 1;
    GEN_SQLSTR(g_sql_buff, 
            "SELECT server_tag, ip_inside FROM t_server_info WHERE server_id = %u",
            node_id);

    MYSQL_ROW row;
    int row_num = g_db->select_first_row(&row, g_sql_buff);
    if (row_num == 0)
    {
        ERROR_LOG("No record: %s", g_sql_buff);
        return 0;
    }

    GEN_SQLSTR(g_sql_buff, 
            "INSERT INTO t_host_info SET host_name = '%s', metric_name = 'host_status', metric_arg = '%s', metric_val = '%u' ON DUPLICATE KEY UPDATE metric_val = '%u', last_update_time = NOW()",
            row[idx_tag], row[idx_ip], node_status, node_status);

    int ret = 0;
    STD_EXECSQL(g_db, g_sql_buff, ret);

    return ret;
}



int db_get_node_head_config(const char * node_ip, db_p_get_node_head_config_out * p_out)
{

    assert(NULL != node_ip && NULL != p_out);

    // 获取node_id
    uint32_t node_id = 0;
    int ret = get_host_info(node_ip, &node_id, p_out->host_tag, sizeof(p_out->host_tag));
    if (DB_SUCC != ret)
    {
        return ret;
    }
    p_out->node_id = node_id;


    //获取 head_addr
    if (NULL != g_p_head_ip && ITL_BETWEEN(g_head_port, ITL_MIN_PORT, ITL_MAX_PORT))
    {
        p_out->head_addr.port = g_head_port;
        ITL_STRCPY(p_out->head_addr.ip, g_p_head_ip);
    }
    else
    {
        char match_ip[16] = {0};
        ITL_STRCPY(match_ip, node_ip);
        char *dot_pos = rindex(match_ip, '.');
        assert(NULL != dot_pos);
        *dot_pos = '\0';
        int idx_head_ip = 0, idx_head_port = 1;
        GEN_SQLSTR(g_sql_buff, "SELECT DISTINCT TNSI.ip_inside AS head_ip, TNSI.listen_port AS head_port FROM t_network_segment_info AS TNSI WHERE segment LIKE '%s%s.%s'", "%", match_ip, "%");
        MYSQL_ROW row;
        int row_num = g_db->select_first_row(&row, g_sql_buff);
        if (row_num != 1)
        {
            ERROR_LOG("Database ERROR: row_num[%d != 1], SQL: %s", row_num, g_sql_buff);
            return DB_ERR;
        }
        p_out->head_addr.port = atoi(row[idx_head_port]);
        ITL_STRCPY(p_out->head_addr.ip, row[idx_head_ip]);
    }

    return DB_SUCC;
}


int db_get_node_control_config(const char * node_ip, db_p_get_node_control_config_out * p_out)
{

    assert(NULL != node_ip && NULL != p_out);

    // 获取node_id
    int idx_node_id = 0;
    const char *format = "SELECT DISTINCT TSRV.server_id AS host_id FROM t_server_info AS TSRV WHERE TSRV.ip_inside = '%s'";
    GEN_SQLSTR(g_sql_buff, format, node_ip);
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("Get server_tag info[%s]", g_sql_buff);
#endif

    MYSQL_ROW row;
    int row_num = g_db->select_first_row(&row, g_sql_buff);
    if (row_num <= 0 || NULL == row) 
    {
        return DB_NO_RECORD;
    } 
    else if (row_num > 1) 
    {
        ERROR_LOG("Database ERROR: row_num[%d != 1], SQL: %s", row_num, g_sql_buff);
        return DB_ERR;
    }
    p_out->node_id = atoi(row[idx_node_id]);


    //获取 control_addr
    if (NULL != g_control_ip && ITL_BETWEEN(g_control_port, ITL_MIN_PORT, ITL_MAX_PORT))
    {
        p_out->control_addr.port = g_control_port;
        ITL_STRCPY(p_out->control_addr.ip, g_control_ip);
    }
    else
    {
        char match_ip[16] = {0};
        ITL_STRCPY(match_ip, node_ip);
        char *dot_pos = rindex(match_ip, '.');
        assert(NULL != dot_pos);
        *dot_pos = '\0';
        int idx_control_ip = 0, idx_control_port = 1;
        GEN_SQLSTR(g_sql_buff, "SELECT DISTINCT TNSI.ip_inside AS control_ip, TNSI.listen_port AS control_port FROM t_network_segment_info AS TNSI WHERE segment LIKE '%s%s.%s'", "%", match_ip, "%");
        row_num = g_db->select_first_row(&row, g_sql_buff);
        if (row_num != 1)
        {
            ERROR_LOG("Database ERROR: row_num[%d != 1], SQL: %s", row_num, g_sql_buff);
            return DB_ERR;
        }
        p_out->control_addr.port = atoi(row[idx_control_port]);
        ITL_STRCPY(p_out->control_addr.ip, row[idx_control_ip]);
    }

    return DB_SUCC;
}

int db_record_db_mgr_exec_result(const db_p_record_db_mgr_exec_result_in * p_in)
{
    assert(NULL != p_in);

    static char sql[SQL_BUFF_LEN];
    static char err_desc[SQL_BUFF_LEN];
    mysql_real_escape_string(g_db->get_conn(), sql, p_in->sql_sentence, p_in->_sql_sentence_len);
    mysql_real_escape_string(g_db->get_conn(), err_desc, p_in->err_desc, p_in->_err_desc_len);
    GEN_SQLSTR(g_sql_buff, 
            "INSERT INTO t_manage_result_info SET manage_value_id = %u, manage_id = %u, sql_no = %u,\
            status_code = %u, `sql` = '%s', err_desc = '%s', log_time = UNIX_TIMESTAMP() \
            ON DUPLICATE KEY UPDATE sql_no = %u, status_code = %u, `sql` = '%s', \
            err_desc = '%s', log_time = UNIX_TIMESTAMP()",
            p_in->manage_value_id, p_in->manage_id, p_in->sql_no, p_in->status_code, sql, err_desc,
            p_in->sql_no, p_in->status_code, sql, err_desc);
    INFO_LOG("store db mgr result sql: %s", g_sql_buff);
    int ret = DB_SUCC;
    STD_EXECSQL(g_db, g_sql_buff, ret);

    //判断操作是否结束
    GEN_SQLSTR(g_sql_buff, 
            "SELECT COUNT(manage_value_id) AS not_exec_cnt FROM t_manage_value_info WHERE manage_id = %u \
            AND manage_value_index NOT IN(SELECT manage_value_index FROM t_manage_value_info WHERE manage_id = %u) \
            AND manage_value_index NOT IN(SELECT manage_value_id FROM t_manage_result_info WHERE manage_id = %u AND manage_key = 'is_result' AND manage_value = 0)",
            p_in->manage_id, p_in->manage_id, p_in->manage_id);
    MYSQL_ROW row;
    int row_num = g_db->select_first_row(&row, g_sql_buff);
    if (row_num != 1 || NULL == row) 
    {
        ERROR_LOG("Database ERROR: row_num[%d != 1], SQL: %s", row_num, g_sql_buff);
        return -1;
    } 
    int not_exec_cnt = atoi(row[0]);
    if (not_exec_cnt == 0)
    {//修改t_manage_info相关字段
        //状态 1：新建，2：执行中，3：执行结束，4：完成
        GEN_SQLSTR(g_sql_buff, 
            "UPDATE t_manage_info SET exec_finish_time = UNIX_TIMESTAMP(), state = 3 WHERE manage_id = %u",
            p_in->manage_id);
        STD_EXECSQL(g_db, g_sql_buff, ret);
    }

    return ret;
}



int db_get_node_mysql_instance(const char * host_ip, db_p_get_node_mysql_instance_out * p_out)
{
    assert(NULL != host_ip && NULL != p_out);

    int ret_code = DB_SUCC;
    int service_type = ITL_TYPE_MYSQL;

    ///获取资产信息: host_id
    uint32_t node_id = 0;
    int ret = get_host_info(host_ip, &node_id, NULL, 0);
    if (DB_SUCC != ret)
    {
        return ret;
    }

    ///获取监控service信息
    int idx_service_id = 0;
    const char *format = "SELECT DISTINCT service_id FROM t_service_info WHERE machine_id = %u AND service_type = %d";
    GEN_SQLSTR(g_sql_buff, format, node_id, service_type);
#ifdef MYSQL_DETAIL_LOG
    INFO_LOG("Get service_id info[%s]", g_sql_buff);
#endif
    MYSQL_ROW row;
    int row_num = g_db->select_first_row(&row, g_sql_buff);
    if (row_num <= 0 || NULL == row)
    {
        // 没有mysql要监控
        return DB_SUCC;
    }

    uint32_t service_id = 0;
    vector<uint32_t> service_id_vec;
    while (NULL != row)
    {
        service_id = atoi(row[idx_service_id]);
        service_id_vec.push_back(service_id);


        row = g_db->select_next_row();
    }


    vector_for_each(service_id_vec, it)
    {
        service_id = *it;
        /**role角色 1:MASTER 2:SLAVE 3: Master/Slave 4:其他*/
        int idx_port = 1, idx_sock = 2, idx_role = 3;
        GEN_SQLSTR(g_sql_buff, "SELECT db_id, port, socket, role FROM t_db_info WHERE db_id = %u", service_id);
        row_num = g_db->select_first_row(&row, g_sql_buff);
        if (row_num != 1)
        {
            ERROR_LOG("Database ERROR: row_num[%d != 1], SQL: %s", row_num, g_sql_buff);
            ret_code = DB_ERR;
            break;
        }

        db_instance_t instance;
        instance.type = atoi(row[idx_role]);
        instance.port = atoi(row[idx_port]);
        ITL_STRCPY(instance.sock, row[idx_sock]);
        p_out->db_instance_info.push_back(instance);
    }

    service_id_vec.clear();
    return ret;
}

