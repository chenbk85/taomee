/** 
 * ========================================================================
 * @file metric_group.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-10
 * Modify $Date: 2012-10-25 11:26:03 +0800 (四, 25 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */



#ifndef H_METRIC_H_2012_07_10
#define H_METRIC_H_2012_07_10

#include "metric.h"
#include "itl_timer.h"




class c_metric_group
{
    public:

        c_metric_group()
        {
            m_collect_interval = 0;
        }

        uint32_t m_group_id;
        uint32_t m_collect_interval;

        timer_struct_t * m_timer;
        std::vector<c_metric_arg *> m_metric_arg;

};


#define metric_group_mgr c_single_container(uint32_t, c_metric_group)



int init_server_metric_group(Cmessage * c_out);


int init_mysql_metric_group(Cmessage * c_out);

int fini_metric_group();

#endif
