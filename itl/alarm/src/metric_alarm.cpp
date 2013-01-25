/** 
 * ========================================================================
 * @file metric_alarm.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-13
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include <string.h>
#include <math.h>
#include <errno.h>

extern "C"
{
#include <libtaomee/timer.h>
}
#include "metric_alarm.h"
#include "itl_common.h"
#include "node.h"
#include "proto.h"
#include "head.h"
#include "db.h"


using namespace std;


const uint32_t g_default_normal_threshold = config_get_intval("normal_threshold", 5);
const uint32_t g_default_abnormal_threshold = config_get_intval("abnormal_threshold", 2);


c_metric_alarm::c_metric_alarm()
{
    memset(m_arg, 0, sizeof(m_arg));
    m_value.reset();
    m_fail = 0;


    m_normal = true;
    m_alarm_level = ALARM_NORMAL;
    m_abnormal_count = 0;
    m_normal_count = 0;

    m_alarm_count = 0;
    m_last_alarm_time = 0;
    m_alarm_start_time = 0;

    m_node = NULL;
    m_metric = NULL;

    m_has_alarm_config = false;

    m_warn_val_coeff = 0;
    m_warn_alarm = NULL;
    m_warn_metric_id = 0;
    m_warn_way = 0;

    m_crit_val_coeff = 0;
    m_crit_alarm = NULL;
    m_crit_metric_id = 0;
    m_crit_way = 0;

    // 不比较阀值
    m_op = OP_BEGIN;


    m_normal_threshold = 0;
    m_abnormal_threshold = 0;

    m_span_map.clear();
    m_off_time.clear();

}


c_metric_alarm::~c_metric_alarm()
{
    m_span_map.clear();
    m_off_time.clear();
}


uint32_t c_metric_alarm::get_span(uint32_t alarmed_count)
{
    if (0 == alarmed_count)
    {
        return 0;
    }
    
    uint32_t key = alarmed_count;
    uint32_t span = 0;
    iterator_t(m_span_map) it = m_span_map.find(key);
    if (m_span_map.end() == it)
    {
        // 其他未指明的alarmed_count就按照0处理
        it = m_span_map.find(0);
        if (m_span_map.end() == it)
        {
            // 使用默认的
            span = 60 * 60;
        }
        else
        {
            span = it->second;
        }
    }
    else
    {
        span = it->second;
    }
    return span;
}


bool c_metric_alarm::is_ban(uint32_t time)
{
    if (0 == time)
    {
        time = get_now_tv()->tv_sec;
    }

    if (0 == m_ban_start_time && 0 == m_ban_end_time)
    {
        return false;
    }

    if (0 != m_ban_start_time && 0 == m_ban_end_time)
    {
        if (m_ban_start_time <= time)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    if (0 == m_ban_start_time && 0 != m_ban_end_time)
    {
        if (time <= m_ban_end_time)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    if (m_ban_start_time <= time && time <= m_ban_end_time)
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool c_metric_alarm::is_alarm_off(uint32_t time)
{
    if (0 == time)
    {
        time = get_now_tv()->tv_sec;
    }

    uint32_t day_time = get_day_timestamp(time);

    // 当前时间跟今天0点之差
    uint32_t interval = time - day_time;

    bool flag = false;

    set_for_each(m_off_time, it)
    {
        uint32_t check_point = *it;
        if (interval < check_point 
         || (interval == check_point && flag))
        {
            continue;
        }
        else
        {
            flag = !flag;
        }
    }

    return flag;
}


bool compare_value(double v1, double v2, uint32_t op)
{

    switch (op)
    {
        case OP_EQ:
            if (fabs(v1 - v2) < 1e-8)
            {
                return true;
            }
            break;
        case OP_GT:
            if (v1 > v2)
            {
                return true;
            }
            break;
        case OP_LT:
            if (v1 < v2)
            {
                return true;
            }
            break;
        case OP_GE:
            if (v1 >= v2)
            {
                return true;
            }
            break;
        case OP_LE:
            if (v1 <= v2)
            {
                return true;
            }
            break;
        default:
            break;
    }

    return false;
}


bool c_metric_alarm::is_reach_warn_level(double v)
{
    if (!m_has_alarm_config)
    {
        return false;
    }

    double warn_val = 0;

    if (0 == m_warn_metric_id)
    {
        warn_val = m_warn_val_coeff;
    }
    else
    {
        if (NULL != m_warn_alarm)
        {
            warn_val = m_warn_val_coeff * get_metric_value(m_warn_alarm);
        }
        else
        {
            c_metric * p_warn_metric = m_node->m_metric->get_object(m_warn_metric_id);
            if (NULL == p_warn_metric)
            {
                // crit发过了，warn这边就不要再发了
                if (m_warn_metric_id != m_crit_metric_id)
                {
                    alarm_get_relative_metric(this, m_warn_metric_id);
                }
                return false;
            }

            m_warn_alarm = find_metric_alarm(p_warn_metric->m_alarm, m_arg);
            if (NULL == m_warn_alarm)
            {
                if (m_warn_metric_id != m_crit_metric_id)
                {
                    alarm_get_relative_metric(this, m_warn_metric_id);
                }
                return false;
            }
            else
            {
                warn_val = m_warn_val_coeff * get_metric_value(m_warn_alarm);

            }

        }
    }

    if (fabs(warn_val) < 1e-8)
    {
        // 计算出来的阀值几乎为0，则视为没有阀值，不做判断
        return false;
    }

    return compare_value(v, warn_val, m_op);
}


bool c_metric_alarm::is_reach_critical_level(double v)
{
    if (!m_has_alarm_config)
    {
        return false;
    }

    double crit_val = 0;

    if (0 == m_crit_metric_id)
    {
        crit_val = m_crit_val_coeff;
    }
    else
    {
        if (NULL != m_crit_alarm)
        {
            crit_val = m_crit_val_coeff * get_metric_value(m_crit_alarm);
        }
        else
        {
            c_metric * p_crit_metric = m_node->m_metric->get_object(m_crit_metric_id);
            if (NULL == p_crit_metric)
            {
                alarm_get_relative_metric(this, m_warn_metric_id);
                return false;
            }

            m_crit_alarm = find_metric_alarm(p_crit_metric->m_alarm, m_arg);
            if (NULL == m_crit_alarm)
            {
                alarm_get_relative_metric(this, m_crit_metric_id);
                return false;
            }
            else
            {
                crit_val = m_crit_val_coeff * get_metric_value(m_crit_alarm);

            }

        }
    }

    if (fabs(crit_val) < 1e-8)
    {
        // 计算出来的阀值几乎为0，则视为没有阀值，不做判断
        return false;
    }


    return compare_value(v, crit_val, m_op);
}


void pure_alarm_off_time(char * p_str)
{
    char * p = p_str;
    char * q = NULL;
    while (NULL != (q = strchr(p, ',')))
    {
        *q = '-';
        p = q + 1;
    }
}


int init_alarm_off_time(c_metric_alarm * p_alarm, char * p_str)
{
    pure_alarm_off_time(p_str);

    char * str_1 = p_str;
    char * save_ptr_1 = NULL;
    char * token_1 = NULL;
    while (NULL != (token_1 = strtok_r(str_1, ";", &save_ptr_1)))
    {
        str_1 = NULL;
        uint32_t start = 0;
        uint32_t end = 0;

        char * str_2 = token_1;
        char * save_ptr_2 = NULL;

        // 取开始时间点
        char * token_2 = strtok_r(str_2, "-", &save_ptr_2);
        if (NULL != token_2)
        {
            uint32_t hour = 0;
            uint32_t minute = 0;
            char * p = strchr(token_2, ':');
            if (NULL == p)
            {
                continue;
            }
            *p = 0;
            hour = atoi(token_2);
            minute = atoi(++p);

            start = hour * 3600 + minute * 60;

        }

        // 取结束时间点
        str_2 = NULL;
        token_2 = strtok_r(str_2, "-", &save_ptr_2);
        if (NULL != token_2)
        {

            uint32_t hour = 0;
            uint32_t minute = 0;
            char * p = strchr(token_2, ':');
            if (NULL == p)
            {
                continue;
            }
            *p = 0;
            hour = atoi(token_2);
            minute = atoi(++p);

            end = hour * 3600 + minute * 60;

        }

        if (start != end)
        {
            p_alarm->m_off_time.insert(start);
            p_alarm->m_off_time.insert(end);
        }

        if (start > end)
        {
            // 跨0点
            p_alarm->m_off_time.insert(0);
            p_alarm->m_off_time.insert(24 * 3600);
        }

    }

    if (p_alarm->m_off_time.empty())
    {
        // 没有解析到合适的配置，则给默认的：0-6点
        p_alarm->m_off_time.insert(0);
        p_alarm->m_off_time.insert(6 * 3600);
    }

    return 0;
}



int init_alarm_span(c_metric_alarm * p_alarm, char * p_str)
{

    char * str_1 = p_str;
    char * save_ptr_1 = NULL;
    char * token_1 = NULL;
    while (NULL != (token_1 = strtok_r(str_1, ";", &save_ptr_1)))
    {
        str_1 = NULL;
        uint32_t start = 0;
        uint32_t end = 0;
        uint32_t interval = 0;

        char * str_2 = token_1;
        char * save_ptr_2 = NULL;

        // 取次数
        char * token_2 = strtok_r(str_2, ":", &save_ptr_2);
        if (NULL == token_2)
        {
            continue;
        }

        char * p = strchr(token_2, '-');
        if (NULL == p)
        {
            continue;
        }
        *p = 0;
        start = atoi(token_2);
        end = atoi(++p);



        // 取间隔
        str_2 = NULL;
        token_2 = strtok_r(str_2, "-", &save_ptr_2);
        if (NULL == token_2)
        {
            continue;
        }

        // 原始数据单位为分钟，转换成秒
        interval = atoi(token_2) * 60;

        if (0 == end)
        {
            p_alarm->m_span_map[start] = interval;
            p_alarm->m_span_map[end] = interval;
        }
        else
        {
            for (uint32_t i = start; i <= end; i++)
            {
                p_alarm->m_span_map[i] = interval;
            }

        }


    }

    return 0;
}


int get_metric_and_coeff(char * str, uint32_t * p_metric_id, double * p_coeff)
{
    if (NULL == str || NULL == p_metric_id || NULL == p_coeff)
    {
        return -1;
    }

    char * p = strchr(str, '*');
    char * q = strchr(str, '$');
    if (NULL == p || NULL == q)
    {
        *p_metric_id = 0;
        *p_coeff = atof(str);
        return 0;
    }

    *p = 0;
    p++;
    q++;
    if (p < q)
    {
        // coeff * $metric_id
        *p_coeff = atof(str);
        *p_metric_id = atoi(q);
    }
    else
    {
        // $metric_id * coeff
        *p_coeff = atof(p);
        *p_metric_id = atoi(q);
    }
    return 0;

}


// 解析$26*0.5这种字段，同时支持0.5*$26
int init_alarm_value(c_metric_alarm * p_alarm, alarm_metric_t * p_alarm_config)
{

    get_metric_and_coeff(p_alarm_config->warning_val, &p_alarm->m_warn_metric_id, &p_alarm->m_warn_val_coeff);
    if (0 == p_alarm->m_warn_metric_id)
    {
        p_alarm->m_warn_alarm = NULL;
    }
    else
    {

        c_metric * p_warn_metric = p_alarm->m_node->m_metric->get_object(p_alarm->m_warn_metric_id);
        if (NULL == p_warn_metric)
        {
            p_alarm->m_warn_alarm = NULL;
        }
        else
        {
            p_alarm->m_warn_alarm = find_metric_alarm(p_warn_metric->m_alarm, p_alarm->m_arg);

        }
    }



    get_metric_and_coeff(p_alarm_config->critical_val, &p_alarm->m_crit_metric_id, &p_alarm->m_crit_val_coeff);
    if (0 == p_alarm->m_crit_metric_id)
    {
        p_alarm->m_crit_alarm = NULL;
    }
    else
    {

        c_metric * p_crit_metric = p_alarm->m_node->m_metric->get_object(p_alarm->m_crit_metric_id);
        if (NULL == p_crit_metric)
        {
            p_alarm->m_crit_alarm = NULL;
        }
        else
        {
            p_alarm->m_crit_alarm = find_metric_alarm(p_crit_metric->m_alarm, p_alarm->m_arg);

        }
    }


    return 0;
}



int init_metric_alarm(c_metric_alarm * p_alarm, alarm_metric_t * p_alarm_config)
{
    DEBUG_LOG("\t\toff time: %s\tspan: %s", p_alarm_config->no_alarm_range, p_alarm_config->alarm_frequency);

    p_alarm->m_op = p_alarm_config->op;
    p_alarm->m_crit_way = p_alarm_config->critical_way;
    p_alarm->m_warn_way = p_alarm_config->warning_way;
    p_alarm->m_normal_threshold = p_alarm_config->normal_interval;
    if (0 == p_alarm->m_normal_threshold)
    {
        p_alarm->m_normal_threshold = g_default_normal_threshold;
    }
    p_alarm->m_abnormal_threshold = p_alarm_config->abnormal_interval;
    if (0 == p_alarm->m_abnormal_threshold)
    {
        p_alarm->m_abnormal_threshold = g_default_abnormal_threshold;
    }
    p_alarm->m_ban_start_time = p_alarm_config->shield_time.start;
    p_alarm->m_ban_end_time = p_alarm_config->shield_time.end;


    init_alarm_off_time(p_alarm, p_alarm_config->no_alarm_range);

    init_alarm_span(p_alarm, p_alarm_config->alarm_frequency);

    init_alarm_value(p_alarm, p_alarm_config);



    if (!config_get_intval("use_default_contact_for_metric", 0))
    {
        STRNCPY_LEN(p_alarm->m_mobile_contact, &(p_alarm_config->alarm_contact), mobile_list, CONTACT_LIST_LEN);
        STRNCPY_LEN(p_alarm->m_email_contact, &(p_alarm_config->alarm_contact), email_list, CONTACT_LIST_LEN);
        STRNCPY_LEN(p_alarm->m_rtx_contact, &(p_alarm_config->alarm_contact), rtx_list, CONTACT_LIST_LEN);

    }
    else
    {
        STRNCPY(p_alarm->m_mobile_contact, g_default_mobile, sizeof(p_alarm->m_mobile_contact));
        STRNCPY(p_alarm->m_email_contact, g_default_email, sizeof(p_alarm->m_email_contact));
        STRNCPY(p_alarm->m_rtx_contact, g_default_rtx, sizeof(p_alarm->m_rtx_contact));
    }

    DEBUG_LOG("\t\talarm, op: %u, normal threshold: %u, abnormal threshold: %u", 
            p_alarm->m_op, 
            p_alarm->m_normal_threshold,
            p_alarm->m_abnormal_threshold);

    DEBUG_LOG("\t\t\twarn way: %u, coeff: %.2f, metric id: %u",
            p_alarm->m_warn_way, 
            p_alarm->m_warn_val_coeff,
            p_alarm->m_warn_metric_id);


    DEBUG_LOG("\t\t\tcrit way: %u, coeff: %.2f, metric id: %u",
            p_alarm->m_crit_way, 
            p_alarm->m_crit_val_coeff,
            p_alarm->m_crit_metric_id);


    DEBUG_LOG("\t\tmobile contact: %s", p_alarm->m_mobile_contact);
    DEBUG_LOG("\t\temail contact: %s", p_alarm->m_email_contact);
    DEBUG_LOG("\t\trtx contact: %s", p_alarm->m_rtx_contact);

    p_alarm->m_has_alarm_config = true;

    // 阀值为0的视为没有告警配置
    if (fabs(p_alarm->m_warn_val_coeff) < 1e-8 && fabs(p_alarm->m_crit_val_coeff) < 1e-8)
    {
        p_alarm->m_has_alarm_config = false;
        p_alarm->m_op = OP_BEGIN;
    }

    c_node * p_node = p_alarm->m_node;
    c_metric * p_metric = p_alarm->m_metric;

    char ip[IP_STRING_LEN] = {0};
    STRNCPY(ip, long2ip(p_node->m_node_ip), IP_STRING_LEN);


    // if (strncmp(ip, "192.168.71", 10) == 0
            // || strncmp(ip, "192.168.11.222", 14) == 0
            // || strncmp(ip, "192.168.11.223", 14) == 0)
    // {
        // if (strncmp(p_metric->m_metric_name, "cpu", 3) == 0)
        // {
            // DEBUG_LOG("blocked alarm: node[%u: %s], metric[%u: %s]",
                    // p_node->m_node_id,
                    // ip,
                    // p_metric->m_metric_id,
                    // p_metric->m_metric_name);
            // p_alarm->m_has_alarm_config = false;
            // p_alarm->m_op = OP_BEGIN;
        // }
    // }

    DEBUG_LOG("\tnode[%u: %s], metric[%u: %s](%s), alarm inited",
            p_node->m_node_id,
            ip,
            p_metric->m_metric_id,
            p_metric->m_metric_name,
            p_alarm->m_arg);
    return 0;
}



c_metric_alarm * alloc_metric_alarm(c_metric * p_metric, const char * p_arg)
{
    c_metric_alarm * p_alarm = new c_metric_alarm();
    if (NULL == p_alarm)
    {
        ERROR_LOG("malloc alarm config failed, %s", strerror(errno));
        return NULL;
    }

    p_alarm->m_metric = p_metric;
    p_alarm->m_node = p_metric->m_node;

    if (ITL_TYPE_MYSQL == p_metric->m_metric_class)
    {
        STRNCPY(p_alarm->m_mobile_contact, p_alarm->m_node->m_mysql_mobile, sizeof(p_alarm->m_mobile_contact));
        STRNCPY(p_alarm->m_email_contact, p_alarm->m_node->m_mysql_email, sizeof(p_alarm->m_email_contact));
        STRNCPY(p_alarm->m_rtx_contact, p_alarm->m_node->m_mysql_rtx, sizeof(p_alarm->m_rtx_contact));

    }
    else
        // else if (ITL_TYPE_MYSQL == p_metric->m_metric_class)
    {
        STRNCPY(p_alarm->m_mobile_contact, p_alarm->m_node->m_server_mobile, sizeof(p_alarm->m_mobile_contact));
        STRNCPY(p_alarm->m_email_contact, p_alarm->m_node->m_server_email, sizeof(p_alarm->m_email_contact));
        STRNCPY(p_alarm->m_rtx_contact, p_alarm->m_node->m_server_rtx, sizeof(p_alarm->m_rtx_contact));
    }

    STRNCPY(p_alarm->m_arg, p_arg, MAX_METRIC_ARG_LEN);

    g_hash_table_insert(p_metric->m_alarm, p_alarm->m_arg, p_alarm);

    return p_alarm;
}


void dealloc_metric_alarm(void * p)
{

    c_metric_alarm * p_alarm = reinterpret_cast< c_metric_alarm * >(p);


    if (!p_alarm->m_normal)
    {
        // 正在告警的，要告诉db告警结束
        p_alarm->m_normal = true;
        db_store_alarm_event_data(p_alarm, "");
    }




    delete p_alarm;

}


c_metric_alarm * find_metric_alarm(GHashTable * p_table, const char * p_arg)
{
    return reinterpret_cast< c_metric_alarm * >(g_hash_table_lookup(p_table, p_arg));
}




double get_metric_value(const c_metric_alarm * p_alarm)
{
    return p_alarm->m_value.get_double();
}


uint32_t compare_metric_value(c_metric_alarm * p_alarm)
{
    double value = get_metric_value(p_alarm);

    if (p_alarm->is_reach_critical_level(value))
    {
        return ALARM_CRITICAL;
    }

    if (p_alarm->is_reach_warn_level(value))
    {
        return ALARM_WARN;
    }

    return ALARM_NORMAL;

}


double get_threshold(double coeff, c_metric_alarm * p_alarm)
{
    if (NULL == p_alarm)
    {
        return coeff;
    }
    else
    {
        return (coeff * get_metric_value(p_alarm));
    }

}

