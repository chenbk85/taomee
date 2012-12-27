/*
 * =====================================================================================
 *
 *       Filename:  Cuser_fish.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/22/2012 11:22:07 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_fish.h"
#include <sstream>

/* 
struct fish_info_t{
	uint32_t fishid;
	uint32_t lifetime;
	uint32_t interval;
	uint32_t grow_stage_point[2];
	uint32_t product[3];
	uint32_t food[3];
	uint32_t output;
};
*/
/*
 * 鱼itemid， 寿命， 产出时间借个， 生长阶段， 每个生长阶段产出， 食用的鱼饲料, 贝壳个数 
 */
fish_info_t fishes[] = {
	{ 1623000, 864000, 180, { 40, 100}, { 0, 1633003, 1633004}, { 1633000, 1633001, 1633002}, { 1, 2}, 10},
	{ 1623001, 864000, 180, { 40, 100}, { 0, 1633003, 1633004}, { 1633000, 1633001, 1633002}, { 1, 2}, 10},
	{ 1623002, 864000, 120, { 40, 100}, { 0, 1633003, 1633004}, { 1633000, 1633001, 1633002}, { 1, 2}, 10},
	{ 1623003, 864000, 300, { 5, 10}, { 0, 1633003, 1633004}, { 1633000, 1633001, 1633002}, { 1, 2}, 10},

	{ 1623004, 864000, 300, { 5, 10}, { 1633007, 1633007, 1633007}, { 1633000, 1633001, 1633002}, { 1, 2}, 10},
	{ 1623005, 864000, 120, { 40, 100}, { 0, 1633003, 1633004}, { 1633000, 1633001, 1633002}, { 1, 2}, 10},
};
enum fish_state{
	COMMON           =    0,
	DEAD             =    1,
};

struct shell_coin_t{
	uint32_t shellid;
	uint32_t coins;
};

shell_coin_t shell_coin[] = {
	{ 1633003, 2},
	{ 1633004, 3},
};

