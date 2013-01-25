/**
 * =====================================================================================
 *       @file  db_operator.cpp
 *      @brief   
 *
 *  数据库操作函数
 *
 *   @internal
 *     Created  18/04/2011 06:31:41 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  mason , mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#include <stdlib.h>
#include "lib/log.h"
#include "lib/utils.h"
#include "db_operator.h"


using namespace std;

/** 
* @brief   获取交换机配置更新标志位
* @param   db_conn  数据库连接
* @param   action   更新操做
* @return  true 有更新 false 无更新
*/
bool db_get_update_status(c_mysql_iface* db_conn, unsigned char *action)
{
    if(db_conn == NULL || action == NULL) {
        return false;
    }

    MYSQL *conn = NULL;
    if((conn = db_conn->get_conn()) == NULL) {
        ERROR_LOG("Mysql connection has gone away.");
        return false;
    }

    //开始事务
    if(mysql_autocommit(conn, false) != 0) {
        ERROR_LOG("Start transaction failed.db error is:[%s].",  db_conn->get_last_errstr());
        return false;
    }
    char select_sql[MAX_STR_LEN] = {'\0'};
    snprintf(select_sql, sizeof(select_sql) - 1,
            "SELECT is_updated,action FROM t_head_status WHERE head_id=-1 FOR UPDATE");//对这条记录加锁

    MYSQL_ROW row = NULL;
    int  row_count = -1;
    row_count = db_conn->select_first_row(&row, select_sql);
    if(row_count < 0) {
        ERROR_LOG("db_conn->select_first_row failed.SQL:[%s] db error is:[%s]", 
                select_sql, db_conn->get_last_errstr());
        //回滚结束事务
        mysql_rollback(conn);
        return false;
    } else if(row_count == 0) {
        ERROR_LOG("There are no record of switch in db.");
        //回滚结束事务
        mysql_rollback(conn);
        return false;
    } else if(row_count > 1) {
        ERROR_LOG("There are too many record of switch in db.");
        //回滚结束事务
        mysql_rollback(conn);
        return false;
    }

    if(row[0] && row[0][0] == 'Y') {
        *action = row[1] != NULL ? (unsigned char)atoi(row[1]) : 15;
        if (*action == 0) {
            DEBUG_LOG("Find update, but action[0x0]");
            return false;
        }
        DEBUG_LOG("Find update action[0x%x]", *action);
        return true;
    } else {
        //回滚结束事务
        mysql_rollback(conn);
        return false;
    }
}

