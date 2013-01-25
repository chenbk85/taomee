/*
 * =====================================================================================
 *
 *       Filename:  timer_func.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/28/2011 08:12:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */
#ifdef __cplusplus
extern "C" {
#endif
#include <libtaomee/timer.h>
#include <libtaomee/log.h>
#ifdef __cplusplus
}
#endif

enum {
	n_load_dirty_timely	= 2,
	n_chat_forbid_pop	= 3,
	n_load_valid_words	= 4,
};

int regist_timers();
int init_all_timer_type();

#define ADD_ONLINE_TIMER(owner, nbr_, data_, last_time_)  \
    ADD_TIMER_EVENT_EX((owner), (nbr_), (data_), get_now_tv()->tv_sec + (last_time_))