Cuser_fish::Cuser_fish(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_fish", "userid")
{

}

int Cuser_fish::get_pos(uint32_t fishid)
{
	int32_t end = sizeof(fishes)/sizeof(fish_info_t);
	if(fishid < 1623000){
		return -1;
	}
	int index = fishid - 1623000;
	if(index >= end){
		return -1;
	}

	return index;
}

int Cuser_fish::check_match_food(uint32_t fishid, uint32_t foodid)
{
	int index = get_pos(fishid);
	if(index == -1){
		return index;
	}
	uint32_t exist = 0;
	for(uint32_t k = 0; k < 3; ++k){
		if(fishes[index].food[k] == foodid && foodid != 0){
			exist = 1;
			break;
		}
	}
	if(exist == 1){
		return 0;
	}
	return -1;
}

int Cuser_fish::raise_fish(uint32_t userid, uint32_t oceanid, uint32_t fishid, 
		uint32_t breed)
{
	uint32_t now = time(0);
	int index = get_pos(fishid);
	if(index  == - 1){
		return index;
	}
	DEBUG_LOG("fishid: %u, breed: %u", fishid, breed);
	sprintf(this->sqlstr, "insert into %s values(NULL, %u, %u, %u,  %u, %u, 0, 0, %u, %u, 50, %d, 0, 0, 0)",
			this->get_table_name(userid),
			userid,
			oceanid,
			fishid,
			breed,
			now,
			now,
			now,
			fishes[index].lifetime
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_fish::raise_fish(uint32_t userid, uint32_t oceanid, uint32_t fishid, 
		uint32_t breed, uint32_t hungry)
{
	uint32_t now = time(0);
	int index = get_pos(fishid);
	if(index  == - 1){
		return index;
	}
	DEBUG_LOG("fishid: %u, breed: %u", fishid, breed);
	sprintf(this->sqlstr, "insert into %s values(NULL, %u, %u, %u,  %u, %u, 0, 0, %u, %u, %u, %d, 0, 0, 0)",
			this->get_table_name(userid),
			userid,
			oceanid,
			fishid,
			breed,
			now,
			now,
			now,
			hungry,
			fishes[index].lifetime
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_fish::raise_fish(uint32_t userid, uint32_t oceanid, uint32_t fishid, 
		uint32_t breed, uint32_t growth, uint32_t hungry)
{
	uint32_t now = time(0);
	int index = get_pos(fishid);
	if(index  == - 1){
		return index;
	}
	DEBUG_LOG("fishid: %u, breed: %u", fishid, breed);
	sprintf(this->sqlstr, "insert into %s values(NULL, %u, %u, %u,  %u, %u, %u, 0, %u, %u, %u, %d, 0, 0, 0)",
			this->get_table_name(userid),
			userid,
			oceanid,
			fishid,
			breed,
			now,
			growth,
			now,
			now,
			hungry,
			fishes[index].lifetime
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_fish::get_ocean_fish(userid_t userid, uint32_t oceanid, user_enter_ocean_out_item_2* p_list, 
		uint32_t *p_count, uint32_t* shells)
{
	sprintf(this->sqlstr, "select id, fishid, breed, birthday, growth, state, feed_time, hungry, \
			lifetime, caltime, yield_time from %s where userid = %u and oceanid = %u",
			this->get_table_name(userid),
			userid,
			oceanid
			);

	fish_t* p_tmp = 0;
	uint32_t p_cnt = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, &p_tmp, &p_cnt);
		INT_CPY_NEXT_FIELD((p_tmp + i)->index);
		INT_CPY_NEXT_FIELD((p_tmp + i)->fishid);
		INT_CPY_NEXT_FIELD((p_tmp + i)->breed);
		INT_CPY_NEXT_FIELD((p_tmp + i)->birthday);
		INT_CPY_NEXT_FIELD((p_tmp + i)->growth);
		INT_CPY_NEXT_FIELD((p_tmp + i)->state);
		INT_CPY_NEXT_FIELD((p_tmp + i)->feed_time);
		INT_CPY_NEXT_FIELD((p_tmp + i)->hungry);
		INT_CPY_NEXT_FIELD((p_tmp + i)->lifetime);
		INT_CPY_NEXT_FIELD((p_tmp + i)->caltime);
		INT_CPY_NEXT_FIELD((p_tmp + i)->yield_time);
	STD_QUERY_WHILE_END_WITHOUT_RETURN();
	
	int	ret = this->cal_fish_info(p_tmp, p_cnt, p_list, p_count, userid, oceanid, shells);
	if(p_tmp != 0){
		free(p_tmp);
	}
	return ret;
}

int Cuser_fish::get_one_fish(userid_t userid, uint32_t oceanid, user_put_in_ocean_out* out)
{
	sprintf(this->sqlstr, "select id, fishid, breed, birthday, growth, state, hungry, \
			lifetime from %s where userid = %u and oceanid = %u  order by id desc limit 1",
			this->get_table_name(userid),
			userid,
			oceanid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(out->index);
		INT_CPY_NEXT_FIELD(out->fishid);
		INT_CPY_NEXT_FIELD(out->breed);
		INT_CPY_NEXT_FIELD(out->birthday);
		INT_CPY_NEXT_FIELD(out->growth);
		INT_CPY_NEXT_FIELD(out->fish_state);
		INT_CPY_NEXT_FIELD(out->hungry);
		INT_CPY_NEXT_FIELD(out->lifetime);
	STD_QUERY_ONE_END();	
}
int Cuser_fish::cal_fish_info(fish_t *p_tmp, uint32_t p_cnt, user_enter_ocean_out_item_2* list,
		uint32_t *count, uint32_t userid, uint32_t oceanid, uint32_t* shells)
{
	uint32_t now = time(0), change_flag = 0;
	for(uint32_t k = 0; k < p_cnt; ++k){
		++(*count);
		DEBUG_LOG("hungry: %u, growth: %u before cal", (p_tmp+k)->hungry, (p_tmp+k)->growth);
		change_flag = 0;
		uint32_t span = (now - (p_tmp+k)->feed_time) / 60;	
		if(span != 0){
			change_flag = 1;
		}
		//普通鱼类,计算饥饿值
		if((p_tmp+k)->breed == 1){
			(p_tmp+k)->hungry = (p_tmp+k)->hungry > span ? ((p_tmp+k)->hungry - span):1;
		}
		(p_tmp+k)->feed_time = now - (now - (p_tmp+k)->feed_time)%60;

		//成长值
		uint32_t interval = (now - (p_tmp+k)->caltime) / 3600;
		(p_tmp+k)->growth = ((p_tmp+k)->growth + interval) > 100 ? 100:((p_tmp+k)->growth + interval);

		DEBUG_LOG("hungry: %u, growth: %u  now: %u, cal_time: %u  after cal", (p_tmp+k)->hungry, (p_tmp+k)->growth, now, (p_tmp+k)->caltime);
		//非不死鱼, 计算是否达到寿命
		if((p_tmp+k)->lifetime != -1){
			if(now - (p_tmp+k)->birthday > (uint32_t)(p_tmp+k)->lifetime){
				(p_tmp+k)->state = DEAD;
				change_flag = 1;
			}				
		}//	
		//计算贝壳数
		 cal_shell_t shell_arr = { (p_tmp+k)->index, (p_tmp+k)->fishid, (p_tmp+k)->growth, (p_tmp+k)->lifetime,
		 								(p_tmp+k)->birthday, (p_tmp+k)->yield_time };
		 cal_offline_shells(&shell_arr, 0, now, shells);

		//特殊鱼类，生长值达到一定数值，变异
		{
		}

		(p_tmp+k)->caltime = now - (now - (p_tmp+k)->caltime)%3600;
		if(change_flag == 1){
			this->change_fish(userid, oceanid, (p_tmp+k));
		}
		memcpy((char*)list, (char*)(p_tmp+k), sizeof(user_enter_ocean_out_item_2));
		++list;
	}
	//DEBUG_LOG("distance: %u", list - p_pli);
	return 0;
}

int Cuser_fish::change_fish(userid_t userid, uint32_t oceanid, fish_t *fish)
{
	sprintf(this->sqlstr, "update %s set fishid = %u, breed = %u, birthday = %u, state = %u, \
			growth = %u, hungry = %u, lifetime = %d, feed_time = %u, caltime = %u where id = %u and \
			userid = %u and oceanid = %u",
			this->get_table_name(userid),
			fish->fishid,
			fish->breed,
			fish->birthday,
			fish->state,
			fish->growth,
			fish->hungry,
			fish->lifetime,
			fish->feed_time,
			fish->caltime,
			fish->index,
			userid,
			oceanid		
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_fish::cal_offline_shells(cal_shell_t* p_tmp, uint32_t k, uint32_t now, uint32_t* shells)
{
		if((p_tmp+k)->lifetime != -1){
			uint32_t deadtime = (p_tmp+k)->birthday + (p_tmp+k)->lifetime;
			if( now > deadtime){
				if(deadtime > (p_tmp+k)->yield_time){
					uint32_t interval = (deadtime - (p_tmp+k)->yield_time)/3600;
					(p_tmp+k)->yield_time = deadtime;
					uint32_t  shell_output = 0;
					if((p_tmp+k)->growth >= 40 && (p_tmp+k)->growth < 100){
						shell_output = fishes[get_pos((p_tmp+k)->fishid)].shell[0];
					}
					else if((p_tmp+k)->growth >= 100){
						shell_output = fishes[get_pos((p_tmp+k)->fishid)].shell[1];

					}
					*shells += interval * shell_output;
					DEBUG_LOG("shells: %u, yield_time: %u", shell_output, (p_tmp+k)->yield_time);
				}
			}//if
			else{
				uint32_t output = (now - (p_tmp+k)->yield_time)/3600;
				(p_tmp+k)->yield_time =  now - (now - (p_tmp+k)->yield_time)%3600;

				uint32_t  shell_output = 0;
				if((p_tmp+k)->growth >= 40 && (p_tmp+k)->growth < 100){
					shell_output = fishes[get_pos((p_tmp+k)->fishid)].shell[0];
				}
				else if( (p_tmp+k)->growth >= 100){
					shell_output = fishes[get_pos((p_tmp+k)->fishid)].shell[1];
				}
					
				*shells += output * shell_output;
			}
		}//if
		else{
			uint32_t output = (now - (p_tmp+k)->yield_time)/3600;
			(p_tmp+k)->yield_time =  now - (now - (p_tmp+k)->yield_time)%3600;

			uint32_t  shell_output = 0;
			if((p_tmp+k)->growth >= 40 && (p_tmp+k)->growth < 100){
				shell_output = fishes[get_pos((p_tmp+k)->fishid)].shell[0];
			}
			else if((p_tmp+k)->growth >= 100){
				shell_output = fishes[get_pos((p_tmp+k)->fishid)].shell[1];
			}
			*shells += output * shell_output;
		}

	return 0;	
}

int Cuser_fish::cal_output_shells(userid_t userid, uint32_t oceanid, uint32_t *shells)
{
	*shells = 0;

	sprintf(this->sqlstr, "select id, fishid, birthday, growth, lifetime, yield_time from %s \
			where userid = %u and oceanid = %u",
			this->get_table_name(userid),
			userid,
			oceanid
			);
	cal_shell_t* p_tmp = 0;
	uint32_t cnt = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, &p_tmp, &cnt);
		INT_CPY_NEXT_FIELD((p_tmp + i)->index);
		INT_CPY_NEXT_FIELD((p_tmp + i)->fishid);
		INT_CPY_NEXT_FIELD((p_tmp + i)->birthday);
		INT_CPY_NEXT_FIELD((p_tmp + i)->growth);
		INT_CPY_NEXT_FIELD((p_tmp + i)->lifetime);
		INT_CPY_NEXT_FIELD((p_tmp + i)->yield_time);
	STD_QUERY_WHILE_END_WITHOUT_RETURN();

	uint32_t now = time(0);	
	//计算产出贝壳数
	for(uint32_t k = 0; k < cnt; ++k){
		cal_offline_shells(p_tmp, k, now, shells);
		/*
		if((p_tmp+k)->lifetime != -1){
			uint32_t deadtime = (p_tmp+k)->birthday + (p_tmp+k)->lifetime;
			if( now > deadtime){
				if(deadtime > (p_tmp+k)->yield_time){
					uint32_t interval = (deadtime - (p_tmp+k)->yield_time)/3600;
					(p_tmp+k)->yield_time = deadtime;
					uint32_t  shell_output = 0;
					if((p_tmp+k)->growth >= 40 && (p_tmp+k)->growth < 100){
						shell_output = fishes[get_pos((p_tmp+k)->fishid)].shell[0];
					}
					else if((p_tmp+k)->growth >= 100){
						shell_output = fishes[get_pos((p_tmp+k)->fishid)].shell[1];

					}
					*shells += interval * shell_output;
					DEBUG_LOG("shells: %u, yield_time: %u", shell_output, (p_tmp+k)->yield_time);
				}
			}//if
			else{
				uint32_t output = (now - (p_tmp+k)->yield_time)/3600;
				(p_tmp+k)->yield_time =  now - (now - (p_tmp+k)->yield_time)%3600;

				uint32_t  shell_output = 0;
				if((p_tmp+k)->growth >= 40 && (p_tmp+k)->growth < 100){
					shell_output = fishes[get_pos((p_tmp+k)->fishid)].shell[0];
				}
				else if( (p_tmp+k)->growth >= 100){
					shell_output = fishes[get_pos((p_tmp+k)->fishid)].shell[1];
				}
					
				*shells += output * shell_output;
			}
		}//if
		else{
			uint32_t output = (now - (p_tmp+k)->yield_time)/3600;
			(p_tmp+k)->yield_time =  now - (now - (p_tmp+k)->yield_time)%3600;

			uint32_t  shell_output = 0;
			if((p_tmp+k)->growth >= 40 && (p_tmp+k)->growth < 100){
				shell_output = fishes[get_pos((p_tmp+k)->fishid)].shell[0];
			}
			else if((p_tmp+k)->growth >= 100){
				shell_output = fishes[get_pos((p_tmp+k)->fishid)].shell[1];
			}
			*shells += output * shell_output;
		}
		*/
		this->update_one_col(userid, oceanid, (p_tmp+k)->index, "yield_time", (p_tmp+k)->yield_time);
	}
	if(p_tmp != 0){
		free(p_tmp);
	}
	return 0;
}

int Cuser_fish::update_one_col(userid_t userid, uint32_t oceanid, uint32_t fishid, 
		const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where id = %u and userid = %u and oceanid = %u",
			this->get_table_name(userid),
			col,
			value,
			fishid,
			userid,
			oceanid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_fish::update_double_col(userid_t userid, uint32_t oceanid, uint32_t fishid, 
		const char* col_1, const char* col_2, uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u  where userid = %u and  oceanid = %u and id = %u",
			this->get_table_name(userid),
			col_1,
			value_1,
			col_2,
			value_2,
			userid,
			oceanid,
			fishid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_fish::get_one_col(userid_t userid, uint32_t oceanid, uint32_t fishid,
	   	const char* col, uint32_t* value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u and oceanid = %u and id = %u",
			col,
			this->get_table_name(userid),
			userid,
			oceanid,
			fishid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*value);
	STD_QUERY_ONE_END();	
}


int Cuser_fish::get_double_col(userid_t userid, uint32_t oceanid, uint32_t fishid, const char* col_1,
	   	const char* col_2, uint32_t *value_1, uint32_t* value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u and oceanid = %u and id = %u",
		col_1,
		col_2,
		this->get_table_name(userid),
		userid,
		oceanid,
		fishid
		);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*value_1);
		INT_CPY_NEXT_FIELD(*value_2);
	STD_QUERY_ONE_END();	
}

int Cuser_fish::get_count(userid_t userid, uint32_t oceanid, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and oceanid = %u",
			this->get_table_name(userid),
			userid,
			oceanid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}

int Cuser_fish::clear_last_drop_shell_time(userid_t userid)
{
	uint32_t now = time(0);
	sprintf(this->sqlstr, "update %s set drop_time = %u where userid = %u",
			this->get_table_name(userid),
			now,
			userid
			);

    STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}

int Cuser_fish::get_online_fish_output(userid_t userid, uint32_t oceanid, uint32_t *p_count,
	   	user_get_ocean_online_shells_out_item* p_out_item)
{
	*p_count = 0;
	sprintf(this->sqlstr, "select id, fishid, birthday, growth, state, lifetime, drop_time from %s \
			where userid = %u and oceanid = %u",
			this->get_table_name(userid),
			userid,
			oceanid
			);
	uint32_t *p_tmp = 0;
	uint32_t p_cnt = 0;

	fish_drop_t out = { 0 };
	uint32_t itemid = 0, count = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, &p_tmp, &p_cnt);
		itemid = 0;
		count = 0;
		memset(&out, 0, sizeof(fish_drop_t));
		INT_CPY_NEXT_FIELD(out.index);
		INT_CPY_NEXT_FIELD(out.fishid);
		INT_CPY_NEXT_FIELD(out.birthday);
		INT_CPY_NEXT_FIELD(out.growth);
		INT_CPY_NEXT_FIELD(out.state);
		INT_CPY_NEXT_FIELD(out.lifetime);
		INT_CPY_NEXT_FIELD(out.drop_time);
		get_online_shells(userid, oceanid, out, &itemid, &count);
		DEBUG_LOG("itemid: %u", itemid);
		if(itemid != 0){
			++(*p_count);
			p_out_item->index = out.index;
			p_out_item->itemid = itemid;
			p_out_item->count = count;
			++p_out_item;
		}
	STD_QUERY_WHILE_END_WITHOUT_RETURN();

	if(p_tmp != 0){
		free(p_tmp);
	}
	DEBUG_LOG("count: %u", *p_count);
	return 0;
}

int Cuser_fish::get_online_shells(userid_t userid, uint32_t oceanid,  
		fish_drop_t& fish, uint32_t *itemid, uint32_t *count)
{
	uint32_t now = time(0);
	DEBUG_LOG("index: %u, birthday: %u, growth: %u, lifetime: %u, drop_time: %u, span: %u",
			fish.index, fish.birthday, fish.growth, fish.lifetime, fish.drop_time, now -fish.drop_time);
	if(fish.lifetime != -1 && (fish.birthday+fish.lifetime) < now){
		DEBUG_LOG("age: %u, now: %u", fish.birthday+fish.lifetime, now);
		return 2;//鱼已经死了
	}	
	int index = get_pos(fish.fishid);	
	if(index == -1){
		DEBUG_LOG("index: %u, fishid: %u", index, fish.fishid);
		return 1; //
	}

	uint32_t k = 0;
	for(; k < 2; ++k){
		if(fish.growth <= fishes[index].checkpoint[k]){
			break;
		}
	}	
	DEBUG_LOG("k: %u, stage: %u", k, fishes[index].checkpoint[k]);

	if(fishes[index].product[k] != 0){
		DEBUG_LOG("interval: %u, span: %u, index: %u", fishes[index].interval, now-fish.drop_time, index);
		if(fishes[index].interval < (now - fish.drop_time)){
			*itemid = fishes[index].product[k];
			*count = (now - fish.drop_time) / fishes[index].interval; 
			DEBUG_LOG("index: %u, product: %u", index, fishes[index].product[k]);
			uint32_t remainder = (now - fish.drop_time)%fishes[index].interval;
			fish.drop_time = now - remainder; 
			this->update_one_col(userid, oceanid, fish.fishid, "drop_time", fish.drop_time);
		}
	}//if
	return 0;
	
}

int Cuser_fish::cal_price_result(userid_t userid, user_sale_ocean_fish_in_item* p_in_item, 
		uint32_t in_count, uint32_t* shell_cnt)
{
	typedef struct{
		uint32_t index;
		uint32_t fishid;
	}cal_price_t;
	std::ostringstream in_str;
	uint32_t flag = 0;
    for(uint32_t i = 0; i < in_count; ++i) {
		if((p_in_item+i)->fish_index != 0 ){
			if(flag == 0){
				in_str << (p_in_item+i)->fish_index;
			}
			else{
				in_str << "," << (p_in_item+i)->fish_index;
			}
			++flag;
		}
    }

	cal_price_t *tmp = 0;
	uint32_t cnt = 0;
	sprintf(this->sqlstr, "select id, fishid from %s where userid = %u and id in(%s)",
			this->get_table_name(userid),
			userid,
			in_str.str().c_str()
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, &tmp, &cnt);
		INT_CPY_NEXT_FIELD((tmp + i)->index);
		INT_CPY_NEXT_FIELD((tmp + i)->fishid);
	STD_QUERY_WHILE_END_WITHOUT_RETURN();

	for(uint32_t k = 0; k < in_count; ++k){
		uint32_t j = 0;
		for(; j < cnt; ++j){
			if((p_in_item+k)->fish_index == (tmp+j)->index){
				break;
			}
		}//for
		if(j < cnt){
			int32_t pos = get_pos((tmp+j)->fishid);
			if(pos != -1){
				*shell_cnt += fishes[pos].price; 
			}
			drop_fish(userid, (tmp+j)->index);
		}
	}

	if(tmp != 0){
		free(tmp);
	}

	return 0;
}

int Cuser_fish::drop_fish(userid_t userid, uint32_t fish_index)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			fish_index
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
