/*
 * =====================================================================================
 *
 *       Filename:  timer_func.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/28/2011 08:09:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */

#include "timer_func.h"
#include "dispatcher.h"
#include "dirty_word.h"
#include "global.h"


#define REGISTER_TIMER_TYPE(nbr_, cb_) \
        do { \
            if (register_timer_callback(nbr_, cb_) == -1) \
                ERROR_RETURN(("register timer type error\t[%u]", nbr_), -1); \
        } while(0)

int load_dirty_timely(void* owner, void* data)
{   
	g_dirty_word->reload();
	ADD_ONLINE_TIMER(&g_events, n_load_dirty_timely, 0, g_dirty_word->reload_time);
    return 0;
} 

int init_all_timer_type()
{   
	INIT_LIST_HEAD(&g_events.timer_list);
    REGISTER_TIMER_TYPE(n_load_dirty_timely, load_dirty_timely);
    return 0;
}


int regist_timers()
{
	ADD_ONLINE_TIMER(&g_events, n_load_dirty_timely, 0, g_dirty_word->reload_time);
	return 0;
}
