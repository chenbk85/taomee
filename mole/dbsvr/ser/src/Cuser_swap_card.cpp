/*
 * =====================================================================================
 *
 *       Filename:  Cuser.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *  common.h
 * =====================================================================================
 */
#include "Cuser_swap_card.h"
#include "proto.h"
#include "benchapi.h"
/* 保存兑换的信息，兑换的高级卡牌的ID号，需要多少张卡牌兑换，{对应兑换的卡牌ID号}*/
swap_card_info super_lower_card[] = {
				  					{1290021, 4, {1290001, 1290002, 1290003, 1290004, 0}},
				  					{1290022, 4, {1290005, 1290006, 1290007, 1290008, 0}},
				  					{1290023, 4, {1290009, 1290010, 1290011, 1290012, 0}},
				  					{1290024, 4, {1290013, 1290014, 1290015, 1290016, 0}},
				  					{1290025, 4, {1290017, 1290018, 1290019, 1290020, 0}},
				  					{0, 	  5, {1290021, 1290022, 1290023, 1290024, 1290025}},
				  					};
/* 总共有多少个卡牌交换 */
#define CARD_NUM_ARRAY		sizeof(super_lower_card) / sizeof(swap_card_info)

#define BASE				1290021

Cuser_swap_card :: Cuser_swap_card(mysql_interface * db, Citem_change_log * p_log ): 
	CtableRoute100x10(db, "USER", "t_user_swap_card", "userid") 
{ 
	this->p_item_change_log = p_log;
}


int Cuser_swap_card :: get_max_num()
{
	return CARD_NUM_ARRAY;
}

/* @brief 插入卡牌记录 
 * @param userid 用户的ID号
 * @param card_id 卡片的ID号
 */
int Cuser_swap_card :: insert(userid_t userid, uint32_t card_id)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, 1, 0, 0, 0x0000)",
			this->get_table_name(userid),
			userid,
			card_id
			);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_swap_card :: update_card(userid_t userid, uint32_t card_id, uint32_t num, uint32_t swap_flag, uint32_t need_card)
{
	sprintf(this->sqlstr, "update %s set card_num = %u, swap_id = %u, swap_flag = %u where userid = %u and card_id = %u",
			this->get_table_name(userid),
			num,
			need_card,
			swap_flag,
			userid,
			card_id
			);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_swap_card :: del(userid_t userid, uint32_t card_id)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and card_id = %u",
			this->get_table_name(userid),
			userid,
			card_id
			);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_swap_card :: set_card(userid_t userid, uint32_t card_id, uint32_t num, uint32_t swap_flag, uint32_t need_card)
{
	int ret;
	ret = this->insert(userid, card_id);
	if (num == 0) {
		ret = this->del(userid, card_id);
		return ret;
	}
	ret = this->update_card(userid, card_id, num, swap_flag, need_card);

	return ret;
}

/* @brief 得到某个用户某种卡牌的数目和交换状态
 * @param userid  用户的ID号
 * @param card_id 卡牌的ID号
 * @param p_num 用户拥有的卡牌的数量
 */
int Cuser_swap_card :: get_card_num(userid_t userid, uint32_t card_id, uint32_t *p_num)
{
	*p_num = 0;
	sprintf(this->sqlstr, "select card_num from %s where userid = %u and card_id = %u",
			this->get_table_name(userid),
			userid,
			card_id
			);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, CARD_IS_NOT_ENOUGH_ERR);
	    INT_CPY_NEXT_FIELD(*p_num);
	STD_QUERY_ONE_END();
}

/* brief 得到交换标志和需要交换的卡牌的ID号
 * @param userid 用户的ID号
 * @param card_id 卡牌的ID号
 * @param p_swap_falg 卡牌的交换标志
 * @param p_swap_id 要交换的卡牌的ID号
 */
int Cuser_swap_card :: get_swap_flag(userid_t userid, uint32_t card_id, uint32_t *p_swap_flag, uint32_t *p_swap_id) 
{
	*p_swap_flag = 0;
	*p_swap_id = 0;
	sprintf(this->sqlstr, "select swap_flag, swap_id from %s where userid = %u and card_id = %u",
			this->get_table_name(userid),
			userid,
			card_id
			);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
	    INT_CPY_NEXT_FIELD(*p_swap_flag);
	    INT_CPY_NEXT_FIELD(*p_swap_id);
	STD_QUERY_ONE_END();
}

