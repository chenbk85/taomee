/*
 * =====================================================================================
 *
 *       Filename:  Cuser_greenhouse.cpp
 *
 *    Description:  暖房类，它实现了向“暖房”的数据更新，此处是实现
 *
 *        Version:  1.0
 *        Created:  11/16/09 16:18:18
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Cuser_greenhouse.cpp
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#include<ctime>

#include"Cuser_greenhouse.h"

Cuser_greenhouse::Cuser_greenhouse(mysql_interface * db)
	:CtableRoute100x10( db, "USER", "t_user_greenhouse", "userid")
{ 
}

int Cuser_greenhouse::insert(userid_t user_id, uint32_t animal_id, uint32_t animal_type)
{
	return greenhouse_insert_(user_id, animal_id, animal_type);
}

int Cuser_greenhouse::drop(userid_t user_id, uint32_t animal_id)
{
	return greenhouse_drop_(user_id, animal_id);
}

int Cuser_greenhouse::get_remain_time(userid_t user_id, uint32_t animal_id, uint32_t* rt)
{
	uint32_t mt=0;
	uint32_t ret=get_married_time_(user_id, animal_id, &mt);
	if(ret!=SUCC)
	{
		return ret;
	}

	*rt= married_to_remain_time(mt);

	return SUCC;
}

int Cuser_greenhouse::user_exist(userid_t user_id, uint32_t* stutas)
{
	sprintf(this->sqlstr, "select count(*) from %s where user_id = %u",
			this->get_table_name(user_id),
			user_id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*stutas);
	STD_QUERY_ONE_END();
}

int Cuser_greenhouse::user_all_animal(const userid_t user_id,
		user_greenhouse_user_all_animal_out_item** pp_list, uint32_t* p_count)
{
	sprintf(this->sqlstr, "select animal_id, animal_type, married_time from %s where user_id = %u",
			this->get_table_name(user_id),
			user_id
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_type);
		INT_CPY_NEXT_FIELD((*pp_list + i)->remain_time);
	STD_QUERY_WHILE_END();
}

//-----------------------------------------
int Cuser_greenhouse::get_married_time_(userid_t user_id, uint32_t animal_id, uint32_t *p_out)
{
	sprintf(this->sqlstr, "select married_time from %s where animal_id=%u and user_id = %u",
			this->get_table_name(user_id),
			animal_id,
			user_id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_out);
	STD_QUERY_ONE_END();
}

int Cuser_greenhouse::greenhouse_insert_(userid_t user_id, uint32_t animal_id, uint32_t animal_type)
{
	//动物ID－用户ID——动物类型——当前时间
	sprintf(this->sqlstr, "insert into %s"
			"(user_id, animal_id, animal_type, married_time)"
			"values(%u, %u, %u, %u)",
			this->get_table_name(user_id),
			user_id,
			animal_id,
			animal_type,
			(uint32_t)::time(NULL)
			);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_greenhouse::greenhouse_drop_(userid_t user_id, uint32_t animal_id)
{
	sprintf(this->sqlstr, "delete from %s where user_id=%u and animal_id=%u",
			this->get_table_name(user_id),
			user_id,
			animal_id
			);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_greenhouse::get_animal_type(userid_t user_id, uint32_t animal_id, uint32_t *p_out)
{
	sprintf(this->sqlstr, "select animal_type from %s where animal_id=%u and user_id = %u",
			this->get_table_name(user_id),
			animal_id,
			user_id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_out);
	STD_QUERY_ONE_END();
}

uint32_t Cuser_greenhouse::user_get_baby_id(userid_t user_id, uint32_t animal_id, const bool is_vip)
{
	uint32_t animal_type = 0;
	int ret = get_animal_type(user_id, animal_id, &animal_type);
	if (ret != SUCC) {
		return ret;
	}
	uint32_t baby_id = 0;
	switch (animal_type) {
	case 1270015:
	case 1270016:
	case 1270017:
		baby_id = this->get_rabbit_baby_id(is_vip);
		break;
	case 1270054:
	case 1270058:
		{
			int t = ::rand() % 10;
			if (t < 2) {
				baby_id = 1270054;//20%的几率获取派对礼宾犬 id = 1270054
			} else {
				baby_id = 1270058;
			}
			break;
		}
	//case 1270072: //大耳兔
	//case 1270073:
		//{
			//baby_id = this->get_big_ear_rabbite_baby_id(is_vip);
			//break;
		//}
	default :
		break;
	}
	return baby_id;
}



