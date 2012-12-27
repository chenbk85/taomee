/*
 * =====================================================================================
 *
 *       Filename:  Cadmin_power_pet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 13时46分59秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "Cadmin_power.h"
#include <algorithm>
using namespace std;
//user_pet
Cadmin_power::Cadmin_power(mysql_interface * db ) 
	:Ctable( db,"ADMIN_DB" ,"t_admin_power" )
{ 

}
int  Cadmin_power::get_powerlist(userid_t  adminid,
			   	uint32_t *p_count, power_item ** pp_list  )
{
	sprintf( this->sqlstr,  "select powerid from %s \
		 	where adminid=%u limit 0,1000 " , 
		this->get_table_name(),adminid ); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		(*pp_list+i)->powerid=atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

int  Cadmin_power::get_adminlist_by_powerid(uint32_t powerid ,
			   	uint32_t *p_count, stru_id ** pp_list )
{
	sprintf( this->sqlstr,  "select adminid from %s \
		 	where powerid=%u limit 0,1000 " , 
		this->get_table_name(),powerid ); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		(*pp_list+i)->id=atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}


int Cadmin_power::add_powerid( userid_t  adminid, uint32_t powerid )
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u)", 
		this->get_table_name(),adminid,powerid );
	STD_SET_RETURN_EX (this->sqlstr, ADMIN_POWERID_EXISTED_ERR );
}
int Cadmin_power::del_powerid( userid_t  adminid, uint32_t powerid )
{
	sprintf( this->sqlstr, "delete   from  %s where adminid=%u and powerid=%u ", 
		this->get_table_name(),adminid,powerid );
	STD_SET_RETURN_EX (this->sqlstr, ADMIN_POWERID_NOFIND_ERR );
}
int Cadmin_power::del_by_adminid( userid_t  adminid  )
{
	sprintf( this->sqlstr, "delete   from  %s where adminid=%u ", 
		this->get_table_name(),adminid);
	STD_SET_RETURN_EX (this->sqlstr, ADMIN_POWERID_NOFIND_ERR );
}
