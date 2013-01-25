/** 
 * ========================================================================
 * @file timer.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-06
 * Modify $Date: 2012-07-12 10:06:44 +0800 (Thu, 12 Jul 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_TIMER_H_2012_07_06
#define H_TIMER_H_2012_07_06


extern "C"
{
#include <libtaomee/list.h>
#include <libtaomee/timer.h>
}



struct timer_head_t
{
    list_head_t timer_list;
};


extern timer_head_t g_event;

extern timer_head_t g_collect_timer;

#endif