/** 
* @brief   设置switch更新标志位
* @param   db_conn  数据库连接
* @return  0-success -1-failed
*/
int db_set_update_status(c_mysql_iface* db_conn)
{
    if(db_conn == NULL) return -1;

    MYSQL *conn = NULL;
    if((conn = db_conn->get_conn()) == NULL) {
        ERROR_LOG("Mysql connection has gone away.");
        return -1;
    }

    char update_sql[MAX_STR_LEN] = {'\0'};
    snprintf(update_sql, sizeof(update_sql) - 1,
            "UPDATE t_head_status SET is_updated='N',action=0 WHERE head_id=-1");

    int ret = 0;
    ///以下每步操作都得做
    if(db_conn->execsql(update_sql) < 0) {
        ERROR_LOG("db_conn->execsql failed.SQL:[%s] db error is:[%s]", update_sql, db_conn->get_last_errstr());
        ret = -1;
    }
    //提交事务
    if(mysql_commit(conn) != 0) {
        ERROR_LOG("Commit transaction failed.db error is:[%s].", db_conn->get_last_errstr());
        ret = -1;
    }
    //改变自动提交模式
    if(mysql_autocommit(conn, true) != 0) {
        ERROR_LOG("Change autocommit mode failed.db error is:[%s].", db_conn->get_last_errstr());
        ret = -1;
    }
    return ret;
}
/** 
* @brief   从数据库中获取metric的特殊报警相关信息
* @param   p_metric_set 保存metric_alarm_t结构的vector指针
* @param   db_conn      数据库连接
* @return  0-success -1-failed
*/
int db_get_specified_metric_alarm_info(metric_alarm_vec_t *p_metric_set, c_mysql_iface* db_conn)
{
    if(p_metric_set == NULL || db_conn == NULL) return -1;

    char select_sql[MAX_STR_LEN] = {'\0'};
    sprintf(select_sql,
            "SELECT ai.alarm_id,asi.metric_name,asi.warning_val,asi.critical_val,asi.operation,\
            mi.normal_interval,mi.retry_interval,mi.max_attempt FROM t_alarm_info AS ai,\
            t_alarm_strategy_info AS asi,t_metric_info AS mi WHERE ai.alarm_strategy_id=asi.id\
            AND ai.alarm_type=2 AND mi.metric_name=asi.metric_name AND mi.metric_type=2");

    MYSQL_ROW row = NULL;
    if(db_conn->select_first_row(&row, select_sql) <= 0)
    {
        ERROR_LOG("db_conn->select_first_row failed.SQL:[%s] db error is:[%s]",
                select_sql, db_conn->get_last_errstr());
        return -1;
    }

    metric_alarm_t   metric_info;
    while(NULL != row)
    {
        memset(&metric_info, 0, sizeof(metric_info));

        if(NULL != row[0] && is_integer(row[0]))
        {
            metric_info.switch_id = atoi(row[0]);
        }
        else
        {
            ERROR_LOG("switch id is null or not correct.");
            goto next;
        }

        if(NULL != row[1])
        {
            strncpy(metric_info.metric_name, row[1], sizeof(metric_info.metric_name) - 1);
        }
        else
        {
            ERROR_LOG("metric name is NULL.");
            goto next;
        }

        if(row[2] && is_numeric(row[2]))
        {
            metric_info.alarm_info.warning_val = atof(row[2]);
        }
        else 
        {
            ERROR_LOG("metric warning value is NULL or not correct.");
            goto next;
        }

        if(row[3] && is_numeric(row[3]))
        {
            metric_info.alarm_info.critical_val = atof(row[3]);
        }
        else 
        {
            ERROR_LOG("metric critical value is NULL.");
            goto next;
        }

        if(row[4] && is_integer(row[4]))
        {
            switch(atoi(row[4]))
            {
                case   1: metric_info.alarm_info.op = OP_GE; break;
                case   2: metric_info.alarm_info.op = OP_LE; break;
                case   3: metric_info.alarm_info.op = OP_GT; break;
                case   4: metric_info.alarm_info.op = OP_LT; break;
                case   5: metric_info.alarm_info.op = OP_EQ; break;
                default : metric_info.alarm_info.op = OP_GT; break;//缺省为大于
            }
        }
        else 
        {
            metric_info.alarm_info.op = OP_GT;//缺省为大于
        }

        if(row[5] && is_integer(row[5]))
        {
            metric_info.alarm_info.normal_interval = atoi(row[5]);
        }
        else 
        {
            metric_info.alarm_info.normal_interval = 4;
        }

        if(row[6] && is_integer(row[6]))
        {
            metric_info.alarm_info.retry_interval = atoi(row[6]);
        }
        else 
        {
            metric_info.alarm_info.retry_interval = 2;
        }

        if(row[7] && is_integer(row[7]))
        {
            metric_info.alarm_info.max_atc = atoi(row[7]);
        }
        else 
        {
            metric_info.alarm_info.max_atc = 2;
        }

        if((metric_info.alarm_info.op == OP_GT || metric_info.alarm_info.op == OP_GE) &&
                metric_info.alarm_info.warning_val > metric_info.alarm_info.critical_val)
        {
            ERROR_LOG("wrong threshold values:[warning=%0.2f,critical=%0.2f,operation=\">|>=\"]",
                    metric_info.alarm_info.warning_val, metric_info.alarm_info.critical_val);
            goto next;
        }

        if((metric_info.alarm_info.op == OP_LT || metric_info.alarm_info.op == OP_LE) &&
                metric_info.alarm_info.warning_val < metric_info.alarm_info.critical_val)
        {
            ERROR_LOG("wrong threshold values:[warning=%0.2f,critical=%0.2f,operation=\"<|<=\"]",
                    metric_info.alarm_info.warning_val, metric_info.alarm_info.critical_val);
            goto next;
        }

        if(metric_info.alarm_info.normal_interval < metric_info.alarm_info.retry_interval)
        {    
            ERROR_LOG("error alarm config :normal interval[%u],retry interval[%u],max attempt count[%u]", 
                    metric_info.alarm_info.normal_interval, metric_info.alarm_info.retry_interval,
                    metric_info.alarm_info.max_atc);
            goto next;
        }   

        //添加到metric集合中
        p_metric_set->push_back(metric_info);
next:
        row = db_conn->select_next_row(false);
    }

    return 0;

}

