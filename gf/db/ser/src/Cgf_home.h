#ifndef _CGF_HOME_H_
#define _CGF_HOME_H_

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_home: public CtableRoute
{
public:
	Cgf_home(mysql_interface * db);
public:
	int get_player_home_data(userid_t userid, uint32_t role_regtime, gf_get_home_data_out* out);
	int set_player_home_data(userid_t userid, uint32_t role_regtime, gf_set_home_data_in * in);
	int set_player_home_update_tm(userid_t userid, uint32_t role_regtime, uint32_t last_update_tm);
	int insert_player_home_data(userid_t userid, uint32_t role_regtime);

	int clear_role_info(userid_t userid,uint32_t role_regtime);
//	int replace_player_home_level_data(userid_t userid, uint32_t role_regtime, int level);
//	int replace_player_home_exp_data(userid_t userid, uint32_t role_regtime, int exp);
//	int replace_player_home_exp_level_data(userid_t userid, uint32_t role_regtime, int exp, int level);
};


#endif
