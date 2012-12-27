/*
 * =====================================================================================
 *
 *       Filename:  Cuser_paradise_contract.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/20/2011 05:30:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include<algorithm>
#include<ctime>
#include "proto.h"
#include "common.h"
#include "msglog.h"
#include "Cuser_paradise_contract.h"
#include <sstream>

angle_to_dragon map_table[] ={
	{1353414, 1350013},
	{1353401, 1350014},
	{1353403, 1350015},
	{1353405, 1350016},
	{1353416, 1350017},
	{1354033, 1350018},
	{1354047, 1350019},
	{1354067, 1350020},
	{1354067, 1350020},
	{1354077, 1350021},
	{1354090, 1350022},
	{1354102, 1350023},
	{1354134, 1350024},
	{1354135, 1350025},
	{1354146, 1350026},
	{1354148, 1350027},
	{1354150, 1350028},
	{1354158, 1350029},
	{1354159, 1350030},
	{1354169, 1350032},
	{1354171, 1350033},
	{1354173, 1350034},
	{1354177, 1350035},
	{1354183, 1350036},
	{1354185, 1350037},
	{1354189, 1350038},
	{1354199, 1350039},
	{1354205, 1350040},
	{1354209, 1350041},
	{1354217, 1350042},
	{1354224, 1350044},
	{1354228, 1350045},
	{1354237, 1350046},
	{1354238, 1350047},
};

Cuser_paradise_contract::Cuser_paradise_contract(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_angel_contract", "userid")
{

	this->msglog_file = config_get_strval("MSG_LOG_FILE");
}

int Cuser_paradise_contract::mapping_to_dragon(uint32_t angelid, uint32_t *dragonid)
{
	uint32_t len = sizeof(map_table)/sizeof(angle_to_dragon);
	uint32_t i = 0;
	for(; i< len; ++i){
		if(map_table[i].angelid == angelid){
			*dragonid = map_table[i].dragonid;
			break;
		}
	}
	if(i >= len){
		return ANGELID_MAPPING_TO_DRAGONID_ERR;
	}
	else{
		return SUCC;
	}
}

int Cuser_paradise_contract::mapping_to_angel(uint32_t dragonid, uint32_t *angelid)
{
	uint32_t len = sizeof(map_table)/sizeof(angle_to_dragon);
	uint32_t i = 0;
	for(; i< len; ++i){
		if(map_table[i].dragonid == dragonid){
			*angelid = map_table[i].angelid;
			break;
		}
	}
	if(i >= len){
		return ANGELID_MAPPING_TO_DRAGONID_ERR;
	}
	else{
		return SUCC;
	}
}

int Cuser_paradise_contract::insert(userid_t userid, uint32_t angelid, uint32_t count,
		uint32_t type)
{
	sprintf(this->sqlstr, "insert into %s values (%u, %u, %u, 0, %u, 0)",
			this->get_table_name(userid),
			userid,
			angelid,
			count,
			type
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_paradise_contract::update(userid_t userid, uint32_t angelid, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set count = count + %u where userid = %u and angelid = %u",
			this->get_table_name(userid),
			value,
			userid,
			angelid
			);
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_paradise_contract::update_used_count(userid_t userid, uint32_t angelid, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set used_count =  %u where userid = %u and angelid = %u",
			this->get_table_name(userid),
			value,
			userid,
			angelid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise_contract::update_hurt_count_inc(userid_t userid, uint32_t angelid, uint32_t value)
{
	DEBUG_LOG("value: %d",value);
	sprintf(this->sqlstr, "update %s set hurt_count = hurt_count + %u where userid = %u and angelid = %u",
			this->get_table_name(userid),
			value,
			userid,
			angelid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise_contract::update_hurt_count(userid_t userid, uint32_t angelid, uint32_t value)
{
	DEBUG_LOG("value: %d",value);
	sprintf(this->sqlstr, "update %s set hurt_count = %u where userid = %u and angelid = %u",
			this->get_table_name(userid),
			value,
			userid,
			angelid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise_contract::update_used_count_dragon(userid_t userid, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set used_count =  %u where userid = %u and (type & 0x02) = 0x02",
			this->get_table_name(userid),
			value,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise_contract::clear_used_count_no_dragon(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set used_count = 0 where userid = %u and (type & 0x02) != 0x02 ",
			this->get_table_name(userid),
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}
int Cuser_paradise_contract::get_all(userid_t userid, user_get_angel_contract_all_out_item **pp_list, uint32_t *count)
{
	sprintf(this->sqlstr, "select angelid, count, used_count, hurt_count from %s where userid = %u ",
			this->get_table_name(userid),
			userid
		   );
	uint32_t used_count = 0, hurt_count = 0;
	uint32_t tmp_count = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->angelid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
		INT_CPY_NEXT_FIELD(used_count);
		INT_CPY_NEXT_FIELD(hurt_count);
		tmp_count = (*pp_list + i)->count;
		(*pp_list + i)->count = (tmp_count >(used_count + hurt_count)? (tmp_count -(used_count + hurt_count)):0);
		//(*pp_list + i)->count = tmp_count - (used_count + hurt_count);
	STD_QUERY_WHILE_END();	

}

int Cuser_paradise_contract::get_contract_angel_all_ex(userid_t userid, user_get_angel_contract_all_ex_out_item **pp_list,
		uint32_t *count){
	sprintf(this->sqlstr, "select angelid, count, used_count, hurt_count from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->angelid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->used_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->hurt_count);
	STD_QUERY_WHILE_END();	


}

int Cuser_paradise_contract::get_counts(userid_t userid,  uint32_t **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u and angelid != 0",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD(*(*pp_list + i));
	STD_QUERY_WHILE_END();	
}

int Cuser_paradise_contract::get_by_angelid(userid_t userid, uint32_t angelid, uint32_t *count)
{
	sprintf(this->sqlstr, "select count, used_count,hurt_count from %s where userid = %u and angelid = %u",
			this->get_table_name(userid),
			userid,
			angelid
			);
	uint32_t used_count = 0, hurt_count = 0;	
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*count);
		INT_CPY_NEXT_FIELD (used_count);
		INT_CPY_NEXT_FIELD (hurt_count);
		*count = ((*count > (used_count + hurt_count))? (*count - (used_count + hurt_count)): 0);
		//DEBUG_LOG("^^^^^count: %u, used_count:%u, hurt_count:%u, a:%u", *count, used_count, hurt_count, angelid);
	STD_QUERY_ONE_END();

}
int Cuser_paradise_contract::get_by_angelid_terminate(userid_t userid, uint32_t angelid, uint32_t *count, uint32_t *total)
{
	sprintf(this->sqlstr, "select count, used_count,hurt_count from %s where userid = %u and angelid = %u",
			this->get_table_name(userid),
			userid,
			angelid
			);
	uint32_t used_count = 0, hurt_count = 0;	
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*total);
		INT_CPY_NEXT_FIELD (used_count);
		INT_CPY_NEXT_FIELD (hurt_count);
		*count = ((*total > (used_count + hurt_count))? (*total - (used_count + hurt_count)): 0);
	STD_QUERY_ONE_END();

}

int Cuser_paradise_contract::get_type(userid_t userid, uint32_t angelid, uint32_t *type)
{
	sprintf(this->sqlstr, "select type from %s where userid = %u and angelid = %u",
			this->get_table_name(userid),
			userid,
			angelid
			);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*type);
	STD_QUERY_ONE_END();

}
int Cuser_paradise_contract::get_contract_angel_count(userid_t userid, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u ",
			this->get_table_name(userid),
			userid
			);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*count);
	STD_QUERY_ONE_END();

}
int Cuser_paradise_contract::get_type_used_count(userid_t userid, uint32_t angelid, 
		uint32_t *type, uint32_t *used_count)
{
	sprintf(this->sqlstr, "select type, used_count from %s where userid = %u and angelid = %u",
			this->get_table_name(userid),
			userid,
			angelid
			);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*type);
		INT_CPY_NEXT_FIELD (*used_count);
	STD_QUERY_ONE_END();
}
int Cuser_paradise_contract::get_hurt_count(userid_t userid, uint32_t angelid, uint32_t *count)
{
	sprintf(this->sqlstr, "select hurt_count from %s where userid = %u and angelid = %u",
			this->get_table_name(userid),
			userid,
			angelid
			);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*count);
	STD_QUERY_ONE_END();
}
int Cuser_paradise_contract::get_count_type2(userid_t userid,  uint32_t &count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and type != 2 \
			and type !=3 and used_count =1",
			this->get_table_name(userid),
			userid
			);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (count);
	STD_QUERY_ONE_END();


}
int Cuser_paradise_contract::sign_contract(userid_t userid, uint32_t angelid, uint32_t value , uint32_t type)
{
	uint32_t statistics[]={
		userid, 1
	};
	msglog(this->msglog_file, 0x04043401 + angelid - 1353000, time(NULL), statistics, sizeof(statistics));
	uint32_t ret = update(userid, angelid, value);
	if(ret == USER_ID_NOFIND_ERR){
		
		ret = insert(userid, angelid, value, type);
		return ret;
	}
	return ret;
}

int Cuser_paradise_contract::del_contract_angel(userid_t userid, uint32_t angelid, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = %u where userid = %u and angelid = %u",
			this->get_table_name(userid),
			count,
			userid,
			angelid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise_contract::get_battle_angel_info(uint32_t userid,  user_get_battle_angel_info_out_item **pp_list,
	   uint32_t	*count)
{
	sprintf(this->sqlstr, "select angelid, count, used_count, hurt_count from %s where userid = %u and (type & 0x01) = 0x01",
			this->get_table_name(userid),
			userid
		   );
	uint32_t used_count = 0, hurt_count = 0;
	uint32_t tmp_count = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->angelid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
		INT_CPY_NEXT_FIELD(used_count);
		INT_CPY_NEXT_FIELD(hurt_count);
		tmp_count = (*pp_list + i)->count;
		(*pp_list + i)->count = (tmp_count >(used_count + hurt_count)? (tmp_count -(used_count + hurt_count)):0);
	STD_QUERY_WHILE_END();	

}
int Cuser_paradise_contract::get_angel_in_favorite(uint32_t userid, user_get_angel_favorite_out_item **pp_list,
	   	uint32_t *count)
{
	sprintf(this->sqlstr, "select angelid  from %s where userid = %u and (count - used_count) > 0 and (type & 0x04) = 0x04",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->angelid);
	STD_QUERY_WHILE_END();	
}

int Cuser_paradise_contract::get_angelid_count(userid_t userid, uint32_t angelid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count, used_count, hurt_count from %s where userid = %u and angelid = %u",
			this->get_table_name(userid),
			userid,
			angelid
			);
	uint32_t total = 0, used_count = 0, hurt_count = 0;

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (total);
		INT_CPY_NEXT_FIELD (used_count);
		INT_CPY_NEXT_FIELD (hurt_count);
		count = (total >(used_count + hurt_count)? (total -(used_count + hurt_count)):0);
	STD_QUERY_ONE_END();

}

int Cuser_paradise_contract::get_another_count(userid_t userid, uint32_t angelid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count, hurt_count from %s where userid = %u and angelid = %u",
			this->get_table_name(userid),
			userid,
			angelid
			);
	uint32_t total = 0,  hurt_count = 0;

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (total);
		INT_CPY_NEXT_FIELD (hurt_count);
		count = total > hurt_count? (total -(hurt_count)):0;
	STD_QUERY_ONE_END();

}

int Cuser_paradise_contract::get_angelid_speci(userid_t userid, uint32_t angelid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and angelid  = %u",
			this->get_table_name(userid),
			userid,
			angelid
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (count);
	STD_QUERY_ONE_END();

}
int Cuser_paradise_contract::update_count_by_angelid(userid_t userid, uint32_t exhibit_angelid, int32_t value)
{
	sprintf(this->sqlstr, "update %s set count = count + %d where userid = %u and angelid = %u",
			this->get_table_name(userid),
			value,
			userid,
			exhibit_angelid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_paradise_contract::set_angel_type(userid_t userid, uint32_t angelid, uint32_t type)
{
	sprintf(this->sqlstr, "update %s set type = %u where userid = %u and angelid = %u",
			this->get_table_name(userid),
			type,
			userid,
			angelid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise_contract::get_vip_angel(userid_t userid, uint32_t *vip_angel, uint32_t count,
		vip_angel_t **pp_list, uint32_t *p_count)
{
	std::ostringstream in_str;
    for(uint32_t i = 0; i < count; ++i) {
		if( i < count-1){
			in_str << vip_angel[i] << ",";
		}
		else{
			in_str << vip_angel[i];
		}
    }

	sprintf(this->sqlstr, "select angelid, count from %s where userid = %u and angelid in (%s)",
			this->get_table_name(userid),
			userid,
			in_str.str().c_str()
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->angelid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
	STD_QUERY_WHILE_END();

}

int Cuser_paradise_contract::set_angelid(userid_t userid, uint32_t old_angelid, uint32_t new_angelid)
{
	sprintf(this->sqlstr, "update %s set angelid = %u, used_count = 0 where userid = %u and angelid = %u",
			this->get_table_name(userid),
			new_angelid,
			userid,
			old_angelid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise_contract::del_contract_angel_another(userid_t userid, uint32_t angelid, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = %u, used_count = if(used_count > 0, used_count - 1, used_count) \
			where userid = %u and angelid = %u",
			this->get_table_name(userid),
			count,
			userid,
			angelid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise_contract::update_count_by_angelid_another(userid_t userid, uint32_t exhibit_angelid, int32_t value)
{
	sprintf(this->sqlstr, "update %s set count = count + %d, used_count = if(used_count > 0, used_count - 1, \
		used_count) where userid = %u and angelid = %u",
			this->get_table_name(userid),
			value,
			userid,
			exhibit_angelid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
