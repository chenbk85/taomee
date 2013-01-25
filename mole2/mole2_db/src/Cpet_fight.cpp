#include "Cpet_fight.h"

Cpet_fight::Cpet_fight(mysql_interface * db)
	:Ctable(db , "RAND_ITEM" , "t_pet_fight")
{

}


int Cpet_fight::add_scores(userid_t userid, uint32_t scores)
{

	GEN_SQLSTR(this->sqlstr, "update %s set scores = scores+%u where usrid = %u",
		this->get_table_name(), userid, scores);
	return this->exec_update_sql(this->sqlstr, MOLE2_KEY_EXIST_ERR);	
}

int Cpet_fight::get_scores(userid_t userid, uint32_t &scores)
{
    GEN_SQLSTR(this->sqlstr, "select scores from %s where userid = %u",
        this->get_table_name(), userid);
    STD_QUERY_ONE_BEGIN(this->sqlstr,MOLE2_KEY_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(scores);
    STD_QUERY_ONE_END();
}

int Cpet_fight::get_ranks(std::vector<stru_petfight_rank_t> &ranks)
{
    GEN_SQLSTR(this->sqlstr, "select userid,scores from %s where order by scores desc limit 20",
        this->get_table_name());
    STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, ranks);
        INT_CPY_NEXT_FIELD(item.userid);
        INT_CPY_NEXT_FIELD(item.scores);
    STD_QUERY_WHILE_END_NEW();
}

int Cpet_fight::get_user_rank(userid_t userid, uint32_t &last_rank, uint32_t &rank)
{
    GEN_SQLSTR(this->sqlstr, "select count(*) from %s where scores > ( select scores from %s where userid = %u)",
        this->get_table_name(), this->get_table_name(), userid);
    STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_KEY_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(rank);
    STD_QUERY_ONE_END();
	get_my_last_rank(userid, last_rank);
	if(last_rank !=rank)
		update_last_rank(userid, rank);
}

int Cpet_fight::update_last_rank(userid_t userid, uint32_t last_rank)
{
    GEN_SQLSTR(this->sqlstr, "update %s set last_rank = %u where userid = %u",
        this->get_table_name(), last_rank, userid);
    return this->exec_update_sql(this->sqlstr, MOLE2_KEY_EXIST_ERR);
}


int Cpet_fight::get_my_last_rank(userid_t userid, uint32_t &last_rank)
{
    GEN_SQLSTR(this->sqlstr, "select last_rank from %s where userid = %u",
        this->get_table_name(), userid);
    STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_KEY_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(last_rank);
    STD_QUERY_ONE_END();
}
