/*
 * =====================================================================================
 *
 *       Filename:  Creg_userid.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/17/2010 11:06:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "Creg_userid.h"
#include "benchapi.h"
Creg_userid::Creg_userid (mysql_interface * db ):Ctable( db, "USERID_DB","t_reg_userid" )
{

}

int Creg_userid::set_used(uint32_t userid)
{
	sprintf( this->sqlstr, "update %s set useflag=1 where userid=%u " ,
		this->get_table_name(), userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Creg_userid::set_noused(uint32_t userid)
{
	sprintf( this->sqlstr, "update %s set useflag=0 where userid=%u " ,
		this->get_table_name(), userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}


int Creg_userid::get(uint32_t *p_userid)
{
	int ret;
	//得到一个userid
	ret= this->get_db(p_userid );
	if(ret!=SUCC) return ret;
	//设置其已经使用了
	return this->set_used(*p_userid );
}
int Creg_userid::get_db(uint32_t *p_userid)
{
	sprintf( this->sqlstr, "select  userid from %s where useflag=0 limit 1 for update", 
		 this->get_table_name());
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(*p_userid);
	STD_QUERY_ONE_END();
}

