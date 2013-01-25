#include "svr_proto.h"
#include "center.h"
#include "event.h"
#include "box.h"
#include "battle.h"
#include "maze.h"
#include "beast.h"
#include "mc_proto.h"
#include "activity.h"
#include "items.h"
#include "sns.h"
#include "cli_login.h"
#include "homemap.h"


/**
  * @brief intitiate the keepalive timer
  */
void regist_timers()
{
	ADD_ONLINE_TIMER(&g_events, n_send_sw_keepalive_pkg, 0, 1);
	ADD_ONLINE_TIMER(&g_events, n_start_kick_all_users, 0, get_period_midnight());

	for (uint32_t loop = 0; loop < MAX_USER_WAIT; loop ++) {
		INIT_LIST_HEAD(&user_for_team[loop].timer_list);
	}
}

#define REGISTER_TIMER_TYPE(nbr_, cb_) \
		do { \
			if (register_timer_callback(nbr_, cb_) == -1) \
				ERROR_RETURN(("register timer type error\t[%u]", nbr_), -1); \
		} while(0)

int init_all_timer_type()
{
	REGISTER_TIMER_TYPE(n_connect_to_switch_timely, connect_to_switch_timely);
	REGISTER_TIMER_TYPE(n_send_sw_keepalive_pkg, send_sw_keepalive_pkg);
	REGISTER_TIMER_TYPE(n_kick_all_users_offline, kick_all_users_offline);
	REGISTER_TIMER_TYPE(n_start_kick_all_users, start_kick_all_users);
	REGISTER_TIMER_TYPE(n_kick_user_timeout, kick_user_timeout);
	REGISTER_TIMER_TYPE(n_update_cloth_duration, update_cloth_duration);
	REGISTER_TIMER_TYPE(n_send_cli_keepalive_pkg, send_cli_keepalive_pkg);
	REGISTER_TIMER_TYPE(n_activate_one_box, activate_one_box);
	REGISTER_TIMER_TYPE(n_kick_out_team_animation, kick_out_team_animation);
	REGISTER_TIMER_TYPE(n_maze_state_change, maze_state_change);
	REGISTER_TIMER_TYPE(n_refresh_beast_grp, refresh_beast_grp);
	REGISTER_TIMER_TYPE(n_map_state_change, map_state_change);
	REGISTER_TIMER_TYPE(n_reset_water_cnt, reset_water_cnt);
	REGISTER_TIMER_TYPE(n_hangup_get_exp, hangup_get_exp);
	REGISTER_TIMER_TYPE(n_refresh_elite_beast, refresh_elite_beast);
	REGISTER_TIMER_TYPE(n_check_users_again, check_users_again);
	REGISTER_TIMER_TYPE(n_release_copy_as_noenter, release_copy_as_noenter);
	REGISTER_TIMER_TYPE(n_auto_add_energy, auto_add_energy);
	REGISTER_TIMER_TYPE(n_noti_close_shop, noti_close_shop);
	REGISTER_TIMER_TYPE(n_lucky_star_timer, lucky_star_add_time);
	return 0;
}

