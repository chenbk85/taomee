/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_card.h
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

#include "Cuser_egg.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
/* 蛋的ID号，蛋孵出幼崽的ID号，总工需要孵蛋多少次 */
struct stru_user_egg_info egg_info[] = {
										{1270018, 1270019, 6},
										{1270029, 1270028, 6},
										};
#define EGG_NUM (sizeof(egg_info) / sizeof(stru_user_egg_info))

/**
 * @brief 依概率产生公或母火鸡幼仔
 */
static inline uint32_t child(const uint32_t attire_id)
{
	if(1270018==attire_id)//火鸡蛋的情况
	{
		int r=::rand()%10;
		if(r<3)
		{
			return 1270019; //母火鸡幼仔
		}

		return 1270020;	//公火鸡幼仔
	}

	//目前不应该执行到此
	return VALUE_OUT_OF_RANGE_ERR;
}

/* @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_egg:: Cuser_egg(mysql_interface * db ) 
	:CtableRoute100x10( db, "USER", "t_user_egg", "userid")
{ 
}

/* @brief 校验是否存在这个ID 
 * @param attireid 蛋的ID号
 * @param p_out 蛋孵出的幼崽
 */
int Cuser_egg :: check_id(uint32_t attireid, uint32_t *p_out)
{
	*p_out = 0;
	for (uint32_t i = 0; i < EGG_NUM; i++) {
		if (egg_info[i].attireid == attireid) {
			*p_out = 1;
		}
	}
	return SUCC;
}


/* @brief 根据蛋得到它的幼崽
 * @param attireid 蛋的ID号
 * @param p_out 蛋孵出的幼崽
 */
int Cuser_egg :: get_child(uint32_t attireid, uint32_t *p_out)
{
	*p_out = 0;
	for (uint32_t i = 0; i < EGG_NUM; i++) {
		if (egg_info[i].attireid == attireid) {
			if (attireid == 1270018) {
				*p_out = child(attireid);
			} else {
				*p_out = egg_info[i].child;
			}
			break;
		}
	}
	if (*p_out == 0) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	return SUCC;
}

/* @brief 根据蛋需要孵的总次数
 * @param attireid 蛋的ID号
 * @param p_out 需要孵的总次数
 */
int Cuser_egg :: get_brood_num(uint32_t attireid, uint32_t *p_out)
{
	*p_out = 0;
	for (uint32_t i = 0; i < EGG_NUM; i++) {
		if (egg_info[i].attireid == attireid) {
			*p_out = egg_info[i].num;
		}
	}
	if (*p_out == 0) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	return SUCC;
}

/* @brief 插入一条记录 
 * @param userid 米米号
 * @param pos 放置的位置
 * @param attireid 放置何种物品
 */
int Cuser_egg:: insert(userid_t userid, userid_t pos, uint32_t attireid) 
{
	uint32_t now = get_date(time(NULL)); 
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, 0, %u, 0x00000000)",
			this->get_table_name(userid),
			userid,
			attireid,
			pos,
			now
			);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

/* @brief 得到某个用户放置物品数量的总数 
 * @param userid 米米号
 * @param p_out 返回各物品的个数
 */
int Cuser_egg:: get_count(userid_t userid, uint32_t pos, uint32_t *p_out)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and pos = %u",
			this->get_table_name(userid),
			userid,
			pos
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_out);
	STD_QUERY_ONE_END();
}

/* @brief 插入一条记录，并且校验上限 
 * @param userid 米米号 
 * @param pos 放置的位置
 * @param attireid 放置物品的ID号
 */
int Cuser_egg :: insert_record(userid_t userid, uint32_t pos, uint32_t attireid)
{
	uint32_t count = 0;
	uint32_t ret = this->get_count(userid, pos, &count);
	if (ret != SUCC) {
		return ret;
	}
	if (count >= 1) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	ret = this->insert(userid, pos, attireid);
	return ret;
}

/* @brief 得到某个用户的的物品和位置信息
 * @param userid 米米号 
 * @param pp_list 返回的记录值 
 * @param p_count 返回记录的条数
 */
int Cuser_egg :: get_all_record(uint32_t userid, stru_user_egg_ex **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select attireid, pos, num from %s where userid = %u",
			this->get_table_name(userid),
			userid
		   );
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->attireid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pos);
		INT_CPY_NEXT_FIELD((*pp_list + i)->num);
	STD_QUERY_WHILE_END();
}

/* @brief 得到某个记录的信息 
 * @param userid 米米号
 * @param p_list 返回的记录信息
 */
