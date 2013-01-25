#ifndef  CRANK_INC
#define  CRANK_INC

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"
#include <time.h>
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"

class Crank : public Ctable
{

	public:
		    Crank(mysql_interface * db);
			int update_rank(uint32_t userid, mole2_user_update_rank_in *p_in);
			int get_top_ten(std::vector<stru_rank_val> &ranks);
			int max(uint32_t *uid,uint32_t *val);
};

#endif