/* @brief  更新用户卡牌的数目
 * @param userid 用户的ID号
 * @param card_id 卡牌的ID号
 * @param num 更新的卡牌的数目
 */
int Cuser_swap_card :: update_num(userid_t userid, uint32_t card_id, uint32_t num)
{
	sprintf(this->sqlstr, "update %s set card_num = %u where userid = %u and card_id = %u",
			this->get_table_name(userid),
			num, 
			userid,
			card_id
			);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/* @brief 更新卡牌的数目和交换标志
 * @param userid 用户的ID号
 * @param card_id 卡牌的ID号
 * @param num 卡牌的数目
 * @parm flag 交换标志
 */
int Cuser_swap_card :: update_num_flag(userid_t userid, uint32_t card_id, uint32_t num, uint32_t flag)
{
	sprintf(this->sqlstr, "update %s set card_num = %u, swap_flag = %u where userid = %u and card_id = %u",
			this->get_table_name(userid),
			num, 
			flag,
			userid,
			card_id
			);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/* @brief 增加卡牌的数量，如不存在记录就插入新的记录
 * @param userid 用户的ID号
 * @param card_id 卡牌的ID号
 */
int Cuser_swap_card :: add(userid_t userid, uint32_t card_id, uint32_t is_vip_opt_type)
{
	uint32_t num = 0;
	int ret = this->get_card_num(userid, card_id, &num);
	if (ret != SUCC) {
		ret = this->insert(userid, card_id);
		//return ret;
	}
	else{
		num += 1;
		//if (num > 99) {
			//return VALUE_OUT_OF_RANGE_ERR;
		//}
		ret = this->update_num(userid, card_id, num);
	}
	if( ret == SUCC){
		ret = this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, card_id, 1);
	}
	return ret;
}
/* @brief 两个交换卡牌
 * @param userid 用户的ID号
 * @param card_id 卡牌的ID号
 * @param swap_id 需要交换的卡牌的ID号
 */
int Cuser_swap_card :: change(userid_t userid, uint32_t type, uint32_t card_id, uint32_t swap_id)
{
	uint32_t num = 0;
	uint32_t flag = 0;
	uint32_t change_id = 0;
	int ret = 0;
	if (type == 0) {
		ret = this->get_swap_flag(userid, card_id, &flag, &change_id);
		if (ret != SUCC) {
			return ret;
		}
		if (flag == 0) {
			return CARD_IS_NOT_CHANGE_ERR;
		}
		if (change_id != swap_id) {
			return CARD_IS_NOT_NEED_ERR;
		}
	}
	ret = this->get_card_num(userid, card_id, &num);
	if (ret != SUCC) {
		return ret;
	}
	if (num <= 0) {
		return CARD_IS_NOT_ENOUGH_ERR;
	}
	num -= 1;
	if (type != 2) {
		flag = 0;
	}

	ret = this->update_num_flag(userid, card_id, num, flag);
	if (ret != SUCC) {
		return ret;
	}
	ret = this->add(userid, swap_id);
	if (type == 2) {
		ret = this->set_swap_flag_ex(userid, swap_id, 1, card_id);	
	}
	return ret;
}


int Cuser_swap_card :: set_swap_flag_ex(userid_t userid, uint32_t card_type, uint32_t state, uint32_t swap_id)
{
	if (state != 0 && state != 1) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	sprintf(this->sqlstr, "update %s set swap_flag = %u, swap_id = %u\
			where userid = %u and card_id = %u",
			this->get_table_name(userid),
			state,
			swap_id,
			userid,
			card_type	
			);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}



int Cuser_swap_card :: set_swap_flag(userid_t userid, uint32_t card_type, uint32_t state, uint32_t swap_id, uint8_t *p_in)
{
	if (state != 0 && state != 1) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	char user_attire[mysql_str_len(25)];
	set_mysql_string(user_attire, (char*)(p_in), 25);
	sprintf(this->sqlstr, "update %s set swap_flag = %u, swap_id = %u, user_attire = '%s'\
			where userid = %u and card_id = %u",
			this->get_table_name(userid),
			state,
			swap_id,
			user_attire,
			userid,
			card_type	
			);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_swap_card :: del_card(userid_t userid, uint32_t card_id, uint32_t is_vip_opt_type)
{
	uint32_t count = 0;
	int ret = this->get_card_num(userid, card_id, &count);	
	if (ret != SUCC) {
		return ret;
	}
	if (count <= 0) {
		return CARD_IS_NOT_ENOUGH_ERR;
	}
	count -= 1;
	if (count == 0) {
		ret = this->update_num_flag(userid, card_id, count, 0);
	} else {
		ret = this->update_num(userid, card_id, count);
	}
	if(ret == SUCC){
		return this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, card_id, -1);
	}

	return ret;
}


int Cuser_swap_card :: swap_super_card(userid_t userid, uint32_t super_id, uint32_t times)
{
	uint32_t swap_type = super_id - BASE;
	if (swap_type > CARD_NUM_ARRAY) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	int ret = 0;
	uint32_t i = 0;
	uint32_t card_num[10] = {};
	for (i = 0; i < super_lower_card[swap_type].count; i++) {
		ret = this->get_card_num(userid, super_lower_card[swap_type].lower_card[i], &card_num[i]);	
		if (ret != SUCC) {
			return ret;
		}
	}
	for (i = 0; i < super_lower_card[swap_type].count; i++) {
		card_num[i] -= 1;
		if (card_num[i] < 0) {
			return CARD_IS_NOT_ENOUGH_ERR;
		}
	}

	for (i = 0; i < super_lower_card[swap_type].count; i++) {
		DEBUG_LOG("swap super card %u", super_lower_card[swap_type].lower_card[i]);
		if (card_num[i] == 0) {
			this->update_num_flag(userid, super_lower_card[swap_type].lower_card[i], card_num[i], 0);
		} else {
			ret = this->update_num(userid, super_lower_card[swap_type].lower_card[i], card_num[i]);
			if (ret != SUCC) {
				return ret;
			}		
		}
	}
	if (times != 0) {
		ret = this->add(userid, super_lower_card[swap_type].super_card);
	}
	return ret;
}

int Cuser_swap_card :: get_card_all(userid_t userid, card_info_db **pp_list, uint32_t *p_count)
{
	*p_count = 0;
    sprintf(this->sqlstr, "select card_id, card_num, swap_flag, swap_id from %s where userid = %u",
               this->get_table_name(userid),
               userid
               );

    STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
        INT_CPY_NEXT_FIELD((*pp_list + i)->card_type);
        INT_CPY_NEXT_FIELD((*pp_list + i)->num);
        INT_CPY_NEXT_FIELD((*pp_list + i)->swap_flag);
        INT_CPY_NEXT_FIELD((*pp_list + i)->need_card);
	STD_QUERY_WHILE_END();
}

int Cuser_swap_card :: search_card(userid_t userid, uint32_t swap_id, uint32_t need_id,
		               user_swap_card_search_out_item_1 **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select userid, user_attire from %s where swap_flag = 1 and \
			card_id = %u and swap_id = %u",
			this->get_table_name(userid),
			need_id,
			swap_id
		   ); 
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
        INT_CPY_NEXT_FIELD((*pp_list + i)->userid);
        BIN_CPY_NEXT_FIELD((*pp_list + i)->attire, 25);
	STD_QUERY_WHILE_END();
}

