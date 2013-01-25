#ifndef  CVAL_INC
#define  CVAL_INC

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"
#include <time.h>
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"

class Cval : public Ctable
{

	public:
		    Cval(mysql_interface * db);
			int update_val(uint32_t userid,uint32_t limit, uint32_t &rt);
			int get_val(std::vector<stru_usr_val> &vals);
			int get_val(uint32_t userid, uint32_t &count);
			int check_val_day(uint32_t userid, uint32_t &val, uint32_t &day);
};

#endif