/** 
* @brief   从数据库中获取metric的缺省报警相关信息
* @param   p_metric_set 保存metric_alarm_t结构的vector指针
* @param   db_conn      数据库连接
* @return  0-success -1-failed
*/
int db_get_default_metric_alarm_info(metric_alarm_vec_t *p_metric_set, c_mysql_iface* db_conn)
{
    if(p_metric_set == NULL || db_conn == NULL) return -1;

    char select_sql[MAX_STR_LEN] = {'\0'};
    //用0表示所有的交换机id，缺省值
    sprintf(select_sql,
            "SELECT 0 as switch_id,metric_name,warning_val,critical_val,operation,\
            normal_interval,retry_interval,max_attempt from t_metric_info WHERE metric_type=2");

    MYSQL_ROW row = NULL;
    if(db_conn->select_first_row(&row, select_sql) <= 0)
    {
        ERROR_LOG("db_conn->select_first_row failed.SQL:[%s] db error is:[%s]",
                select_sql, db_conn->get_last_errstr());
        return -1;
    }

    metric_alarm_t metric_info;
    while(NULL != row)
    {
        memset(&metric_info, 0, sizeof(metric_info));

        if(NULL != row[1])
        {
            strncpy(metric_info.metric_name, row[1], sizeof(metric_info.metric_name) - 1);
        }
        else
        {
            ERROR_LOG("metric name is NULL.");
            goto next;
        }

        if(row[2] && is_numeric(row[2]))
        {
            metric_info.alarm_info.warning_val = atof(row[2]);
        }
        else 
        {
            ERROR_LOG("metric warning value is NULL or not correct.");
            goto next;
        }

        if(row[3] && is_numeric(row[3]))
        {
            metric_info.alarm_info.critical_val = atof(row[3]);
        }
        else 
        {
            ERROR_LOG("metric critical value is NULL.");
            goto next;
        }

        if(row[4] && is_integer(row[4]))
        {
            switch(atoi(row[4]))
            {
                case   1: metric_info.alarm_info.op = OP_GE; break;
                case   2: metric_info.alarm_info.op = OP_LE; break;
                case   3: metric_info.alarm_info.op = OP_GT; break;
                case   4: metric_info.alarm_info.op = OP_LT; break;
                case   5: metric_info.alarm_info.op = OP_EQ; break;
                default : metric_info.alarm_info.op = OP_GT; break;//缺省为大于
            }
        }
        else 
        {
            metric_info.alarm_info.op = OP_GT;//缺省为大于
        }

        if(row[5] && is_integer(row[5]))
        {
            metric_info.alarm_info.normal_interval = atoi(row[5]);
        }
        else 
        {
            metric_info.alarm_info.normal_interval = 4;
        }

        if(row[6] && is_integer(row[6]))
        {
            metric_info.alarm_info.retry_interval = atoi(row[6]);
        }
        else 
        {
            metric_info.alarm_info.retry_interval = 2;
        }

        if(row[7] && is_integer(row[7]))
        {
            metric_info.alarm_info.max_atc = atoi(row[7]);
        }
        else 
        {
            metric_info.alarm_info.max_atc = 2;
        }

        if((metric_info.alarm_info.op == OP_GT || metric_info.alarm_info.op == OP_GE) &&
                metric_info.alarm_info.warning_val > metric_info.alarm_info.critical_val)
        {
            ERROR_LOG("wrong threshold values:[warning=%0.2f,critical=%0.2f,operation=\">|>=\"]",
                    metric_info.alarm_info.warning_val, metric_info.alarm_info.critical_val);
            goto next;
        }

        if((metric_info.alarm_info.op == OP_LT || metric_info.alarm_info.op == OP_LE) &&
                metric_info.alarm_info.warning_val < metric_info.alarm_info.critical_val)
        {
            ERROR_LOG("wrong threshold values:[warning=%0.2f,critical=%0.2f,operation=\"<|<=\"]",
                    metric_info.alarm_info.warning_val, metric_info.alarm_info.critical_val);
            goto next;
        }

        if(metric_info.alarm_info.normal_interval < metric_info.alarm_info.retry_interval)
        {    
            ERROR_LOG("error alarm config :normal interval[%u],retry interval[%u],max attempt count[%u]", 
                    metric_info.alarm_info.normal_interval, metric_info.alarm_info.retry_interval,
                    metric_info.alarm_info.max_atc);
            goto next;
        }   

        //添加到metric集合中
        p_metric_set->push_back(metric_info);
next:
        row = db_conn->select_next_row(false);
    }

    return 0;

}
/** 
* @brief   从数据库中获取metric信息
* @param   p_metric_set 保存metric结构的vector指针
* @param   db_conn      数据库连接
* @return  0-success -1-failed
*/
int db_get_metric_info(metric_info_vec_t *p_metric_set, c_mysql_iface* db_conn)
{
    if(p_metric_set == NULL || db_conn == NULL) return -1;

    char select_sql[MAX_STR_LEN] = {'\0'};
    sprintf(select_sql, 
            "SELECT mi.metric_name,mi.metrictype_id,mi.argument FROM t_metric_info mi,t_metric_group_info mgi\
            WHERE mgi.metric_group_id=mi.metric_group_id AND mgi.metric_group_type=2 AND mi.metric_type=2");

    MYSQL_ROW row = NULL;
    if(db_conn->select_first_row(&row, select_sql) <= 0)
    {
        ERROR_LOG("db_conn->select_first_row failed.SQL:[%s] db error is:[%s]",
                select_sql, db_conn->get_last_errstr());
        return -1;
    }

    metric_info_t metric_info;
    while(NULL != row)
    {
        memset(&metric_info, 0, sizeof(metric_info));

        if(NULL != row[0])
        {
            strncpy(metric_info.metric_name, row[0], sizeof(metric_info.metric_name) - 1);
        }
        else
        {
            ERROR_LOG("metric name is NULL.");
            goto next;
        }

        if(row[1] && is_integer(row[1]))
        {
            metric_info.metric_type = atoi(row[1]);
        }
        else 
        {
            //默认置为1，即写rrd类型和报警
            metric_info.metric_type = 3;
        }

        if(row[2])
        {
            strncpy(metric_info.arg, row[2], sizeof(metric_info.arg) - 1);
        }
        else 
        {
            //argument 可以为空
            //do nothing
        }

        //添加到metric集合中
        p_metric_set->push_back(metric_info);
next:
        row = db_conn->select_next_row(false);
    }

    return 0;

}

