/*
 * =========================================================================
 *
 *        Filename: timer_func.h
 *
 *        Version:  1.0
 *        Created:  2011-05-17 10:40:18
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#ifndef  TIMER_FUNC_H
#define  TIMER_FUNC_H
#include <stdint.h>

enum {
	n_sw_report_online_info=1,
	n_deal_cahce_cmd=2,
	n_deal_all_off_line=3,
	n_deal_game_timer=4,
	n_noti_45min_online=5,
	n_noti_total_oltime=6,
	n_reset_island_time=7,
};

void sw_report_online_info(uint32_t key, void*data,int data_len);
void init_timer_funcs();
void deal_all_off_line (uint32_t key, void*data,int data_len);
void reset_island_time(uint32_t key, void*data,int data_len);

#endif  /*TIMER_FUNC_H*/
