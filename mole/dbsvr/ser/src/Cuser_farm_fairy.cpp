/*
 * =====================================================================================
 *
 *       Filename:  Cuser_farm_fairy.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/18/2011 09:42:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_farm_fairy.h"
#include "proto.h"

fairy_convert_angel_info convert_table[] ={
	//【圣光兽】眼镜白兔设定
	{1270015, 1, 2, {{0, 5, 1353414, 2}, {5, 100, 190252, 0}}}, 
	{1270015, 2, 2, {{0, 40, 1353414, 2}, {40, 100, 190252, 0}}}, 
	{1270015, 3, 2, {{0, 80, 1353414, 2},{80, 100, 190252, 0}}}, 
	//【圣光兽】嘟噜噜设定
	{1270005, 1, 2, {{0, 5, 1353412, 1},{5, 100, 190244, 0}}}, 
	{1270005, 2, 2, {{0, 40, 1353412, 1},{40, 100, 190244, 0}}}, 
	{1270005, 3, 2, {{0, 80, 1353412, 1},{80, 100, 190244, 0}}}, 
	//【圣光兽】蘑蘑菇设定
	{1270064, 1, 2, {{0, 5, 1353413, 1},{5, 100, 190324, 0}}}, 
	{1270064, 2, 2, {{0, 40, 1353413, 1},{40, 100, 190324, 0}}}, 
	{1270064, 3, 2, {{0, 80, 1353413, 1},{80, 100, 190324, 0}}}, 
	//【圣光兽】彩羽鸡设定
	{1270129, 1, 2, {{0, 5, 1353407, 1},{5, 100, 190844, 0}}}, 
	{1270129, 2, 2, {{0, 40, 1353407, 1},{40, 100, 190844, 0}}}, 
	{1270129, 3, 2, {{0, 80, 1353407, 1},{80, 100, 190844, 0}}}, 
	//【圣光兽】刺球花设定
	{1270130, 1, 2, {{0, 5, 1353411, 1},{5, 100, 190845, 0}}}, 
	{1270130, 2, 2, {{0, 40, 1353411, 1},{40, 100, 190845, 0}}}, 
	{1270130, 3, 2, {{0, 80, 1353411, 1},{80, 100, 190845, 0}}}, 
	
	//【圣光兽】长鼻菇设定
	{1270126, 1, 2, {{0, 5, 1353408, 1},{5, 100, 190841, 0}}}, 
	{1270126, 2, 2, {{0, 40, 1353408, 1},{40, 100, 190841, 0}}}, 
	{1270126, 3, 2, {{0, 80, 1353408, 1},{80, 100, 190841, 0}}}, 

	//圣光兽】果冻花设定
	{1270128, 1, 2, {{0, 5, 1353410, 1},{5, 100, 190843, 0}}}, 
	{1270128, 2, 2, {{0, 40, 1353410, 1},{40, 100, 190843, 0}}}, 
	{1270128, 3, 2, {{0, 80, 1353410, 1},{80, 100, 190843, 0}}}, 
	
	//【圣光兽】咕唧鸟设定
	{1270127, 1, 2, {{0, 5, 1353409, 1},{5, 100, 190842, 0}}}, 
	{1270127, 2, 2, {{0, 40, 1353409, 1},{40, 100, 190842, 0}}}, 
	{1270127, 3, 2, {{0, 80, 1353409, 1},{80, 100, 190842, 0}}}, 
};
/*
 * 圣光兽id，圣光兽等级， 能够变异个数， 起始范围， 终止范围， 变异后天使id， 变异后天使属性....
 *
 */
Cuser_farm_fairy::Cuser_farm_fairy(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_fairy", "userid")
{

}

/*
 * @brief 插入记录
 */
int Cuser_farm_fairy::insert(userid_t userid, user_farm_fairy_in *p_in)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			p_in->id,
			p_in->animal_id,
			p_in->adult_id,
			p_in->level,
			p_in->state
		   );

	STD_INSERT_RETURN(this->sqlstr,  USER_ID_EXISTED_ERR);
}

/*
 * @brief 改变精灵的状态
 */
int Cuser_farm_fairy::update_state_by_id(userid_t userid, uint32_t id, uint32_t state)
{
	
	sprintf(this->sqlstr, "update %s set state = %u where userid = %u and id = %u",
			this->get_table_name(userid),
			state,
			userid,
			id
			);
				
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 改变精灵的状态
 */
int Cuser_farm_fairy::update_state_outgo(userid_t userid)
{
	//17表示星级动物在牧场的状态，20表示星级动动物从牧场中带到身边的状态
	//34表示星级动物在农副产品仓库的状态， 36表示星级动物是从农副产品中带到身边的状态
	sprintf(this->sqlstr, "update %s set state = (case when state = 20 then 17 when \
		state = 36 then 34 else state end) where userid = %u",
			this->get_table_name(userid),
			userid
			);
				
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 获取用户所有精灵的等级和编号
 */
int Cuser_farm_fairy::get_level_list(uint32_t userid, std::map<uint32_t, uint32_t> &mp_level)
{
	animal_level_info *p_level_list = NULL;
	uint32_t lev_count = 0;
	sprintf(this->sqlstr, "select id, level from %s where userid = %u and state = 1",
			this->get_table_name(userid),
			userid
			);
	uint32_t id = 0;	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, &p_level_list, &lev_count);
		id = atoi_safe(NEXT_FIELD);
		mp_level[id] = atoi_safe(NEXT_FIELD);
	STD_QUERY_WHILE_END_WITHOUT_RETURN();

	if(p_level_list != NULL){
		free(p_level_list);
	}
	return SUCC;
}

/*
 * @brief 获取所有精灵
 */
int Cuser_farm_fairy::get_fairy_list(userid_t userid,  user_farm_get_fairy_out_item **pp_list, uint32_t * count)
{
	sprintf(this->sqlstr, "select id, adultid, animalid, level from %s where userid = %u and state = 34",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, count)
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->adult_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->level);
	STD_QUERY_WHILE_END();
}

