#ifndef  __GF_CGF_REWARD_H__
#define  __GF_CGF_REWARD_H__
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_reward: Ctable
{
	public:
		Cgf_reward(mysql_interface * db ); 

        int insert_reward_player(uint32_t userid, uint32_t role_regtime, uint32_t reward_id);
        int get_reward_player_list(gf_get_reward_player_out_item ** plist, uint32_t *pcount);
        int set_reward_flag(uint32_t userid, uint32_t role_regtime, uint32_t key);

	private:

};
#endif   /* ----- #ifndef __GF_CGF_SUMMON_H__  ----- */
