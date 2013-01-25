#ifndef  CCLI_BUFF_INC
#define  CCLI_BUFF_INC

#include "CtableRoute.h"
#include "proto.h"
#include "db_error.h"
#include "Ccommon.h"

class Ccli_buff:public CtableRoute
{
public:
	Ccli_buff(mysql_interface *db);

	int set(uint32_t userid, mole2_set_user_cli_buff_in *p_in);

	int get_range(uint32_t userid,uint32_t min,uint32_t max,mole2_get_user_cli_buff_out * p_out );
};

#endif

