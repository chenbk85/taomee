/** 
 * ========================================================================
 * @file itl_timer.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-11
 * Modify $Date: 2013-01-08 11:34:28 +0800 (二, 08  1月 2013) $
 * Modify $Author: (local) $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_ITL_TIMER_H_2012_10_11
#define H_ITL_TIMER_H_2012_10_11



extern "C"
{
#include <libtaomee/list.h>
#include <libtaomee/timer.h>
}



struct timer_head_t
{
    timer_head_t()
    {
        DEBUG_LOG("init timer_head_t");
        INIT_LIST_HEAD(&timer_list);
    }
    ~timer_head_t()
    {
        DEBUG_LOG("fini timer_head_t");
        list_del_init(&timer_list);
    }
    list_head_t timer_list;
};




#endif
