/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_ocean_info.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/2012 07:18:34 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_ocean_info.h"
#include <sstream>
uint32_t ocean_ex[] = {
	10 ,40 ,90 ,160 ,250 ,360 ,490 ,640 ,810 ,1000 ,1210 ,1440 ,1690 ,1960 ,
	2250 ,2560 ,2890 ,3240 ,3610 ,4000 ,4410 ,4840 ,5290 ,5760 , 6250 ,6760 ,
	7290 ,7840 ,8410 ,9000 ,9610 ,10240 ,10890 ,11560 ,12250 ,12960 ,13690 ,14440 ,
	15210 ,16000 ,16810 ,17640 ,18490 ,19360 ,20250 ,21160 ,22090 ,23040 ,24010 ,
	25000 ,26010 ,27040 ,28090 ,29160 ,30250 ,31360 ,32490 ,33640 ,34810 ,36000 ,
	37210 ,38440 ,39690 ,40960 ,42250 ,43560 ,44890 ,46240 ,47610 ,49000 ,50410 ,
	51840 ,53290 ,54760 ,56250 ,57760 ,59290 ,60840 ,62410 ,64000 ,	
};

uint32_t Csysarg_ocean_info::get_level(uint32_t exp)
{
	uint32_t k = 0;
	for(; k < sizeof(ocean_ex)/sizeof(uint32_t); ++k){
		if(exp < ocean_ex[k]){
			break;
		}	
	}

	if(k  >= sizeof(ocean_ex)/sizeof(uint32_t)){
		return  100;
	}	
	else{
		return k+1;
	}
}

Csysarg_ocean_info::Csysarg_ocean_info(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_ocean_info")
{

}

int Csysarg_ocean_info::insert(userid_t userid, uint32_t exp)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u)",
			this->get_table_name(),
			userid,
			exp
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Csysarg_ocean_info:: update(userid_t userid, uint32_t exp)
{
	sprintf(this->sqlstr, "update %s set exp = %u  where userid = %u",
			this->get_table_name(),
			exp,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_ocean_info:: add(userid_t userid, uint32_t exp)
{
	int m_ret = update(userid, exp);
	if(m_ret != SUCC){
		m_ret = insert(userid, exp);
	}
	return m_ret;
}

int Csysarg_ocean_info::select_ocean_friend(userid_t userid, uint32_t in_count,
	   	sysarg_get_ocean_friend_in_item *p_in_list, 	uint32_t *out_count, 
		sysarg_get_ocean_friend_out_item **pp_out_list)
{
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_list + i)->friendid;
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	//in_str <<  ',' << userid;
	sprintf(this->sqlstr, "select userid, exp from %s where userid  in (%s) order by exp desc",
			this->get_table_name(),
			in_str.str().c_str()
		   );

	uint32_t exp = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_list, out_count);
		INT_CPY_NEXT_FIELD((*pp_out_list + i)->friendid);
		INT_CPY_NEXT_FIELD(exp);
		(*pp_out_list + i)->level = get_level(exp);
		exp = 0;
	STD_QUERY_WHILE_END();

}


