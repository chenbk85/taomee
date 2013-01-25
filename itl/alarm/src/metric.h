/** 
 * ========================================================================
 * @file metric.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-13
 * Modify $Date: 2012-10-11 15:42:56 +0800 (四, 11 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_METRIC_H_2012_07_13
#define H_METRIC_H_2012_07_13

extern "C"
{
#include <glib.h>
}

#include "itl_common.h"
#include "metric_alarm.h"


class c_node;

/** 
 * @brief metric有个属性叫状态。
 * 每次收到一个metric_value，就进行一次检查，
 * 如果超过阀值，则计算一次不正常，如果低于阀值，则计算一次正常
 * 连续一定次数的不正常，metric会切换到不正常状态
 * 连续一定次数的正常，metric会切换到正常状态
 *
 * 每次metric进行检查之后，如果metric处于不正常状态，则进行告警判断
 * 是否屏蔽告警，根据告警节奏是否可以告警
 * 确定触发告警之后，记下告警时间和次数，进行告警操作
 * 
 */
class c_metric
{
    public:

        c_metric(c_node * p_node);

        ~c_metric();

        int set_value_type(uint32_t value_type);



    public:

        c_node * m_node;
        // arg和告警配置的表
        GHashTable * m_alarm;

    public:
        
        uint32_t m_metric_id;
        char m_metric_name[MAX_METRIC_NAME_LEN];
        char m_metric_unit[MAX_METRIC_UNIT_LEN];
        char m_metric_fmt[MAX_METRIC_FMT_LEN];
        uint32_t m_metric_class;
        uint32_t m_collect_time;
        uint32_t m_value_type;

};




#endif
