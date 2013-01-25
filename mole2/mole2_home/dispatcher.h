#ifndef MOLE2_DISPATCHER_H
#define	MOLE2_DISPATCHER_H

#include "proto.h"
#include "global.h"



enum {
	/*! len must equal to another given len */
	cmp_must_eq		= 1,
	/*! len must be greater or equal to another given len */
	cmp_must_ge		= 2,
};



enum {
	proto_begin				= 0,
	proto_enter_home			= 1,
	proto_send_self			= 3,

	proto_cli_leave_home		= 1003,
	proto_cli_get_user_info	= 1005,
	proto_cli_walk				= 1009,
	proto_cli_action			= 1011,
	proto_cli_set_user_nick	= 1023,
	proto_cli_busy_state		= 1030,

	proto_join_group			= 1352,
	proto_cancel_group		= 1353,
	proto_challenge_pos		= 1354,
	proto_play_card 			= 1355,
	proto_attack_boss			= 1356,
	proto_game_start			= 1357,
	proto_update_score		= 1358,
	proto_be_attacked			= 1359,
	proto_challenge_result	= 1360,
	proto_game_over 			= 1361,
	proto_get_positions_info = 1362,
	proto_leave_positions		= 1363,

	proto_get_version		= 4096,
	proto_max				= 50000,	
};

enum {
	err_invalid_home_map		=	300001,
	err_invalid_position		=	300002,
	err_position_is_busy		= 	300003,
	err_position_freezen		=	300004,
	err_position_level		=	300005
};

int dispatcher(void* data, fdsession_t* fdsess);
void init_cli_handle_funs();


#endif