/*
 *@brief 获取用户带出的星级动物数目 
 */
int Cuser_farm_fairy::get_state_out_count(userid_t userid, uint32_t &count_in_fairy)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and (state = 36 or state = 20)",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(count_in_fairy);
    STD_QUERY_ONE_END();
}

/*
 *@brief 获取用户的特定状态下的动物数目 
 */
int Cuser_farm_fairy::get_state_count(userid_t userid, uint32_t state, uint32_t &count_in_fairy)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and state = %u",
			this->get_table_name(userid),
			userid,
			state
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(count_in_fairy);
    STD_QUERY_ONE_END();
}

/*
 *@brief 获取用户的特定状态下的动物数目 
 */
int Cuser_farm_fairy::get_warehouse_count(userid_t userid, uint32_t &count_in_fairy)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and (state = 36 or state = 34)",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(count_in_fairy);
    STD_QUERY_ONE_END();
}

/*
 * @brief 获取动物的animalid 和等级
 */
int Cuser_farm_fairy::get_fairy(userid_t userid, uint32_t id, uint32_t &animal_id, uint32_t &level)
{
	 sprintf(this->sqlstr, "select animalid, level from %s where id = %u and userid = %u",
            this->get_table_name(userid),
			id,
            userid
            );
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(animal_id); 
        INT_CPY_NEXT_FIELD(level); 
    STD_QUERY_ONE_END();
}

/*
 * @brief 删除精灵
 */
int Cuser_farm_fairy::delete_fairy_by_id(uint32_t userid, uint32_t id)
{
	sprintf(this->sqlstr, "delete from %s where id = %u",
			this->get_table_name(userid),
			id
			);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/*
 * @brief 获取动物的animalid
 */
int Cuser_farm_fairy::get_fairyid(userid_t userid, uint32_t id, uint32_t &animal_id)
{
	 sprintf(this->sqlstr, "select animalid  from %s where id = %u and userid = %u",
            this->get_table_name(userid),
			id,
            userid
            );
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(animal_id); 
    STD_QUERY_ONE_END();
}

/*
 * @brief 获取动物的animalid
 */
int Cuser_farm_fairy::get_fairyid_level(userid_t userid, uint32_t id, uint32_t &animal_id, uint32_t &level)
{
	 sprintf(this->sqlstr, "select animalid, level from %s where id = %u and userid = %u",
            this->get_table_name(userid),
			id,
            userid
            );
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(animal_id); 
        INT_CPY_NEXT_FIELD(level); 
    STD_QUERY_ONE_END();
}


/*
 * @brief 获取精灵个数
 */
int Cuser_farm_fairy::get_count(userid_t userid, uint32_t &count)
{
	 sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
            this->get_table_name(userid),
            userid
            );
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(count); 
    STD_QUERY_ONE_END();
}

int Cuser_farm_fairy::get_state(userid_t userid, uint32_t id, uint32_t *state)
{
	 sprintf(this->sqlstr, "select state from %s where userid = %u and id = %u",
            this->get_table_name(userid),
            userid,
			id
            );
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*state); 
		DEBUG_LOG("*state: %u", *state);
    STD_QUERY_ONE_END();

}
int Cuser_farm_fairy::convert_to_angel(uint32_t userid, uint32_t id, uint32_t *angelid, 
		uint32_t *success, uint32_t *type)
{
	uint32_t fairyid = 0, level = 0;
	uint32_t ret = get_fairyid_level(userid, id, fairyid, level);
	if(ret != SUCC){
		return ret;
	}
	uint32_t size = sizeof(convert_table) / sizeof(fairy_convert_angel_info);
	uint32_t k = 0;
	for(; k < size; k++){
		if(convert_table[k].fairyid == fairyid && convert_table[k].level == level){
			break;
		}
	}
	uint32_t random = rand() % 100;
	for(uint i = 0; i< convert_table[k].count; ++i){
		if(random >= convert_table[k].rand_rate[i].rand_start && 
				random < convert_table[k].rand_rate[i].rand_end){
			*angelid = convert_table[k].rand_rate[i].angelid;
			*type = convert_table[k].rand_rate[i].type;
			if(*type == 0){
				*success = 0;
			}
			else{
				*success = 1;
			}
			break;
		}
	}
	return 0;	
}
