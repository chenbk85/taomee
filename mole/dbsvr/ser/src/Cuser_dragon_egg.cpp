/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_dragon_egg.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年12月22日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  zheng, zheng@taomee.com
 *
 * =====================================================================================
 */

#include "Cuser_dragon_egg.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

	struct egg_time_max_tag {
		uint32_t eggid;
		uint32_t limit;
	} egg_time[] = {
		{190681,60},
		{190690,60*60*8},
		{190708,60*60*8},
		{190709,60*60*4},
		{190710,60*60*6},
		{190711,60*60*6},
		{190712,60*60*8},
		{190713,60*60*8},
		{190714,60*60*12},
		{190715,60*60*10},
		{190716,60*60*10}
	};

Cuser_dragon_egg:: Cuser_dragon_egg(mysql_interface * db )
        :CtableRoute100x10( db, "USER", "t_user_dragon_egg", "userid")
{
}

uint32_t Cuser_dragon_egg::get_item_growth(uint32_t itemid)
{
	struct item_growth_egg {
        uint32_t itemid;
		uint32_t growth;
	} item_tab[] = {
		{190674,20*60},			    
		{190685,90*60}		    
	};

    int i = 0;
	int count = sizeof(item_tab) / sizeof(item_tab[0]);
	while(i < count ) {
		if(item_tab[i].itemid == itemid) return item_tab[i].growth;
		i++;
	}

    return 0;
}

uint32_t Cuser_dragon_egg::get_egg_time_limit(uint32_t dragon_egg_id)
{
	int i = 0;
	int count = sizeof(egg_time) / sizeof(egg_time[0]);

	while(i < count) {
		if(egg_time[i].eggid == dragon_egg_id) {
			return egg_time[i].limit;
		}
		i++;
	}

	return 0xFFFFFFFF;
}

int Cuser_dragon_egg::check_egg_is_hatched(uint32_t dragon_egg_id,uint32_t start)
{
	int i = 0;
	int count = sizeof(egg_time) / sizeof(egg_time[0]);

	uint32_t hatchtime = time(NULL) - start;

	while(i < count) {
		if(egg_time[i].eggid == dragon_egg_id) {
			if(egg_time[i].limit <= hatchtime) {
				return 1;
			}
			return 0;
		}
		i++;
	}

	return 0;
}

int Cuser_dragon_egg::add(userid_t userid, uint32_t dragon_egg_id)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
		this->get_table_name(userid),userid,dragon_egg_id,(uint32_t)time(NULL));
    STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_dragon_egg::del(userid_t userid, uint32_t dragon_egg_id)
{
    sprintf(this->sqlstr, "delete from %s where userid=%u",
		this->get_table_name(userid),userid);
    STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_dragon_egg::get_egg_hatch(userid_t userid, uint32_t &dragon_egg_id, uint32_t &time)
{
    sprintf(this->sqlstr, "select eggid,time from %s where userid=%u",
		this->get_table_name(userid),userid);
    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(dragon_egg_id);
        INT_CPY_NEXT_FIELD(time);
    STD_QUERY_ONE_END();
}

int Cuser_dragon_egg::add_egg_growth(userid_t userid, uint32_t dragon_egg_id,uint32_t growth)
{
    sprintf(this->sqlstr, "update %s set time=time - %u where userid=%u",
		this->get_table_name(userid),growth,userid);
    STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