/** 
* @brief   从数据库中获取switch信息
* @param   p_switch_group_set 保存switch结构的vector指针
* @param   switch_group_num   收集线程的个数(即switch分组的个数，一个线程处理一个分组的switch)
* @param   db_conn            数据库连接
* @return  0-success -1-failed
*/
int db_get_switch_info(switch_group_vec_t *p_switch_group_set, unsigned int switch_group_num, c_mysql_iface* db_conn)
{
    if(p_switch_group_set == NULL || db_conn == NULL || switch_group_num <= 0) return -1;

    char select_sql[MAX_STR_LEN] = {'\0'};
    //id对switch_group_num求余分组
    sprintf(select_sql, 
            "SELECT si.switch_id%s%u as idx,si.switch_id,si.ip_inside,\
            si.community,bti.brand_name\
            FROM t_switch_info si,t_pattern_info pi,t_brand_type_info bti\
            WHERE si.pattern_id=pi.pattern_id and pi.brand_id=bti.brand_id \
            AND si.ip_inside IS NOT NULL AND si.ip_inside <> '' \
            AND si.community IS NOT NULL AND si.community <> '' \
            ORDER BY idx", "\%", switch_group_num);

    MYSQL_ROW row = NULL;
    if(db_conn->select_first_row(&row, select_sql) <= 0)
    {
        ERROR_LOG("db_conn->select_first_row failed.SQL:[%s] db error is:[%s]",
                select_sql, db_conn->get_last_errstr());
        return -1;
    }

    unsigned int cur_id  = 0;
    unsigned int prev_id = 0;
    unsigned int counter = 0;
    switch_group_info_t tmp_switch_group;
    memset(&tmp_switch_group, 0, sizeof(tmp_switch_group));

    prev_id = atoi(row[0]);
    if(row[1] && is_integer(row[1]))
    {
        tmp_switch_group.switch_table[counter].id = atoi(row[1]);
    }
    else
    {
        ERROR_LOG("switch id is null or not correct.");
        return -1;
    }

    if(row[2] && is_inet_addr(row[2]))
    {
        strcpy(tmp_switch_group.switch_table[counter].ip, row[2]);
    }
    else
    {
        ERROR_LOG("switch ip is null or not correct.");
        return -1;
    }

    if(row[3])
    {
        strncpy(tmp_switch_group.switch_table[counter].community, row[3],
                sizeof(tmp_switch_group.switch_table[counter].community) - 1);
    }
    else
    {
        ERROR_LOG("switch community is null.");
        return -1;
    }

    if(row[4])
    {
        if(strcasestr(row[4], "Cisco"))
            tmp_switch_group.switch_table[counter].type = 1;
        else if(strcasestr(row[4], "H3C"))
            tmp_switch_group.switch_table[counter].type = 2;
        else 
        {
            ERROR_LOG("Wrong switch brand name.");
            return -1;//wrong switch brand 
        }
    }
    else 
    {
        ERROR_LOG("switch type is NULL.");
        return -1;
    }
    counter++;

    while(NULL != (row = db_conn->select_next_row(false)))
    {
        cur_id = atoi(row[0]);
        if(cur_id != prev_id)
        {   
            tmp_switch_group.switch_num = counter;
            p_switch_group_set->push_back(tmp_switch_group);
            memset(&tmp_switch_group, 0, sizeof(tmp_switch_group));
            counter = 0;
            prev_id = cur_id;
        }  
        else if(counter >= MAX_SWITCH_NUM_PER_GROUP) //如果一组中的交换机个数大于MAX_SWITCH_NUM_PER_GROUP只截取前MAX_SWITCH_NUM_PER_GROUP个 
        {
            //不太可能发生
            continue;
        }

        if(row[1] && is_integer(row[1]))
        {
            tmp_switch_group.switch_table[counter].id = atoi(row[1]);
        }
        else
        {
            ERROR_LOG("switch id is null or not correct");
            continue;
            //return -1;
        }

        if(row[2] && is_inet_addr(row[2]))
        {
            strcpy(tmp_switch_group.switch_table[counter].ip, row[2]);
        }
        else
        {
            ERROR_LOG("switch ip is null or not correct");
            continue;
            //return -1;
        }

        if(row[3])
        {
            strncpy(tmp_switch_group.switch_table[counter].community, row[3], 
                    sizeof(tmp_switch_group.switch_table[counter].community) - 1);
        }
        else
        {
            ERROR_LOG("switch community is null.");
            continue;
            //return -1;
        }

        if(row[4])
        {
            if(strcasestr(row[4], "Cisco"))
                tmp_switch_group.switch_table[counter].type = 1;
            else if(strcasestr(row[4], "H3C"))
                tmp_switch_group.switch_table[counter].type = 2;
            else 
            {
                ERROR_LOG("wrong switch brand name.");
                continue;
                //return -1;//wrong switch brand 
            }
        }
        else 
        {
            ERROR_LOG("switch type is NULL.");
            continue;
            //return -1;
        }
        counter++;
    }
    tmp_switch_group.switch_num = counter;
    p_switch_group_set->push_back(tmp_switch_group);
    return 0;
}
/** 
* @brief   从数据库中获取交换机接口的报警信息
* @param   p_if_alarm_map     保存交换机对应的报警信息
* @param   db_conn            数据库连接
* @return  0-success -1-failed
*/
int db_get_interface_alarm_info(if_alarm_map_t *p_if_alarm_map, c_mysql_iface* db_conn)
{
    if(p_if_alarm_map == NULL || db_conn == NULL) return -1;

    char select_sql[MAX_STR_LEN] = {'\0'};
    sprintf(select_sql, 
            "SELECT distinct speed,warning_val,critical_val,normal_interval,\
            retry_interval,max_attempt FROM t_switch_interface_speed");

    MYSQL_ROW row = NULL;
    if(db_conn->select_first_row(&row, select_sql) <= 0)
    {
        ERROR_LOG("db_conn->select_first_row failed.SQL:[%s] db error is:[%s]",
                select_sql, db_conn->get_last_errstr());
        return -1;
    }

    alarm_info_t alarm_info;
    unsigned int speed;
    unsigned int ret; 
    while(NULL != row)
    {
        memset(&alarm_info, 0, sizeof(alarm_info));
        alarm_info.op = OP_GT;
        speed = 0;
        ret = 0;

        if(row[0] && is_integer(row[0]))
        {
            speed = atoi(row[0]);
        }
        else
        {
            ERROR_LOG("interface speed is null or not correct.");
            goto next;
        }

        //百分比
        if(row[1] && is_integer(row[1]))
        {
            ret = atoi(row[1]);
            if(ret > 0 && ret <= 100)
                alarm_info.warning_val = speed * (BITS_PER_MBIT / 100) * ret;
            else 
                goto next;
        }
        else 
        {
            ERROR_LOG("the warning value is null or not correct.");
            goto next;
        }

        //百分比
        if(row[2] && is_integer(row[2]))
        {
            ret = atoi(row[2]);
            if(ret > 0 && ret <= 100)
                alarm_info.critical_val = speed * (BITS_PER_MBIT / 100) * ret;
            else 
                goto next;
        }
        else 
        {
            ERROR_LOG("the critical value is null or not correct.");
            goto next;
        }

        if(row[3] && is_integer(row[3]))
        {
            alarm_info.normal_interval = atoi(row[3]);
        }
        else 
        {
            ERROR_LOG("the normal_interval is null or not correct.");
            goto next;
        }

        if(row[4] && is_integer(row[4]))
        {
            alarm_info.retry_interval = atoi(row[4]);
        }
        else 
        {
            ERROR_LOG("the retry_interval is null or not correct.");
            goto next;
        }

        if(row[5] && is_integer(row[5]))
        {
            alarm_info.max_atc = atoi(row[5]);
        }
        else 
        {
            ERROR_LOG("the max_atc is null or not correct.");
            goto next;
        }

        if(alarm_info.warning_val > alarm_info.critical_val)
        {
            ERROR_LOG("wrong threshold values:[warning=%.0lf,critical=%.0lf,operation=\">\"]",
                    alarm_info.warning_val, alarm_info.critical_val);
            goto next;
        }

        if(alarm_info.normal_interval < alarm_info.retry_interval)
        {    
            ERROR_LOG("error alarm config :normal interval[%u],retry interval[%u],max attempt count[%u]", 
                    alarm_info.normal_interval, alarm_info.retry_interval, alarm_info.max_atc);
            goto next;
        }   

        p_if_alarm_map->insert(pair<unsigned int, alarm_info_t>(speed, alarm_info));
next:
        row = db_conn->select_next_row(false);
    }

    return 0;
}

