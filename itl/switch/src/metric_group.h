/** 
 * ========================================================================
 * @file metric_group.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-10
 * Modify $Date: 2012-08-14 12:13:30 +0800 (Tue, 14 Aug 2012) $
 * Modify $Author: (local) $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */
#ifndef H_METRIC_H_2012_07_10
#define H_METRIC_H_2012_07_10

#include "metric.h"
#include "switch.h"

class c_metric_group
{
public:
    c_metric_group();
    ~c_metric_group();

    int init(uint32_t collect_interval, c_switch *p_switch);
    int uninit();
    int push_metric(c_metric *p_metric);
    int do_group_collect_and_send();

public:
    inline uint32_t get_collect_interval()
    {
        return m_collect_interval;
    }

    inline c_switch *get_swich_handler()
    {
        return m_p_switch;
    }

private:
    bool m_inited;
    uint32_t m_collect_interval;
    c_switch *m_p_switch;               /**<group所属switch*/
    std::vector<c_metric *> m_metric;   /**<group所包含的metric列表*/
};


#define metric_group_mgr c_single_container(uint32_t, c_metric_group)

int init_switch_metric_group(Cmessage * c_out);

int fini_metric_group();

#endif
