/*
 * =====================================================================================
 *
 *       Filename:  Cuser_farm_fairy_skill.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/20/2011 03:04:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_farm_fairy_skill.h"
#include "proto.h"


Cuser_farm_fairy_skill::Cuser_farm_fairy_skill(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_fairy_skill", "userid")
{

}
/*
 * @brief插入记录
 */
int Cuser_farm_fairy_skill::insert(userid_t userid, uint32_t id, uint32_t animalid, 
		uint32_t skillid, uint32_t cold_time,uint32_t used_count, uint32_t type, uint32_t date)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u, %u\
		, %u, %u)",
			this->get_table_name(userid),
			userid,
			id,
			animalid,
			skillid,
			cold_time,
			used_count,
			type,
			date
			);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 删除某一技能
 */
int Cuser_farm_fairy_skill::delete_skill(userid_t userid, uint32_t id, uint32_t skillid)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and id = %u and skillid = %u",
			this->get_table_name(userid),
			userid,
			id,
			skillid
			);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/*
 * @brief 获取动物技能及使用次数
 */
//int Cuser_farm_fairy_skill::get_user_animal_skill_list(userid_t userid, 
		//std::map<uint32_t, user_farm_fairy_skill_used_count> &skill_map)
//{
	//sprintf(this->sqlstr, "select id, skillid, used_count from %s where userid = %u",
			//this->get_table_name(userid),
			//userid
			//);
	//p_id_skillid_list *p_skillid_list = 0;
	//uint32_t skill_count = 0;
	//uint32_t id = 0;
	//user_farm_fairy_skill_used_count temp[MAX_SPELL_COUNT];

	//STD_QUERY_WHILE_BEGIN(this->sqlstr, &p_skillid_list, &skill_count);
        //id = atoi_safe(NEXT_FIELD);
		//temp.skillid = atoi_safe(NEXT_FIELD);
		//temp.used_count = atoi_safe(NEXT_FIELD);
        //skill_map[id] = temp;
    //STD_QUERY_WHILE_END_WITHOUT_RETURN();
	
	//if(p_skillid_list != 0){
		//free(p_skillid_list);
	//}
	//return SUCC;
//}
/*
 * @brief 获取动物技能使用次数
 */
int Cuser_farm_fairy_skill::get_user_animal_skill_list(userid_t userid, uint32_t id,
		user_farm_fairy_skill_used_count *p_list, uint32_t *skill_count)
{
	sprintf(this->sqlstr, "select skillid, used_count from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
			);
	user_farm_fairy_skill_used_count *p_skillid_list = 0;

	STD_QUERY_WHILE_BEGIN(this->sqlstr, &p_skillid_list, skill_count);
        (p_list+ i)->skillid = atoi_safe(NEXT_FIELD);
        (p_list + i)->used_count= atoi_safe(NEXT_FIELD);
    STD_QUERY_WHILE_END_WITHOUT_RETURN();	

	if(p_skillid_list != 0){
        free(p_skillid_list);
    }
    return SUCC;
}

/*
 * @brief 获取某一动物所有技能信息
 */
int Cuser_farm_fairy_skill::get_animalid_all_skill_info(userid_t userid, uint32_t id, 
		user_fairy_skill_get_info_out_item **pp_list, uint32_t *count )
{
	sprintf(this->sqlstr, "select skillid, cold_time, used_count, type from %s \
			where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
			);
	uint32_t now = time(0);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->skillid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->cold_time);
		(*pp_list + i)->cold_time = (now - (*pp_list + i)->cold_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->used_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->type);
	STD_QUERY_WHILE_END();	
}

/*
 * @brief 更新星级动物某一技能信息
 */
int Cuser_farm_fairy_skill::update_animal_one_skill(userid_t userid,  user_fairy_skill_update_info_in *p_in, 
		uint32_t used_count, uint32_t cold_time, uint32_t date)
{
	sprintf(this->sqlstr, "update %s set cold_time = %u, used_count = %u , date = %u where \
			userid = %u and id = %u and skillid = %u",
			this->get_table_name(userid),
			cold_time,
			used_count,
			date,
			userid,
			p_in->id,
			p_in->skillid
			);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/*
 * @brief 获取星级动物某一技能信息(包括type)
 */
int Cuser_farm_fairy_skill::get_animal_one_skill_info(userid_t userid, uint32_t id, uint32_t skillid,
		user_fairy_skill_update_info_out *out, uint32_t *date)
{
	sprintf(this->sqlstr, "select cold_time, used_count, type, date from %s where \
			userid = %u and id = %u and skillid = %u",
			this->get_table_name(userid),
			userid,
			id,
			skillid
			);
	 STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);         
        INT_CPY_NEXT_FIELD(out->cold_time);                            
        INT_CPY_NEXT_FIELD(out->used_count);                                
        INT_CPY_NEXT_FIELD(out->type);                                
		INT_CPY_NEXT_FIELD(*date);
    STD_QUERY_ONE_END();
}
