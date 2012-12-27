/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_npc.h
 * 
 *    Description:  一个NPC与一个用户之间存在一个相关的系数，称为好感度
 *    				好感度不同可能影响用户与该NPC的交互，这里记录着这个
 *    				好感度。
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

#include "Cuser_npc.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

/**
 * @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_npc :: Cuser_npc(mysql_interface * db ) 
	:CtableRoute100x10( db, "USER", "t_user_npc", "userid")
{ 
}

/**
 * @brief 插入一条NPC记录
 * @param userid NPC关于一个特定用户
 */
int Cuser_npc::insert(userid_t userid) 
{
	sprintf(this->sqlstr, "insert into %s(userid, npc_1, npc_2, npc_3, npc_4, npc_5, npc_6, npc_7, npc_8, npc_9, npc_10, npc_11, npc_12, npc_13,npc_14,npc_15,npc_16,npc_17,npc_18,npc_19,npc_20)"
			"values(%u, 1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)",
			this->get_table_name(userid),
			userid
			);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

/**
 * @brief 得到某个用户的某个NPC的值
 * @param userid 米米号
 * @param p_out 返回各个NPC好感度的数值 
 */
int Cuser_npc::get_one_npc(userid_t userid, uint32_t index, int32_t *p_out)
{
	sprintf(this->sqlstr, "select npc_%u from %s where userid = %u",
			index,
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_out);
	STD_QUERY_ONE_END();
}

/**
 * @brief 得到某个用户的所有NPC的值
 * @param userid 米米号
 * @param p_out 返回各个NPC好感度的数值 
 */
int Cuser_npc::get_npc(userid_t userid, user_npc_out *p_out)
{
	sprintf(this->sqlstr, "select npc_1, npc_2, npc_3, npc_4, npc_5, npc_6, npc_7, npc_8, npc_9, npc_10, npc_11, npc_12, npc_13,npc_14, npc_15, npc_16, npc_17,npc_18,npc_19,npc_20 from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->npc_1);
		INT_CPY_NEXT_FIELD(p_out->npc_2);
		INT_CPY_NEXT_FIELD(p_out->npc_3);
		INT_CPY_NEXT_FIELD(p_out->npc_4);
		INT_CPY_NEXT_FIELD(p_out->npc_5);
		INT_CPY_NEXT_FIELD(p_out->npc_6);
		INT_CPY_NEXT_FIELD(p_out->npc_7);
		INT_CPY_NEXT_FIELD(p_out->npc_8);
		INT_CPY_NEXT_FIELD(p_out->npc_9);
		INT_CPY_NEXT_FIELD(p_out->npc_10);
		INT_CPY_NEXT_FIELD(p_out->npc_11);
		INT_CPY_NEXT_FIELD(p_out->npc_12);
		INT_CPY_NEXT_FIELD(p_out->npc_13);
		INT_CPY_NEXT_FIELD(p_out->npc_14);
		INT_CPY_NEXT_FIELD(p_out->npc_15);
		INT_CPY_NEXT_FIELD(p_out->npc_16);
		INT_CPY_NEXT_FIELD(p_out->npc_17);
		INT_CPY_NEXT_FIELD(p_out->npc_18);
		INT_CPY_NEXT_FIELD(p_out->npc_19);
		INT_CPY_NEXT_FIELD(p_out->npc_20);
	STD_QUERY_ONE_END();
}

/**
 * @brief 更新NPC好感度的数值
 * @param userid 米米号
 * @param change_value 要改变的NPC好感度的数值
 * @param index NPC的下标，从1开始
 */
int Cuser_npc :: update_npc(userid_t userid, int32_t change_value, uint32_t index)
{
	//NPC好感度增加，不能超过300
	if (change_value >= 0) {
		sprintf(this->sqlstr, "update %s set npc_%u = if(npc_%u + %d > 300, 300, npc_%u + %d) where userid = %u",
			this->get_table_name(userid),
			index,
			index,
			change_value,
			index,
			change_value,
			userid
			);
	//NPC好感度的减少，不能少于-200
	} else {
		sprintf(this->sqlstr, "update %s set npc_%u = if(npc_%u + %d < -200, -200, npc_%u + %d) where userid = %u",
			this->get_table_name(userid),
			index,
			index,
			change_value,
			index,
			change_value,
			userid
			);
	}

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

/**
 * @brief 更新NPC好感度的数值
 * @param userid 米米号
 * @param change_value 要改变的NPC好感度的数值
 * @param index NPC的下标，从1开始
 */
int Cuser_npc :: change_npc(userid_t userid, int32_t change_value, uint32_t index)
{
	uint32_t ret = update_npc(userid, change_value, index);
	if (ret != SUCC) {
		ret = this->insert(userid);
		if (ret != SUCC) {
			return ret;
		}
		ret = this->update_npc(userid, change_value, index);
	}
	return ret;	
}

/**
 * @breif 更新所有NPC的好感度
 * @param userid 用户的米米号
 * @param p_in 要更新的各个NPC好感度的数值
 */
int Cuser_npc :: update_all_npc(userid_t userid, user_npc_update_web_in *p_in)
{
	sprintf(this->sqlstr, "update %s set npc_1 = %u, npc_2 = %u, npc_3 = %u, npc_4 = %u, npc_5 = %u, npc_6 = %u, npc_7 = %u, npc_8 = %u, npc_9 = %u, npc_10 = %u, npc_11 = %u, npc_12 = %u, npc_13 = %u, npc_14=%u, npc_15=%u,npc_16=%u,npc_17=%u,npc_18=%u,npc_19=%u,npc_20=%u where userid = %u",
		this->get_table_name(userid),
		p_in->npc_1,
		p_in->npc_2,
		p_in->npc_3,
		p_in->npc_4,
		p_in->npc_5,
		p_in->npc_6,
		p_in->npc_7,
		p_in->npc_8,
		p_in->npc_9,
		p_in->npc_10,
		p_in->npc_11,
		p_in->npc_12,
		p_in->npc_13,
		p_in->npc_14,
		p_in->npc_15,
		p_in->npc_16,
		p_in->npc_17,
		p_in->npc_18,
		p_in->npc_19,
		p_in->npc_20,
		userid
		);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

