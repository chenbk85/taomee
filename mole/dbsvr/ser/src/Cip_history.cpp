/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_gen_vip.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/08/2009 09:21:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cip_history.h"


Cip_history :: Cip_history(mysql_interface *db)
	              : CtableRoute10x10(db, "IP_HISTORY", "t_ip_history", "userid")
{

}


int Cip_history :: update_last_ip(userid_t userid, uint32_t ip)
{
	sprintf(this->sqlstr, "update %s set ip1 = %u  where userid = %u and game = 0",
			this->get_table_name(userid),
			ip,
			userid
			);
	STD_SET_RETURN_EX (this->sqlstr, DB_ERR);

}

int Cip_history :: update(userid_t userid, uint32_t which_game, uint32_t ip1, uint32_t ip2, uint32_t ip3)
{
	sprintf(this->sqlstr, "update %s set ip1 = %u, ip2 = %u, ip3 = %u where userid = %u and game = %u",
			this->get_table_name(userid),
			ip1,
			ip2,
			ip3,
			userid,
			which_game
			);
	STD_SET_RETURN_EX (this->sqlstr, DB_ERR);
}

int Cip_history :: add_ip(userid_t userid, uint32_t which_game, uint32_t ip)
{
	if (which_game==0 ) 
		return ENUM_OUT_OF_RANGE_ERR;

	three_ip_history item = {};
	bool save_flag=false;
	int ret = this->get_ip(userid, which_game, &item);
	if (ret != SUCC) {
		ret = this->insert(userid, which_game, ip);
		save_flag=true;
	} else {
		if (item.add_ip(ip)	){
			ret = this->update(userid, which_game, item.ip[0], item.ip[1], item.ip[2]);
			save_flag=true;
		}
	}
	//更新 last 
	if (save_flag)	{
		ret=this->update_last_ip(userid,ip );
		if (ret!=SUCC){
			ret = this->insert(userid,0,ip);
		}
	}

	return SUCC;
}

int Cip_history :: insert(userid_t userid, uint32_t which_game, uint32_t ip)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, 0, 0)",
			this->get_table_name(userid),
			userid,
			which_game,
			ip
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

int Cip_history :: get_last_ip(userid_t userid,  uint32_t *p_ip )
{
	sprintf(this->sqlstr, "select ip1 from %s where userid = %u order by game  limit 1",
			this->get_table_name(userid), userid);
	 STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_ip);
	 STD_QUERY_ONE_END();
}


int Cip_history :: get_ip(userid_t userid, uint32_t which_game, three_ip_history *p_out)
{
	sprintf(this->sqlstr, "select ip1, ip2, ip3 from %s where userid = %u and game = %u",
			this->get_table_name(userid),
			userid,
			which_game
			);
	 STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->ip[0]);
		INT_CPY_NEXT_FIELD(p_out->ip[1]);
		INT_CPY_NEXT_FIELD(p_out->ip[2]);
	 STD_QUERY_ONE_END();
}

