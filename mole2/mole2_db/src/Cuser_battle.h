#ifndef  CUSER_BATTLE_INC
#define  CUSER_BATTLE_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cbattle : public CtableRoute100x10
{

public:
	Cbattle (mysql_interface * db);

	int insert(userid_t userid);

	int insert(uint32_t userid, stru_mole2_user_battle_info_1* p_in);

	int info_set(uint32_t userid, stru_mole2_user_battle_info_1* p_in);

	int info_set_ex(uint32_t userid, stru_mole2_user_battle_info_1* p_in);

	int info_get(uint32_t userid, stru_mole2_user_battle_info* p_out);
	int get_online_login(uint32_t userid, stru_battle_info &p_out);
};

#endif
