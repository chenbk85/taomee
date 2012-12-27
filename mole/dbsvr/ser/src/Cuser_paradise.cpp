/*
 * =====================================================================================
 *
 *       Filename:  Cuser_paradise.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/16/2011 01:53:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_paradise.h"
#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "msglog.h"
#include "common.h"
#include <map>


static uint32_t level_boundary[] = {
	65, 145, 240, 350, 475, 615, 770, 940, 1125, 1325, 1540, 1770, 2015, 2275, 2550,
	2840, 3145, 3465, 3800
};

static uint32_t plant_angel[][7] ={
	{3, 6},
	{4, 7},
	{5, 8},
	{5, 8},
	{6, 9},
	{6, 9},
	{7, 10},
	{7, 10},
	{8, 11},
	{8, 11},
	{9, 12},
	{9, 12},
	{9, 12},
	{10,13},
	{10,13},
	{10,13},
	{11,14},
	{11,14},
	{11,14},
	{12,15},

};

struct add_nimsbus_item_t add_nimsbus_item[] = 
{
	{190738,50}, 
	{190028,50},
	{190196,120},
	{190351,100},
	{190660,120},
	{190653,120},
	{190380,120},
	{190388,200},
	{190141,190},
	{190419,190},
	{190142,120},
	{190432,240},
	{190807,120},
	{190202,120},
	{190379,190},
	{190418,240},
	{190024,120},
	{190309,360},
	{190662,140},
	{190683,170},
	{190809,360},
	{190167,200},
	{190811,720},
	{190350,190},
};


Cuser_paradise:: Cuser_paradise(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_paradise", "userid")
{
	this->msglog_file = config_get_strval("MSG_LOG_FILE");
}

int Cuser_paradise::insert(userid_t userid, bool is_vip)
{
	char pattire_list[200];
    char pattire_list_mysql[401];
    memset(pattire_list,0,sizeof(pattire_list) );

	paradise_attirelist *p_attire=(paradise_attirelist*)pattire_list;
	p_attire->count=1;
	p_attire->item[0].attireid = 1353000;
	set_mysql_string(pattire_list_mysql ,(char *)pattire_list, 20);

	uint32_t dimension_1 = 0, dimension_2 = 0;
	uint32_t ret = get_dimensions(1, is_vip, dimension_1, dimension_2);
	if(ret != SUCC){
		return ret;
	}
	uint32_t limit_count = plant_angel[dimension_1][dimension_2];
	uint32_t nimbus_count = 1920;
	sprintf(this->sqlstr, "insert into %s values(%u, %u, 0, %u, 1 ,300, '%s', \
		0x00000000,0,0,0,0,0,0,0,0)",
		this->get_table_name(userid),
		userid,
		nimbus_count,
		limit_count,
		pattire_list_mysql
		);
		
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_paradise::exchange_attire_to_nimbus(uint32_t attireid, uint32_t &inc_nimbus)
{
	uint32_t i =0;
	for(; i < sizeof(add_nimsbus_item)/sizeof(add_nimsbus_item_t); ++i){
		if(add_nimsbus_item[i].item_id == attireid){
			inc_nimbus = add_nimsbus_item[i].add_value;
			break;	
		}
	}
	if(i >= sizeof(add_nimsbus_item)/sizeof(add_nimsbus_item_t)){
		return NOT_RIGHT_ATTIRE_EXCHANGE_NIMBUS_ERR;
	}
	else{
		return SUCC;
	}
}
//获取记录
int Cuser_paradise::get_all(uint32_t userid, uint32_t *nimbus, uint32_t *exp, uint32_t *limit_count, 
		uint32_t *level, paradise_attirelist *p_out, uint32_t *count, uint32_t *exhibit, uint32_t layer)
{
	sprintf(this->sqlstr,"select nimbus, exp, limit_count, level, parattirelist, unsuccess,exhibit \
			from %s where userid = %u and layer = %u",
			this->get_table_name(userid),
			userid,
			layer
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*nimbus);
		INT_CPY_NEXT_FIELD (*exp);
		INT_CPY_NEXT_FIELD (*limit_count);
		INT_CPY_NEXT_FIELD (*level);
		BIN_CPY_NEXT_FIELD ((char*)p_out, sizeof(*p_out) );
		INT_CPY_NEXT_FIELD (*count);
		INT_CPY_NEXT_FIELD (*exhibit);
	STD_QUERY_ONE_END();

}

//更新灵气
int Cuser_paradise::update_nimbus(uint32_t userid, uint32_t &value, uint32_t level)
{
	if((level - 1)< 0){
		return LEVEL_IS_NOT_EXSIT_ERR;
	}
	uint32_t nimbus_limit = 1920 + (level - 1) * 20 * 24;
	if(value > nimbus_limit){
		value = nimbus_limit;
	}
    sprintf(this->sqlstr, "update %s set nimbus = %d where userid = %u",
            this->get_table_name(userid),
            value,
            userid
            );

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise::update_nimbus(uint32_t userid, uint32_t value)
{
    sprintf(this->sqlstr, "update %s set nimbus = %d where userid = %u",
            this->get_table_name(userid),
            value,
            userid
            );

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise::get_nimbus(uint32_t userid, uint32_t &value)
{
    sprintf(this->sqlstr, "select nimbus from %s where userid = %u",
            this->get_table_name(userid),
            userid
            );

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (value);
	STD_QUERY_ONE_END();

}

int Cuser_paradise::get_limit_count(userid_t userid,  uint32_t &ret_limit)
{
	sprintf(this->sqlstr, "select limit_count from %s where userid = %u",
            this->get_table_name(userid),
            userid
            );

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (ret_limit);
	STD_QUERY_ONE_END();

}
int Cuser_paradise::get_level(userid_t userid,  uint32_t &level)
{
	sprintf(this->sqlstr, "select level from %s where userid = %u",
            this->get_table_name(userid),
            userid
            );

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (level);
	STD_QUERY_ONE_END();

}
int Cuser_paradise::get_exp(userid_t userid,  uint32_t *exp)
{
	sprintf(this->sqlstr, "select exp from %s where userid = %u",
            this->get_table_name(userid),
            userid
            );

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*exp);
	STD_QUERY_ONE_END();

}
int Cuser_paradise::get_exhibit_angel(userid_t userid,  uint32_t *angelid)
{
	sprintf(this->sqlstr, "select exhibit from %s where userid = %u",
            this->get_table_name(userid),
            userid
            );

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*angelid);
	STD_QUERY_ONE_END();

}
int Cuser_paradise::update_exhibit_angel(uint32_t userid, uint32_t angel)
{
    sprintf(this->sqlstr, "update %s set exhibit = %d where userid = %u",
            this->get_table_name(userid),
            angel,
            userid
            );

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_paradise::get_plant_first(userid_t userid,  uint32_t &first)
{
	sprintf(this->sqlstr, "select first from %s where userid = %u",
            this->get_table_name(userid),
            userid
            );

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (first);
	STD_QUERY_ONE_END();

}


//增加灵气
int Cuser_paradise::update_nimbus_inc(uint32_t userid, uint32_t value, uint32_t level)
{
	uint32_t nimbus_limit = 1920 + (level - 1) * 20 * 24;
    sprintf(this->sqlstr, "update %s set nimbus = if(nimbus + %d > %u, %u, numbus + %d) \
		where userid = %u",
            this->get_table_name(userid),
            value,
			nimbus_limit,
			nimbus_limit,
			value,
            userid
            );

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise::update_limit_count_inc(userid_t userid, uint32_t level, bool is_vip)
{
	uint32_t dimension_2 = 0;
	uint32_t dimension_1 = 0;
	uint32_t ret = get_dimensions(level, is_vip, dimension_1, dimension_2);
	if(ret != SUCC){
		return ret;
	}
	sprintf(this->sqlstr, "update %s set limit_count = %d where userid = %u",
            this->get_table_name(userid),
            plant_angel[dimension_1][dimension_2],
            userid
            );
	//out.id = 1;

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}

int Cuser_paradise::update_limit_count(userid_t userid, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set limit_count = %d where userid = %u",
            this->get_table_name(userid),
           	count, 
            userid
            );
	//out.id = 1;

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}

int Cuser_paradise::get_config_plant_limit(userid_t userid, uint32_t level, 
		bool is_vip, uint32_t &limit)
{
	uint32_t dimension_2 = 0;
    uint32_t dimension_1 = 0;
    uint32_t ret = get_dimensions(level, is_vip, dimension_1, dimension_2);
    if(ret != SUCC){
        return ret;
    }
	limit =  plant_angel[dimension_1][dimension_2];
	DEBUG_LOG("is_vip: %u, =========== limit: %u, dimesion1: %u, dimesion2: %u",is_vip, limit, dimension_1, dimension_2);
	return SUCC;
}

int Cuser_paradise::get_dimensions(uint32_t level, bool is_vip,
		uint32_t &dimension_1, uint32_t &dimension_2)
{
	if((level - 1)< 0 || (level -1) >= 20){
         return LEVEL_IS_NOT_EXSIT_ERR;
     }	
    if(!is_vip){
        dimension_2 = 0;
    }
    else{
        dimension_2 = 1;
    }

    dimension_1 = level - 1;

	return SUCC;
}

int Cuser_paradise::update_level_exp(uint32_t userid, uint32_t exp_inc, uint32_t *level, uint32_t *exp,
		uint32_t old_level)
{
	uint32_t old_exp = 0;
	uint32_t ret = get_exp(userid, &old_exp);
	if(ret != SUCC){
		return ret ;
	}
    *exp = old_exp + exp_inc;
	//uint32_t len = sizeof(level_scope) / sizeof(level_exp_info);
	//for(uint32_t k = 0; k < len ; ++k){
		//if( *exp >=level_scope[k].range.start && *exp < level_scope[k].range.end){
			//*level = level_scope[k].level;
			//break;
		//}
	//}
	uint32_t k = 0;
	for(; k < sizeof(level_boundary)/sizeof(uint32_t); ++ k){
		if(*exp < level_boundary[k]){
			*level = k + 1;
			break;
		}
	}
	if(k >= sizeof(level_boundary)/sizeof(uint32_t)){
		*level = 20;
	}
	if(*level != old_level){
		sprintf(this->sqlstr, "update %s set level = %u, exp = %u where userid = %u",
            this->get_table_name(userid),
            *level,
			*exp,
            userid
            );
	}
	else{
		sprintf(this->sqlstr, "update %s set exp = %u where userid = %u",
            this->get_table_name(userid),
			*exp,
            userid
            );
	}

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

//更换获取天使乐园使用
int Cuser_paradise::get_bin_paradise_used_attirelist(uint32_t userid, paradise_attirelist *p_out,
	   	uint32_t layer)
{
    sprintf(this->sqlstr,"select parattirelist from %s where userid = %u and layer = %u",
            this->get_table_name(userid),
            userid,
            layer
            );
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        BIN_CPY_NEXT_FIELD ((char*)p_out, sizeof(*p_out) );
    STD_QUERY_ONE_END();

}

int Cuser_paradise::update_paradise_background(uint32_t userid, uint32_t new_id, uint32_t pos,
	   	uint32_t *background, uint32_t layer)
{
	uint32_t statistics[]={
		userid, 1
	};
	if(new_id == 1353000){
		msglog(this->msglog_file, 0x0406FAD1, time(NULL), statistics, sizeof(statistics));
	}
	else if(new_id == 1353001){
		msglog(this->msglog_file, 0x0406FACD, time(NULL), statistics, sizeof(statistics));
	}

	paradise_attirelist p_out = {0};
    get_bin_paradise_used_attirelist(userid, &p_out, layer);
    *background = p_out.item[pos].attireid;
    p_out.item[pos].attireid = new_id;

    char pattire_list_mysql[401];
    set_mysql_string(pattire_list_mysql ,(char *)(&p_out), sizeof(home_attire_item) * p_out.count);

	sprintf(this->sqlstr, "update %s set  parattirelist = '%s' where userid = %u and layer = %u",
			this->get_table_name(userid),
			pattire_list_mysql,
			userid,
			layer
			);


    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

//获取访客列表
int Cuser_paradise::get_accesslist(uint32_t userid, user_paradise_get_visitlist_out *accesslist, 
		uint32_t layer)
{
	sprintf(this->sqlstr, "select paradise_accesslist from %s where userid = %u and layer = %u",
			this->get_table_name(userid),
			userid,
			layer
			);
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        BIN_CPY_NEXT_FIELD ((char*)(accesslist), sizeof(*accesslist) );
	STD_QUERY_ONE_END();
	
}

int Cuser_paradise::update_paradise_accesslist(uint32_t userid, user_paradise_get_visitlist_out
	   	&accesslist, uint32_t layer)
{
	char visit_mysql[mysql_str_len(sizeof(user_paradise_get_visitlist_out))];
	memset(visit_mysql, 0, sizeof(visit_mysql));
    set_mysql_string(visit_mysql ,(char *)(&accesslist), sizeof(visit_trace) * accesslist.count + 4);

	sprintf(this->sqlstr, "update %s set  paradise_accesslist = '%s' where userid = %u and layer = %u",
			this->get_table_name(userid),
			visit_mysql,
			userid,
			layer
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise::update_free_count(userid_t userid,  uint32_t value)
{
	sprintf(this->sqlstr, "update %s set free_count = free_count + %u where userid = %u ",
			this->get_table_name(userid),
			value,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_paradise::update_plant_first(userid_t userid,  uint32_t value)
{
	sprintf(this->sqlstr, "update %s set first = %u where userid = %u ",
			this->get_table_name(userid),
			value,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise::add_paradise_exp(userid_t userid,  uint32_t value)
{

	sprintf(this->sqlstr, "update %s set exp = exp + %u where userid = %u ",
			this->get_table_name(userid),
			value,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser_paradise::update_level(uint32_t userid, uint32_t &out_level)
{
    uint32_t exp = 0;
    uint32_t ret = get_exp(userid, &exp);
    if(ret != SUCC){
        return ret ;
    }
	//uint32_t len = sizeof(level_scope) / sizeof(level_exp_info);
	//for(uint32_t k = 0; k < len ; ++k){
		//if( exp >=level_scope[k].range.start && exp < level_scope[k].range.end){
			//level = level_scope[k].level;
			//break;
		//}
	//}
	uint32_t k = 0;
	for(; k < sizeof(level_boundary)/sizeof(uint32_t); ++ k){
		if(exp < level_boundary[k]){
			out_level = k + 1;
			break;
		}
	}
	if(k >= sizeof(level_boundary)/sizeof(uint32_t)){
		out_level = 20;
	}
	sprintf(this->sqlstr, "update %s set level = %u where userid = %u",
	this->get_table_name(userid),
	out_level,
	userid
	);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise::update_unsuccess(userid_t userid, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set unsuccess = %u where userid = %u ",
			this->get_table_name(userid),
			count,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}

int Cuser_paradise::get_unsuccess(uint32_t userid, uint32_t &count)
{
	sprintf(this->sqlstr, "select unsuccess from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (count);
	STD_QUERY_ONE_END();
	
}

int Cuser_paradise::get_honor_first(uint32_t userid, uint32_t &flag)
{
	sprintf(this->sqlstr, "select honor_first from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (flag);
	STD_QUERY_ONE_END();
	
}

int Cuser_paradise::update_honor_first(userid_t userid, uint32_t flag)
{
	sprintf(this->sqlstr, "update %s set honor_first = %u where userid = %u ",
			this->get_table_name(userid),
			flag,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}


int Cuser_paradise::get_syn_exp(uint32_t userid, uint32_t &syn_exp)
{
	sprintf(this->sqlstr, "select syn_exp from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (syn_exp);
	STD_QUERY_ONE_END();
	
}

int Cuser_paradise::get_synthesis_level(userid_t userid, uint32_t &level, uint32_t &exp)
{
	exp = 0;
	get_syn_exp(userid, exp);
	for(uint32_t k = 1; k <= 20; ++k){
		uint32_t boundary = 0;
		if(k < 11){
			boundary = 2*k*k*k + 5*k + 5;
		}
		else{
			boundary = 3*k*k*k + 5*k;
		}
		if(k < 20){
			if(exp < boundary){
				level = k;
				break;
			}
		}//if(k<20)
		else{
			level = 20;
			break;
		}	
	}//for

	return SUCC;
}

int Cuser_paradise::update_syn_exp(userid_t userid, uint32_t syn_exp)
{
	sprintf(this->sqlstr, "update %s set syn_exp = if(syn_exp + %u >= 24100, 24100, syn_exp + %u) where userid = %u ",
			this->get_table_name(userid),
			syn_exp,
			syn_exp,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}

int Cuser_paradise::get_synthesis_lose_count(userid_t userid, uint32_t *lose_count)
{
	sprintf(this->sqlstr, "select syn_lose_count from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*lose_count);
	STD_QUERY_ONE_END();
}

int Cuser_paradise::update_synthesis_lose_count(userid_t userid, uint32_t lose_count)
{
	sprintf(this->sqlstr, "update %s set syn_lose_count = %u where userid = %u",
			this->get_table_name(userid),
			lose_count,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
