/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_paradise_rank.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/01/2011 09:44:54 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */



#include "Csysarg_paradise_rank.h"
#include <sstream>
#include "proto.h"

//static level_exp_info level_scope[] ={
        //{ 1, {0,  65} },
        //{ 2, {65, 145 } },
        //{ 3, {145, 240} },
        //{ 4, {240, 350} },
        //{ 5, {350, 475} },
        //{ 6, {475, 615} },
        //{ 7, {615, 770} },
        //{ 8, {770, 940} },
        //{ 9, {940, 1125} },
        //{ 10, {1125, 1325} },
        //{ 11, {1325, 1540} },
        //{ 12, {1540, 1770} },
        //{ 13, {1770, 2015} },
        //{ 14, {2015, 2275} },
        //{ 15, {2275, 2550} },
        //{ 16, {2550, 2840} },
        //{ 17, {2840, 3145} },
        //{ 18, {3145, 3465} },
        //{ 19, {3465, 3800} },
        //{ 20, {3800, 9999999} },
//};

static uint32_t level_boundary[] = {
	65, 145, 240, 350, 475, 615, 770, 940, 1125, 1325, 1540, 1770, 2015, 
		2275, 2550, 2840, 3145, 3465, 3800
};


Csysarg_paradise_rank::Csysarg_paradise_rank(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_paradise_rank")
{

}

int Csysarg_paradise_rank::insert(userid_t userid, uint32_t exp, uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(),
			userid,
			exp,
			count
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Csysarg_paradise_rank::select(userid_t userid, uint32_t count, sysarg_get_paradise_friend_rank_in_item  
		*p_in_item, uint32_t *p_out_count,sysarg_get_paradise_friend_rank_out_item **pp_out_item)
{
	std::ostringstream in_str;
	for(uint32_t i = 0; i < count; ++i){
		in_str <<(p_in_item + i)->userid;
		if(i < count -1){
			in_str << ',';
		}
	}
	in_str<< ','<< userid;

	sprintf(this->sqlstr, "select userid, exp, count from %s where userid in(%s) order by exp desc",
			this->get_table_name(),
			in_str.str().c_str()	
			);
	uint32_t exp = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_out_count);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->userid);
		INT_CPY_NEXT_FIELD(exp);
		(*pp_out_item + i)->level = cal_level(exp);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->count);
	STD_QUERY_WHILE_END();

}

int Csysarg_paradise_rank::select(userid_t count,sysarg_get_paradise_access_rank_in_item 
		*p_in_item, uint32_t *p_out_count, sysarg_get_paradise_access_rank_out_item **pp_out_item)
{
	std::ostringstream in_str;
	for(uint32_t i = 0; i < count; ++i){
		in_str <<(p_in_item + i)->userid;
		if(i < count -1){
			in_str << ',';
		}
	}
	sprintf(this->sqlstr, "select userid, exp, count from %s where userid in(%s) order by exp desc",
			this->get_table_name(),
			in_str.str().c_str()	
			);
	uint32_t exp = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_out_count);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->userid);
		INT_CPY_NEXT_FIELD(exp);
		(*pp_out_item + i)->level = cal_level(exp);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->count);
	STD_QUERY_WHILE_END();
}

int Csysarg_paradise_rank::cal_level(uint32_t exp)
{
	//for(uint32_t i = 0; i< sizeof(level_scope)/sizeof(level_exp_info); ++ i){
		//if(exp >= level_scope[i].range.start && exp < level_scope[i].range.end){
			//return level_scope[i].level;
		//}
	//}
	uint32_t k = 0, level = 0;
	for(; k < sizeof(level_boundary)/sizeof(uint32_t); ++ k){
		if(exp < level_boundary[k]){
			level = k + 1;
			break;
		}
	}
	if(k >= sizeof(level_boundary)/sizeof(uint32_t)){
		level = 20;
	}
	return level;
}

int Csysarg_paradise_rank::select(userid_t userid, char *column, uint32_t &value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u",
			column,
			this->get_table_name(),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(value);
	STD_QUERY_ONE_END();

}


int Csysarg_paradise_rank::update(userid_t userid, uint32_t exp, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set exp = %u, count = %u where userid = %u",
			this->get_table_name(),
			exp,
			count,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}


int Csysarg_paradise_rank::update(userid_t userid, char *column, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u",
			this->get_table_name(),
			column,
			value,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
