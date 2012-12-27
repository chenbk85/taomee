/*
 * =====================================================================================
 *
 *       Filename:  Csys_arg.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Csys_fire_cup.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include <algorithm>

//create
Csys_fire_cup::Csys_fire_cup(mysql_interface * db ) : Ctable( db, "SYSARG_DB","t_sysarg_fire_cup")
{ 

}

int Csys_fire_cup :: insert(uint32_t teamid, int32_t value)
{
	if (value < 0) {
		value = 0;
	}
	sprintf(this->sqlstr, "insert into %s values(%u, %u)",
			this->get_table_name(),teamid,value);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);	
}

int Csys_fire_cup::get_team_medal(uint32_t teamid, uint32_t* p_count)
{
	sprintf(this->sqlstr, "select count from %s where teamid = %u",this->get_table_name(), teamid);   
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Csys_fire_cup :: update(uint32_t teamid, int32_t value)
{
	//if (value > 30 || value < -30) {
		//return SUCC;
	//}
	//if (value < 0 && (value != -10 || value != -20 || value != -30)) {
		//return SUCC;
	//}
	//if (value > 0 && (value != 1 || value != 2 || value != 5 || value != 8 || value != 20 || value != 30)) {
		//return SUCC;
	//}

	if (value < 0) {
		uint32_t count = 0;
		int ret = this->get_team_medal(teamid, &count);
		if (ret != SUCC) {
			return ret;
		}

		if ((int32_t)count + value < 0) {
			count = 0;
		} else {
			count += value;
		}
		sprintf(this->sqlstr, "update %s set count = %u where teamid = %u",
				this->get_table_name(), count, teamid);
	} else {
		sprintf(this->sqlstr, "update %s set count = count + %d where teamid = %u",
				this->get_table_name(), value, teamid);
	}
	STD_SET_RETURN_EX(this->sqlstr, KEY_NOFIND_ERR );
}

int Csys_fire_cup :: get_list_medal(sys_get_all_medal_list_out_item **pp_list, uint32_t *p_count )
{
	sprintf(this->sqlstr, "select teamid,count from %s order by count desc",this->get_table_name());   

	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_list+i)->teamid); 
		INT_CPY_NEXT_FIELD((*pp_list+i)->count); 
	STD_QUERY_WHILE_END();
}

