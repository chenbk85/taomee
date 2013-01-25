/** 
 * ========================================================================
 * @file metric_alarm.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-13
 * Modify $Date: 2012-10-11 15:42:56 +0800 (四, 11 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_METRIC_ALARM_H_2012_07_13
#define H_METRIC_ALARM_H_2012_07_13

#include <stdint.h>
#include <map>
#include <set>

#include "proto.h"
#include "itl_common.h"
#include "mobile_ban.h"

class c_node;
class c_metric;


class c_metric_alarm
{
    public:

        c_metric_alarm();

        ~c_metric_alarm();

        // 根据已发告警次数，获取发送间隔
        uint32_t get_span(uint32_t alarmed_count);

        // 给定时间，判断是否屏蔽告警
        bool is_alarm_off(uint32_t time = 0);

        bool is_reach_warn_level(double v);

        bool is_reach_critical_level(double v);

        bool is_ban(uint32_t time = 0);

        uint32_t get_warn_way() const
        {
            if (m_warn_way & ALARM_WAY_MOBILE)
            {
                if (is_mobile_ban())
                {
                    // 需要把短信告警屏蔽
                    return (m_warn_way ^ ALARM_WAY_MOBILE);
                }
            }

            return m_warn_way;
        }

        uint32_t get_crit_way() const 
        {
            if (m_crit_way & ALARM_WAY_MOBILE)
            {
                if (is_mobile_ban())
                {
                    return (m_crit_way ^ ALARM_WAY_MOBILE);
                }
            }

            return m_crit_way;
        }


    public:

        char m_arg[MAX_METRIC_ARG_LEN];
        c_value m_value;

    public:

        // 是否采集失败
        // 0初始状态，从未失败过
        // 1上一次采集失败
        // 2上一次采集成功
        uint8_t m_fail;

        // metric的状态，正常or非正常
        bool m_normal;
        // 连续不正常的次数
        uint32_t m_abnormal_count;
        // 连续正常的次数
        uint32_t m_normal_count;

        // 告警的级别，ALARM_WARN / ALARM_CRITICAL
        uint32_t m_alarm_level;
        // 已发告警的次数
        uint32_t m_alarm_count;
        // 上一次发出告警的时间戳
        uint32_t m_last_alarm_time;
        // 本次告警开始的时间戳
        uint32_t m_alarm_start_time;


    public:

        // 对应的node
        c_node * m_node;
        // 对应的metric
        c_metric * m_metric;

    public:

        // 是否有告警配置
        bool m_has_alarm_config;

        // 告警阀值
        double m_warn_val_coeff;
        // 与告警阀值计算相关的metric alarm
        // NULL表示不需要
        c_metric_alarm * m_warn_alarm;
        uint32_t m_warn_metric_id;
        // 告警方式
        uint32_t m_warn_way;


        // 严重告警阀值
        double m_crit_val_coeff;
        // 与严重告警阀值计算相关的metric alarm
        // NULL表示不需要
        c_metric_alarm * m_crit_alarm;
        uint32_t m_crit_metric_id;
        // 严重告警方式
        uint32_t m_crit_way;

        // 阀值算子
        uint32_t m_op;

        // 从不正常转换到正常需要的连续正常次数
        uint32_t m_normal_threshold;
        // 从正常转换到不正常需要的连续不正常次数
        uint32_t m_abnormal_threshold;

        // 告警发送节奏
        std::map< uint32_t, uint32_t > m_span_map;

        // 屏蔽告警的时间点
        std::set< uint32_t > m_off_time;

        // 告警控制台中屏蔽的起止时间戳
        uint32_t m_ban_start_time;
        uint32_t m_ban_end_time;



        char m_mobile_contact[CONTACT_LIST_LEN];
        char m_email_contact[CONTACT_LIST_LEN];
        char m_rtx_contact[CONTACT_LIST_LEN];



};


c_metric_alarm * alloc_metric_alarm(c_metric * p_metric, const char * p_arg);

void dealloc_metric_alarm(void * p_alarm);


int init_metric_alarm(c_metric_alarm * p_alarm, alarm_metric_t * p_metric_config);


c_metric_alarm * find_metric_alarm(GHashTable * p_table, const char * p_arg);



double get_metric_value(const c_metric_alarm * p_alarm);

bool compare_value(double v1, double v2, uint32_t op);

uint32_t compare_metric_value(c_metric_alarm * p_alarm);



double get_threshold(double coeff, c_metric_alarm * p_alarm);


#endif