int Cuser_egg :: get_one_info(uint32_t userid, uint32_t pos, stru_user_egg_all *p_out)
{
	sprintf(this->sqlstr, "select attireid, pos, num, time, member from %s where userid = %u and pos = %u",
			this->get_table_name(userid),
			userid,
			pos
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->attireid);
		INT_CPY_NEXT_FIELD(p_out->pos);
		INT_CPY_NEXT_FIELD(p_out->num);
		INT_CPY_NEXT_FIELD(p_out->time);
		BIN_CPY_NEXT_FIELD(&p_out->member, sizeof(stru_egg_member));
	STD_QUERY_ONE_END();
}

/* @删除一条记录
 * @param userid 用户的米米号
 * @param pos 蛋的位置
 */
int Cuser_egg :: del(userid_t userid, uint32_t pos)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and pos = %u",
		this->get_table_name(userid),
		userid,
		pos
		);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

/* @breif 增加孵蛋的次数，如果达到次数，则在牧场饲养幼崽
 * @param userid 米米号
 * @param userid_other  帮忙孵蛋的人
 * @param pos 蛋的位置
 * @param p_attireid 如果孵蛋成功返回对应的幼崽的ID号，如果没成功返回零
 */
int Cuser_egg :: add_num(userid_t userid, uint32_t userid_other, uint32_t pos, uint32_t *p_attireid)
{
	*p_attireid = 0;
	stru_user_egg_all list = { };
	uint32_t ret = this->get_one_info(userid, pos, &list);
	if (ret != SUCC) {
		return ret;
	}
	uint32_t brood_num = 0;
	ret = this->get_brood_num(list.attireid, &brood_num);
	if (ret != SUCC) {
		return ret;
	}
	if (list.num >= brood_num) {
		return EGG_HAVE_ENOUGH_BROEED_NUM_ERR;
	}
	uint32_t now = get_date(time(NULL));
	/* 如果是第二天，清零 */
	if (now != list.time) {
		memset(&list.member, 0, sizeof(stru_egg_member));
		list.time = now;
	}
	/* 一天最多三个人孵蛋 */
	if (list.member.count >= 3)  {
		return MANY_PEOPLE_BROOD_EGG_ERR;
	}
	/* 一个人一天最多孵蛋一次 */
	for (uint32_t i = 0; i < list.member.count; i++) {
		if (list.member.item[i] == userid_other) {
			return YOU_HAVE_BROOD_EGG_ERR;
		}
	}
	list.member.item[list.member.count] = userid_other;
	list.member.count++;
	list.num++;
	ret = this->update_egg(userid, &list);
	return ret;
}

/* @breif 更新记录信息
 * @param userid 用户的米米号
 * @param p_in 要更新的记录的数值
 */
int Cuser_egg :: update_egg(userid_t userid, stru_user_egg_all *p_list)
{
    char mysql_list[mysql_str_len(sizeof (stru_egg_member))];
	set_mysql_string(mysql_list, (char*)&p_list->member, sizeof(stru_egg_member));
	sprintf(this->sqlstr, "update %s set num = %u, time = %u, member = '%s' where userid = %u and pos = %u",
		this->get_table_name(userid),
		p_list->num,
		p_list->time,
		mysql_list,
		userid,
		p_list->pos
		);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

/* @brief 得到某个用户的的所有蛋的信息, web页面用
 * @param userid 米米号 
 * @param pp_list 返回的记录值 
 * @param p_count 返回记录的条数
 */
int Cuser_egg :: get_all_record_web(uint32_t userid, user_egg_get_all_web_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select attireid, pos, num, time from %s where userid = %u",
			this->get_table_name(userid),
			userid
		   );
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->attireid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pos);
		INT_CPY_NEXT_FIELD((*pp_list + i)->num);
		INT_CPY_NEXT_FIELD((*pp_list + i)->time);
	STD_QUERY_WHILE_END();
}

/* @breif 更新记录信息,WEB页面用
 * @param userid 用户的米米号
 * @param p_in 要更新的记录的数值
 */
int Cuser_egg :: update_egg_web(userid_t userid, user_egg_update_web_in *p_list)
{
	sprintf(this->sqlstr, "update %s set num = %u, time = %u where userid = %u and pos = %u",
		this->get_table_name(userid),
		p_list->num,
		p_list->time,
		userid,
		p_list->pos
		);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

/* @brief 得到某个蛋的数量 
 * @param userid 米米号
 * @param egg_id 蛋的ID号
 * @param p_out 返回各物品的个数
 */
int Cuser_egg:: get_one_egg_count(userid_t userid, uint32_t egg_id, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and attireid = %u",
			this->get_table_name(userid),
			userid,
			egg_id	
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}


