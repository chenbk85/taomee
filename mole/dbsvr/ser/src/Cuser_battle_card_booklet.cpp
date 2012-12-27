/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_card_booklet.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/20/2011 05:20:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_battle_card_booklet.h"


Cuser_battle_card_booklet::Cuser_battle_card_booklet(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_battle_card_booklet", "userid")
{

}

int Cuser_battle_card_booklet::get_count(userid_t userid, uint32_t cardid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u and cardid = %u",
			this->get_table_name(userid),
			userid,
			cardid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();

}

int Cuser_battle_card_booklet::get_variety_count(userid_t userid, uint32_t type, uint32_t &count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and type = %u",
			this->get_table_name(userid),
			userid,
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();

}
int Cuser_battle_card_booklet::add_card(userid_t userid, uint32_t cardid, uint32_t type, uint32_t count,
		uint32_t limit, uint32_t *if_first)
{
	if(count == 0)	
		return SUCC;
	uint32_t old_count = 0;
	int ret = get_count(userid, cardid, old_count); 
	if(ret == SUCC){
		if( limit <= old_count){
			*if_first = 0;
			return 0;
		}
		sprintf(this->sqlstr, "update %s set count = count + %u where userid = %u and cardid = %u",
				this->get_table_name(userid),
				count,
				userid,
				cardid
				);
	 *if_first = old_count + count;
	}
	else{
		sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, 1)",
				this->get_table_name(userid),
				userid,
				cardid,
				type
				);
		*if_first = 1;
	}

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	
}

int Cuser_battle_card_booklet::get_all(userid_t userid, uint32_t *p_count, user_battle_get_card_booklet_out_item 
		**pp_list)
{
	sprintf(this->sqlstr, "select cardid, count from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_list, p_count) ;
		(*pp_list+i)->cardid=atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