/** 
* @brief   更新数据库中一个switch的一个interface的信息
* @param   db_conn    数据库连接
* @param   switch_id  switch id
* @param   if_idx     interface index
* @param   if_status  interface status
* @param   if_desc    interface desc
* @return  0-success -1-failed
*/
int db_update_switch_interface_info(c_mysql_iface *db_conn, unsigned int switch_id, unsigned int if_idx, unsigned int if_status, const char *if_desc, unsigned int if_type, unsigned int if_speed, unsigned int add_time)
{
    if(db_conn == NULL || if_idx == 0 || if_status == 0 || if_desc == NULL || if_type == 0) return -1;

    if(db_conn->execsql(
                "INSERT INTO t_switch_interface_info(switch_id,if_idx,if_status,if_desc,if_type,if_speed,add_time)\
                VALUES(%u,%u,%u,'%s',%u,%u,%u) ON DUPLICATE KEY UPDATE if_status=%u,if_desc='%s',\
                if_type=%u,if_speed=%u,add_time=%u",
                switch_id, if_idx, if_status, if_desc, if_type, if_speed, add_time, if_status, if_desc,
                if_type, if_speed, add_time) < 0)
    {   
        ERROR_LOG("Could not update the switch %u's %u interface_info in db,db error:%s", 
                switch_id, if_idx, db_conn->get_last_errstr());
        return -1;
    } 
    return 0;
}

/** 
* @brief   删除数据库中一个switch的过期的interface的信息
* @param   db_conn    数据库连接
* @param   switch_id  switch id
* @param   add_time   最近一次更新的时间
* @return  0-success -1-failed
*/
int db_delete_overdue_switch_interface_info(c_mysql_iface *db_conn, unsigned int switch_id, unsigned int add_time)
{
    if(db_conn == NULL) return -1;

    if(db_conn->execsql("DELETE FROM t_switch_interface_info WHERE add_time<%u AND switch_id=%u",
                add_time, switch_id) < 0)
    {   
        ERROR_LOG("Could not delete the switch %u's overdue interface_info in db,db error:%s", 
                switch_id, db_conn->get_last_errstr());
        return -1;
    } 
    return 0;
}
