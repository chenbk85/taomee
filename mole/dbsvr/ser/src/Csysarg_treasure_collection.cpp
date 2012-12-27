/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_treasure_collection.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2011 04:17:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include "Csysarg_treasure_collection.h"
#include <sstream>
#include "proto.h"

uint32_t level_boundary [] ={
	48, 138, 268, 454, 712, 1058, 1508, 2078, 2784, 3642, 4668, 5878, 
				7288, 8914, 10772, 12878, 15248, 17898, 20844
};

int Csysarg_treasure_collection::get_level(uint32_t exp, uint32_t &level)
{
	uint32_t k = 0;
	for(; k < sizeof(level_boundary)/sizeof(uint32_t); ++k){
		if(exp < level_boundary[k]){
			level = k + 1;
			break;
		}
	}	
	if(k >=  sizeof(level_boundary)/sizeof(uint32_t)){
		level = 20;
	}
	return 0;	
}

Csysarg_treasure_collection::Csysarg_treasure_collection(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_treasure")
{

}

int Csysarg_treasure_collection::insert(userid_t userid, uint32_t exp, uint32_t piglet_level)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(),
			userid,
			exp,
			piglet_level
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Csysarg_treasure_collection:: update(userid_t userid, uint32_t exp)
{
	sprintf(this->sqlstr, "update %s set exp = if(%u > exp ,%u, exp)  where userid = %u",
			this->get_table_name(),
			exp,
			exp,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_treasure_collection::update_piglet_level(userid_t userid, uint32_t level)
{
	sprintf(this->sqlstr, "update %s set pig_level = if(%u > pig_level ,%u, pig_level)  where userid = %u",
            this->get_table_name(),
            level,
            level,
            userid
            );
    
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_treasure_collection::select_visit(uint32_t in_count, 
		sysarg_get_treasure_visit_in_item *p_in_item, uint32_t *out_count,
			   sysarg_get_treasure_visit_out_item **pp_out_item)
{
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_item + i)->userid;
		if(i < in_count-1){
			in_str << ',';
		}	
	}//for

	sprintf(this->sqlstr, "select userid, exp from %s where userid in (%s) order by exp desc",
			this->get_table_name(),
			in_str.str().c_str()
			);
	uint32_t exp = 0, level = 0;

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, out_count);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->userid);
		INT_CPY_NEXT_FIELD(exp);
		get_level(exp, level);
		(*pp_out_item + i)->level = level;
	STD_QUERY_WHILE_END();

}
int Csysarg_treasure_collection::select_friend(userid_t userid, uint32_t in_count,
	   	sysarg_get_treasure_friend_in_item *p_in_list, 	uint32_t *out_count, 
		sysarg_get_treasure_friend_out_item **pp_out_list)
{
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_list + i)->userid;
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	in_str <<  ',' << userid;
	sprintf(this->sqlstr, "select userid, exp from %s where userid  in (%s) order by exp desc",
			this->get_table_name(),
			in_str.str().c_str()
		   );

	uint32_t exp = 0, level = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_list, out_count);
		INT_CPY_NEXT_FIELD((*pp_out_list + i)->userid);
		INT_CPY_NEXT_FIELD(exp);
		get_level(exp, level);
		(*pp_out_list + i)->level = level;
	STD_QUERY_WHILE_END();

}

int Csysarg_treasure_collection::select_piglet_house_friend(userid_t userid, uint32_t in_count,
	   	sysarg_get_treasure_friend_in_item *p_in_list, 	uint32_t *out_count, 
		sysarg_get_treasure_friend_out_item **pp_out_list)
{
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_list + i)->userid;
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	//in_str <<  ',' << userid;
	sprintf(this->sqlstr, "select userid, pig_level from %s where userid  in (%s) order by pig_level desc",
			this->get_table_name(),
			in_str.str().c_str()
		   );

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_list, out_count);
		INT_CPY_NEXT_FIELD((*pp_out_list + i)->userid);
		INT_CPY_NEXT_FIELD((*pp_out_list + i)->level);
	STD_QUERY_WHILE_END();

}


