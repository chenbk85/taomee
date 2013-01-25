#ifndef  CHOME_H
#define  CHOME_H

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Chome : public CtableRoute100x10
{

public:
	Chome(mysql_interface * db);

	int insert(uint32_t userid, stru_mole2_home_info_t* p_in);

	int insert(userid_t userid);

	int info_get(uint32_t userid, stru_mole2_home_info_t* p_out);

	int flag_set(uint32_t userid, uint32_t index,uint32_t flag);

protected:

};

#endif

