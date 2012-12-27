/*
 * =====================================================================================
 *
 *       Filename:  Cuser_ocean.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/22/2012 09:58:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_ocean.h"
#include "common.h"
#include "proto.h"


uint32_t ocean_exp[] = {
	10 ,40 ,90 ,160 ,250 ,360 ,490 ,640 ,810 ,1000 ,1210 ,1440 ,1690 ,1960 ,
	2250 ,2560 ,2890 ,3240 ,3610 ,4000 ,4410 ,4840 ,5290 ,5760 , 6250 ,6760 ,
	7290 ,7840 ,8410 ,9000 ,9610 ,10240 ,10890 ,11560 ,12250 ,12960 ,13690 ,14440 ,
	15210 ,16000 ,16810 ,17640 ,18490 ,19360 ,20250 ,21160 ,22090 ,23040 ,24010 ,
	25000 ,26010 ,27040 ,28090 ,29160 ,30250 ,31360 ,32490 ,33640 ,34810 ,36000 ,
	37210 ,38440 ,39690 ,40960 ,42250 ,43560 ,44890 ,46240 ,47610 ,49000 ,50410 ,
	51840 ,53290 ,54760 ,56250 ,57760 ,59290 ,60840 ,62410 ,64000 ,	
};

uint32_t Cuser_ocean::get_level(uint32_t exp)
{
	uint32_t k = 0;
	for(; k < sizeof(ocean_exp)/sizeof(uint32_t); ++k){
		if(exp < ocean_exp[k]){
			break;
		}	
	}

	if(k  >= sizeof(ocean_exp)/sizeof(uint32_t)){
		return  100;
	}	
	else{
		return k+1;
	}
}

uint32_t Cuser_ocean::get_upgrade_exp(uint32_t level)
{
	return ocean_exp[level-1];
}

Cuser_ocean::Cuser_ocean(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_ocean", "userid")
{

}

int Cuser_ocean::insert(userid_t userid, uint32_t oceanid, uint32_t siren)
{
	furnish_diy_t box;
	memset(&box, 0, sizeof(furnish_diy_t));
	box.ground = 1633005; 

	char furnish_my[mysql_str_len(sizeof(furnish_diy_t))];
	memset(furnish_my, 0, sizeof(furnish_my));
	set_mysql_string(furnish_my, (char*)&box, sizeof(furnish_diy_t));
	
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, 10, '%s')",
			this->get_table_name(userid),
			userid,
			oceanid,
			siren,
			furnish_my
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_ocean::get_current_ocean_count(userid_t userid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);	

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();	
}

int Cuser_ocean::get_diy(userid_t userid, uint32_t oceanid, furnish_diy_t* diy)
{
	sprintf(this->sqlstr, "select furnish from %s where userid = %u and oceanid = %u",
			this->get_table_name(userid),
			userid,
			oceanid
			);	

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(diy, sizeof(furnish_diy_t));
	STD_QUERY_ONE_END();	
}

int Cuser_ocean::add(userid_t userid, uint32_t siren)
{
	uint32_t cur_cnt = 0;
	this->get_current_ocean_count(userid, cur_cnt);
	int ret = this->insert(userid, cur_cnt+1, siren);
	return ret;
}

int Cuser_ocean::diy(userid_t userid, uint32_t oceanid, diy_t& furnish, uint32_t& itemid)
{
	itemid = 0;

	furnish_diy_t box;
	memset(&box, sizeof(furnish_diy_t), 0);

	int ret = this->get_diy(userid, oceanid, &box);
	if(ret != 0){
		return -1;
	}
	if(furnish.x == -1 && furnish.y == -1 && furnish.z == -1){
		itemid = box.ground;
		box.ground = furnish.itemid;
	}
	else{
		uint32_t k = 0;
		for(; k < box.count; ++k){
			if(box.furnishes[k].x == furnish.x && box.furnishes[k].y == furnish.y 
					&& box.furnishes[k].z == furnish.z){
				itemid = box.furnishes[k].itemid;
				box.furnishes[k].itemid = furnish.itemid;
				break;
			}	
		}
		if(k >= box.count && box.count < FURNISH_CNT){
			box.furnishes[box.count].itemid =  furnish.itemid;
			box.furnishes[box.count].x = furnish.x;
			box.furnishes[box.count].y= furnish.y;
			box.furnishes[box.count].z= furnish.z;
			++box.count;
		}
		else if( box.count == FURNISH_CNT){
			return -2;//cannot diy as reached diy limit number
		}
	}

	this->update(userid, oceanid, &box);
	return 0;
}

int Cuser_ocean::update(userid_t userid, uint32_t oceanid, furnish_diy_t* diy)
{
	char furnish_my[mysql_str_len(sizeof(furnish_diy_t))];
	memset(furnish_my, sizeof(furnish_my), 0);
	set_mysql_string(furnish_my, (char*)diy, sizeof(furnish_diy_t));

	sprintf(this->sqlstr, "update %s set furnish = '%s' where userid = %u and oceanid = %u",
			this->get_table_name(userid),
			furnish_my,
			userid,
			oceanid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_ocean::get_all(userid_t userid, uint32_t oceanid, furnish_diy_t *diy, uint32_t *capacity)
{
	sprintf(this->sqlstr, "select capacity, furnish from %s where userid = %u and oceanid = %u",
			this->get_table_name(userid),
			userid,
			oceanid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*capacity);
		BIN_CPY_NEXT_FIELD(diy, sizeof(furnish_diy_t));
	STD_QUERY_ONE_END();	
}

int Cuser_ocean::get_one_col(userid_t userid, uint32_t oceanid, const char* col, uint32_t *value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u and oceanid = %u",
			col,
			this->get_table_name(userid),
			userid,
			oceanid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*value);
	STD_QUERY_ONE_END();	
}

int Cuser_ocean::update_one_col(userid_t userid, uint32_t oceanid, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u and oceanid = %u",
			this->get_table_name(userid),
			col,
			value,
			userid,
			oceanid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
