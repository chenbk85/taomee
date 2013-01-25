/** 
 * ========================================================================
 * @file head.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-27
 * Modify $Date: 2012-08-06 17:27:33 +0800 (一, 06  8月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_HEAD_H_2012_07_27
#define H_HEAD_H_2012_07_27


#include "itl_common.h"
#include "node.h"

class c_head
{
    public:

        int m_head_fd;


    public:


};


#define head_mgr c_single_container(int, c_head)


int alarm_get_relative_metric(c_metric_alarm * p_alarm, uint32_t metric_id);

#endif