int Cuser_swap_card :: get_swap_card(userid_t userid, uint32_t swap_id, uint32_t need_id, char *out, uint32_t *p_count)
{
	user_swap_card_search_out_item_1 *p_list;
	*p_count = 0;
	uint32_t table_userid = 0;
	char *temp = out;
	int ret;
	for (uint32_t i = 0; i < 10; i++) {
		uint32_t count = 0;
		table_userid = i * 1000 + userid % 100;
		ret = this->search_card(table_userid, swap_id, need_id, &p_list, &count);
		if (ret != SUCC) {
			return ret;
		}
		for (uint32_t j = 0; j < count; j++) {
			if ((p_list + j)->userid == userid) {
				break;
			}
			memcpy(temp, p_list + j, sizeof(user_swap_card_search_out_item_1));
			temp += sizeof(user_swap_card_search_out_item_1);
			(*p_count)++;
			if (*p_count > 30) {
				free(p_list);
				return SUCC;
			}
		
		}
		free(p_list);
	}
	return SUCC;
}

int Cuser_swap_card :: get_card_interval(userid_t userid, uint32_t card_start, uint32_t card_end,
		               user_get_attire_list_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select card_id, card_num from %s where userid=%u and card_id>=%u and card_id<%u \
							order by card_id ",
			this->get_table_name(userid),
			userid,
			card_start,
			card_end
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
        INT_CPY_NEXT_FIELD((*pp_list + i)->attireid);
        INT_CPY_NEXT_FIELD((*pp_list + i)->count);
	STD_QUERY_WHILE_END();
}
