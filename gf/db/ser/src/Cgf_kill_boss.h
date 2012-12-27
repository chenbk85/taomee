#ifndef _CGF_KILL_BOSS_H_
#define _CGF_KILL_BOSS_H_

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"


class Cgf_kill_boss: public CtableRoute
{
public:
	Cgf_kill_boss(mysql_interface * db);
	int get_kill_boss_list(userid_t userid, uint32_t role_regtime, gf_get_kill_boss_list_out_element** pData, uint32_t* count);
	int replace_kill_boss(userid_t userid, uint32_t role_regtime, uint32_t stage_id, uint32_t boss_id, uint32_t kill_time, uint32_t pass_cnt);

	int clear_role_info(userid_t userid,uint32_t role_regtime);
};








#endif
