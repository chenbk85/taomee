#ifndef CGF_SECONDARY_PRO_H
#define CGF_SECONDARY_PRO_H

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

#define DEFAULT_MAX_SECONDARY_PRO_EXP 0xfffffffe
class Cgf_secondary_pro: public CtableRoute
{
public:
	Cgf_secondary_pro(mysql_interface * db );
	int get_player_secondary_pro_info( userid_t userid, uint32_t role_regtime, uint32_t type, uint32_t* exp);
	int add_player_secondary_pro_exp(userid_t userid, uint32_t role_regtime, uint32_t type, uint32_t exp, uint32_t max_exp_limit);
	int insert_player_secondary_pro(userid_t userid, uint32_t role_regtime, uint32_t type, uint32_t exp = 0);
	int get_player_secondary_pro_list(userid_t userid, uint32_t role_regtime, gf_get_secondary_pro_list_out_type** pData, uint32_t* count);
	int clear_player_secondary_pro(userid_t userid, uint32_t role_regtime);
	int update_player_secondary_pro(userid_t userid, uint32_t role_regtime, uint32_t type, uint32_t exp);
	int clear_role_info(userid_t userid,uint32_t role_regtime);
};

#endif
