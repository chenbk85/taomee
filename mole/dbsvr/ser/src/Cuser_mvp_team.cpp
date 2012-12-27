/*
 * =====================================================================================
 *
 *       Filename:  Cuser_mvp_team.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/20/2012 10:59:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_mvp_team.h"

Cuser_mvp_team::Cuser_mvp_team(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_mvp_team", "userid")
{

}

int Cuser_mvp_team::add(userid_t captain, uint32_t memberid)
{
	sprintf(this->sqlstr, "insert into %s values (%u, %u)",
			this->get_table_name(captain),
			captain,
			memberid
			);

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_EXISTED_ERR);	
}

int Cuser_mvp_team::get_count(userid_t captain, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where captain = %u",
			this->get_table_name(captain),
			captain
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}
int  Cuser_mvp_team::remove(userid_t captain, uint32_t memberid)
{
	sprintf(this->sqlstr,"delete from %s where captain = %u and memberid = %u",
			this->get_table_name(captain),captain,memberid);
	STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}

int  Cuser_mvp_team::destory_team(userid_t captain)
{
	sprintf(this->sqlstr,"delete from %s where captain = %u",
			this->get_table_name(captain),captain);
	STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}
int Cuser_mvp_team::get_memberlist(userid_t captain,user_get_teaminfo_out_item **memberlist, uint32_t* count)
{
    sprintf(this->sqlstr, "select memberid from %s where captain = %u",this->get_table_name(captain),captain);
    STD_QUERY_WHILE_BEGIN(this->sqlstr,memberlist,count);
        (*memberlist+i)->member = atoi_safe(NEXT_FIELD);	
	STD_QUERY_WHILE_END();
}
int Cuser_mvp_team::get_memberlist(userid_t captain,user_mvp_deleteall_out_item **memberlist, uint32_t* count)
{
    sprintf(this->sqlstr, "select memberid from %s where captain = %u",this->get_table_name(captain),captain);
    STD_QUERY_WHILE_BEGIN(this->sqlstr,memberlist,count);
        (*memberlist+i)->member = atoi_safe(NEXT_FIELD);	
	STD_QUERY_WHILE_END();
}

