#include <algorithm>
#include "Cgf_card.h"
#include "db_error.h"

using namespace std;
//加入
Cgf_card::Cgf_card(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_card","userid")
{ 

}

int Cgf_card::get_card_list(userid_t userid,
	   						uint32_t user_tm,
						   	gf_get_card_list_out_item **pp_list,
							uint32_t * p_count)
{
	*p_count = 0;
	GEN_SQLSTR(this->sqlstr,"select card_id, card_type, card_set \
		       from %s where userid=%u and role_regtime=%u",
		   	   this->get_table_name(userid),
			   userid,
		       user_tm);	   

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->card_id);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->card_type);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->card_set);
	STD_QUERY_WHILE_END();

}

int Cgf_card::insert_card(userid_t userid,
		                  uint32_t usertm,
						  uint32_t card_type,
						  uint32_t card_set,
						  uint32_t * card_id)
{
	*card_id = 0;
     GEN_SQLSTR(this->sqlstr, "insert into %s (userid, role_regtime, card_type, card_set) \
			 value (%u, %u, %u, %u)", this->get_table_name(userid),
			 userid,
			 usertm,
			 card_type,
			 card_set);
	return exec_insert_sql_get_auto_increment_id(sqlstr,  GF_CARD_INFO_ERR, card_id);
}

int Cgf_card::erase_card(userid_t userid,
		                 uint32_t usertm,
						 uint32_t card_id)
{
	GEN_SQLSTR(this->sqlstr, 
			   "delete from %s where userid=%u and role_regtime=%u and card_id=%u",
			  this->get_table_name(userid),
			  userid,
			  usertm,
			  card_id);

	return this->exec_update_sql(this->sqlstr, GF_CARD_INFO_ERR);
}

int Cgf_card::get_card_cnt(userid_t userid, uint32_t role_regtime, gf_get_player_community_info_out* p_out)
{
    GEN_SQLSTR(this->sqlstr,"select count(*) from %s \
        where userid=%u and role_regtime=%u",
		this->get_table_name(userid),userid, role_regtime);
    STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->card_cnt);
    STD_QUERY_ONE_END();
}

int Cgf_card::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

