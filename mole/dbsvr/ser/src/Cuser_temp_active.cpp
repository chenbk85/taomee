/*
 * =====================================================================================
 *
 *       Filename:  Cuser_temp_active.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/27/2012 05:00:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_temp_active.h"

//已经用掉的type值
enum{
	GOLDEN_KEY = 0,
	//1, 已经用掉
	M_PRODUCE  = 2,
	U_PRODUCE  = 3,
    thirty_S  =  4,
	three_M   = 5,	
	menue1 = 6,//皇家订单
	menue2 = 7,//加急订单
    yellow_butterfly = 9,
	blue_butterfly = 10,
	angle_butterfly = 11,
	//12-15 ,32, 33have been used 
	//pig_chong = 16-27,
	//butterfly 8, 9, 10, 11, 28, 29, 30, 31
	//34,35思念策划案
	//36,27,38,39,40, 41美人鱼与王子故事已用
	//50 mole connect 用
	//42, 43, 44, 45人鱼王国探险
	
	//51-56 57徽章数量 cosplay
	//300,301 for delete old email already used
	//200, 251, 100+ already used
};

Cuser_temp_active::Cuser_temp_active(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_temp_active", "userid")
{

}
int Cuser_temp_active::insert(userid_t userid, uint32_t type, uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "insert into %s values(%u,%u,%u,%u)",
			this->get_table_name(userid),
			userid,
			type,
			value_1,
			value_2
			);
	STD_SET_RETURN_EX(this->sqlstr,USER_ID_EXISTED_ERR);	
}

int Cuser_temp_active::update(userid_t userid, uint32_t type, uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set value_1 = %u, value_2 = %u where userid = %u and type = %u",
			this->get_table_name(userid),
			value_1,
			value_2,
			userid,
			type
			);

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	
}
int Cuser_temp_active::add(userid_t userid, uint32_t type, uint32_t value_1, uint32_t value_2)
{
	int ret = update(userid, type, value_1, value_2);
	if(ret == USER_ID_NOFIND_ERR){
		ret = insert(userid, type, value_1, value_2);
	}
	return ret;
}

int Cuser_temp_active::get_one(userid_t userid, uint32_t type, const char* col, uint32_t *data)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u and type = %u",
			col,
			this->get_table_name(userid),
			userid,
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*data=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}
int Cuser_temp_active::get_two(userid_t userid, uint32_t type, const char* col_1, const char* col_2, 
		uint32_t *data_1, uint32_t *data_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u and type = %u",
			col_1,
			col_2,
			this->get_table_name(userid),
			userid,
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*data_1=atoi_safe(NEXT_FIELD); 
		*data_2=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();

}
int  Cuser_temp_active::get_pig_guess_info(userid_t userid,user_get_pig_guess_out_item** list,uint32_t* count)
{
	uint32_t value_1 = 0;
	sprintf(this->sqlstr, "select value_1,value_2 from %s where userid = %u and type in (16,17,18,19,20,21,22,23,24,25,26,27)",
			this->get_table_name(userid),userid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr,list,count);
		value_1 	= atoi_safe(NEXT_FIELD);	
	    (*list+i)->isget = value_1>>16;
	    (*list+i)->index = value_1 & 0xFFFF;
		value_1 = 0;
	    (*list+i)->pig_id = atoi_safe(NEXT_FIELD);	
	STD_QUERY_WHILE_END();
}

int Cuser_temp_active::drop_record(userid_t userid, uint32_t type)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and type = %u",
			this->get_table_name(userid),
				userid,
				type
			);

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	
}

int  Cuser_temp_active::get_chapter_states(userid_t userid,user_get_story_chapter_state_out_item** list,uint32_t* count)
{
	sprintf(this->sqlstr, "select type, value_1  from %s where userid = %u and type in (38, 39, 40)",
			this->get_table_name(userid),userid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr,list,count);
		 (*list+i)->type = atoi_safe(NEXT_FIELD);	
		 (*list+i)->state = atoi_safe(NEXT_FIELD);	
	STD_QUERY_WHILE_END();
}


int Cuser_temp_active::get_merman_kingdom(userid_t userid, user_get_merman_kingdom_out_item** pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select type, value_1  from %s where userid = %u and type >= 42 and type < 47",
			this->get_table_name(userid),
			userid
			);
	
	uint32_t state = 1, type = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_list,p_count);
		 type  = atoi_safe(NEXT_FIELD);	
		 (*pp_list+i)->schedule = atoi_safe(NEXT_FIELD);	
		 if(type == 42){
			 (*pp_list+i)->index = type - 41;	
			uint32_t low = (*pp_list+i)->schedule & 0xFFFF;
			uint32_t high = (*pp_list+i)->schedule >> 16;
			if(low == 3 && high == 3){
				state = 2;	
			}
		 } 
		 else if( type == 43){
			 (*pp_list+i)->index = type - 41;	
			uint32_t step_1 = (*pp_list+i)->schedule%10;	
			uint32_t step_2 = ((*pp_list+i)->schedule/10)%10;	
			uint32_t step_3 = ((*pp_list+i)->schedule)/100;	
			if(step_1 == 3 && step_2 == 3 && step_3 == 3){
				state = 2;
			}
		 }
		 else if( type == 44){
			 (*pp_list+i)->index = type - 41;	
			 if((*pp_list+i)->schedule == 10){
				 state = 2;
			 }
		 }
		 else if( type == 45){
			 (*pp_list+i)->index = type - 41;
			 state = 0;
		 }
		 else if(type == 46){
			(*pp_list+i)->index = type - 41;
			if((*pp_list+i)->schedule == 111){
				state = 2;
			}
		 }
		 (*pp_list+i)->state = state;	
	     DEBUG_LOG("index: %u", (*pp_list+i)->index);
		 state = 1;
		 type = 0;
	STD_QUERY_WHILE_END_WITHOUT_RETURN();

	uint32_t value_251 = 0; 
	get_one(userid, 251, "value_1", &value_251);
	for(uint32_t k = 0; k < *p_count; ++k){
		if((*pp_list+k)->index == 4){
			 uint32_t high = 0, low = 0;
			 high = (*pp_list+k)->schedule >> 16;
			 low = (*pp_list+k)->schedule & 0xFFFF;
			 if(high == 4 && low == 5 && value_251 == 5){
				 (*pp_list+k)->state = 2;	
			 }
			 (*pp_list+k)->schedule = high*100 + low*10 + value_251;
			 break;
		}
	}

	return 0;

}

int Cuser_temp_active::update_one(userid_t userid, uint32_t type, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u and type = %u",
			this->get_table_name(userid),
			col,
			value,
			userid,
			type
			);

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	
}

int Cuser_temp_active::get_day_ocean_list(userid_t userid, uint32_t* p_count, 
		user_get_mole_adventure_pass_out_item_2** pp_list)
{
	sprintf(this->sqlstr, "select type, value_1, value_2 from %s where \
			userid = %u and type > 100 and type < 120",
			this->get_table_name(userid),
			userid
			);

	uint32_t now = time(0), datetime = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_list,p_count);
		 (*pp_list+i)->prime = atoi_safe(NEXT_FIELD);	
		 (*pp_list+i)->prime =  (*pp_list+i)->prime - 100;	
		 (*pp_list+i)->max_limit = atoi_safe(NEXT_FIELD);	
		 datetime = atoi_safe(NEXT_FIELD);
		 if(get_date(now) != get_date(datetime)){
			 (*pp_list+i)->max_limit = 0;	
		 }

	STD_QUERY_WHILE_END();
}
