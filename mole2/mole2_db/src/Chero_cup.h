#ifndef  CHERO_CUP_INC
#define  CHERO_CUP_INC

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"
#include <time.h>
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"

class Chero_cup : public Ctable
{
public:
	Chero_cup(mysql_interface * db);

	int set_teamid(userid_t userid,uint32_t teamid);
	int add_medals(userid_t userid,uint32_t medals);
	int add_team_users(userid_t teamid);
	int get_teams_users(std::vector<stru_team_rank_t> &teams);
	int get(userid_t userid,uint32_t &teamid,uint32_t &medals);
	int get_team_rank(std::vector<stru_team_rank_t> &ranks);
	int get_user_rank(std::vector<stru_user_rank_t> &ranks);
};

#endif /* ----- #ifndef CHERO_CUP_INC  ----- */

