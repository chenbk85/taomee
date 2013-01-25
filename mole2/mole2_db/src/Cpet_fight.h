#ifndef CPET_FIGHT_INC
#define CPET_FIGHT_INC
#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"
#include <time.h>
#include "db_error.h"
#include "Ccommon.h"

class Cpet_fight:public Ctable {
	public:
		Cpet_fight(mysql_interface *db);

 		int add_scores(userid_t userid, uint32_t scores);
		int get_scores(userid_t userid, uint32_t &scores);
		int get_ranks(std::vector<stru_petfight_rank_t> &ranks);
		int get_user_rank(userid_t userid, uint32_t &last_rank, uint32_t &rank);
	private:
		int get_my_last_rank(userid_t userid, uint32_t &last_rank);
		int update_last_rank(userid_t userid, uint32_t last_rank);
};
#endif /* ----- #ifndef CPET_FIGHT_INC  ----- */
