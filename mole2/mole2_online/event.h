#ifndef MOLE2_EVENT_H
#define MOLE2_EVENT_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/list.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/types.h>
#ifdef __cplusplus
}
#endif


enum {
	n_connect_to_switch_timely = 1,
	n_send_sw_keepalive_pkg,		
	n_kick_all_users_offline,	
	n_start_kick_all_users,		
	n_kick_user_timeout,			
	n_send_cli_keepalive_pkg,	
	n_activate_one_box,			
	n_update_cloth_duration,		
	n_kick_out_team_animation,	
	n_maze_state_change,			
	n_refresh_beast_grp,		
	n_map_state_change,		
	n_reset_water_cnt,
	n_hangup_get_exp,
	n_refresh_elite_beast,
	n_activate_grasses,
	n_check_users_again,
	n_release_copy_as_noenter,
	n_auto_add_energy,
	n_noti_close_shop,
	n_lucky_star_timer,
};


#define ADD_ONLINE_TIMER(owner, nbr_, data_, last_time_)  \
	ADD_TIMER_EVENT_EX((owner), (nbr_), (data_), get_now_tv()->tv_sec + (last_time_))

void regist_timers();
int init_all_timer_type();



#endif
