/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_auction.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/09/2010 03:44:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "Csysarg_auction.h"
#include "common.h"
#include "proto.h"
#include "benchapi.h"

//物品id 物品价格 物品数量
const uint32_t g_auction_item[][3] = {
	{1220157, 11000, 1},
	{190802, 22000, 1},
	{190749, 50900, 1},
	{190803, 102000, 1},
	{190748, 50500, 1},
	{190713, 228000, 1},
	{190712, 326000, 1},
	{190715, 455000, 1},
	{190750, 530, 1},
	{190751, 2600, 1},
	{190752, 2300, 1},
	{190753, 840, 1},
	{190754, 1700, 1},
	{190755, 560, 1},
	{190756, 540, 1},
	{190757, 660, 1},
	{1270017, 12700, 1},
	{1270018, 3600, 1},
	{1270012, 62000, 1},
	{1270055, 101000, 1},
	{1270052, 33000, 1},
	{1270059, 5400, 1},
	{1270060, 3200, 1},
	{1270065, 4600, 1},
	{1270064, 6200, 1},
	{1270044, 13200, 1},
	{1270010, 50900, 1},
	{1270021, 1300, 1},
	{190028, 1060, 50},
	{190026, 1530, 50},
	{190351, 1260, 20},
	{190196, 1999, 30},
	{190801, 11000, 1}
};

#define AUCTION_ITEM_COUNT (sizeof(g_auction_item) / sizeof(g_auction_item[0]))
int get_auction_item_price_and_cnt(uint32_t item_id, uint32_t &item_price, uint32_t& item_cnt)
{
	for (uint32_t i = 0; i < AUCTION_ITEM_COUNT; ++i) {
		if (item_id == g_auction_item[i][0]) {
			item_price = g_auction_item[i][1];
			item_cnt = g_auction_item[i][2];
			break;
		}
	}
	return 0;
}

Csysarg_auction::Csysarg_auction(mysql_interface * db) : Ctable(db, "SYSARG_DB","t_sysarg_auction")
{
}

/*
 * @brief insert a record
 */
int Csysarg_auction::insert(userid_t userid, int32_t xiaomee)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %d)", this->get_table_name(), userid, xiaomee);
	STD_INSERT_RETURN(this->sqlstr, SYSARG_AUCTION_USER_EXIST_ERR);
}

/*
 * @brief update
 */
int Csysarg_auction::update_value(uint32_t id, uint32_t value)
{
	int ret = insert(id, value);
	if (ret == SYSARG_AUCTION_USER_EXIST_ERR) {
		sprintf(this->sqlstr, "update %s set xiaomee = %u where userid = %u", this->get_table_name(), value, id);
		STD_SET_RETURN_EX(this->sqlstr, SUCC);
	}
	return ret;
}

/*
 * @brief 竞拍：添加一条新的竞拍信息
 */
int Csysarg_auction::add_auction_record(userid_t userid, int32_t xiaomee)
{
   	int ret;
	if (check_in_auction_time()) {
		uint32_t item_id = 0;
		ret = get_value(1, &item_id);
		uint32_t item_price = 0;
	   	uint32_t item_cnt = 0;
		get_auction_item_price_and_cnt(item_id, item_price, item_cnt);
		xiaomee -= (int)item_price;
		ret = insert(userid, xiaomee);		//insert价格的差价
	} else {
		ret = SYSARG_AUCTION_CHECK_TIME_ERR;
	}
	return ret;
}

/*
 * @brief 竞拍：开始竞拍
 */
int Csysarg_auction::auction_begin(uint32_t& item_id, uint32_t& item_cnt)
{
	int ret;
	ret = update_value(0, time(0));
	uint32_t index = rand() % AUCTION_ITEM_COUNT;
	item_id = g_auction_item[index][0];
	uint32_t tmp_price = 0;
	get_auction_item_price_and_cnt(item_id, tmp_price, item_cnt);
	ret = update_value(1, item_id);
	return ret;
}

/*
 * @brief 竞拍: 得到竞拍的物品id、数量和竞拍剩余时间 
 */
int Csysarg_auction::get_attireid_and_time(uint32_t& item_id, uint32_t& item_cnt, uint32_t& end_time)
{
	int ret;
	
	ret	= get_value(0, &end_time);
	int32_t offset_time = time(0) - end_time;
	/*
	if (offset_time > 120) {
		return SYSARG_AUCTION_CHECK_TIME_ERR;
	}
	*/
	if (offset_time < 120) {
		end_time = 120 - offset_time;
	}
	
	ret = get_value(1, &item_id);
	if (ret == SUCC) {
		uint32_t item_price = 0;
		get_auction_item_price_and_cnt(item_id, item_price, item_cnt);
	}
	return SUCC;
}

/*
 * @brief 得到竞拍的信息
 * @param id: 0表示竞拍结束时间，1表示竞拍物品id
 */
int Csysarg_auction::get_value(uint32_t id, uint32_t* p_value)
{
	sprintf(this->sqlstr, "select xiaomee from %s where userid = %u", this->get_table_name(), id);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_value);
	STD_QUERY_ONE_END();
}

/*
 * @brief 得到竞拍成功的用户的信息
 */
int Csysarg_auction::get_success_auction_list(sysarg_auction_end_auction_out_item **pp_list, 
		uint32_t *p_count, uint32_t item_price)
{
	uint32_t min_xiaomee = 0;
	sprintf(this->sqlstr, "select min(abs(xiaomee)) from %s where userid > 2", this->get_table_name());
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(min_xiaomee);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if (min_xiaomee > item_price / 5) {		//如果价格的偏差在物品实际价格的20%之外，用户不能获得
		*p_count = 0;
		return SUCC;
	}

	sprintf(this->sqlstr, "select userid, xiaomee from %s where xiaomee in(%u, -1 * %u) and userid > 2", 
			this->get_table_name(), min_xiaomee, min_xiaomee);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->userid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->xiaomee);
		(*pp_list + i)->xiaomee += item_price;
	STD_QUERY_WHILE_END();
}

/*
 * @brief 竞拍结束，得到竞拍成功的用户的信息，同时删除本次竞拍保存在表中的所有信息
 */
int Csysarg_auction::auction_end(sysarg_auction_end_auction_out_item **pp_list,
		sysarg_auction_end_auction_out_header* p_out_header)
{
//	if (check_in_auction_time()) {
		uint32_t item_id;
		uint32_t item_cnt = 0;
		uint32_t item_price = 0;
		this->get_value(1, &(item_id));
		
		get_auction_item_price_and_cnt(item_id, item_price, item_cnt);
		p_out_header->attire_cnt = item_cnt;
		p_out_header->attire_id = item_id;
		
		this->get_success_auction_list(pp_list, &(p_out_header->count), item_price);

		sprintf(this->sqlstr, "delete from %s", this->get_table_name());
		STD_SET_RETURN_WITH_NO_AFFECTED(this->sqlstr);
//	}
//	return SUCC;
}

/*
 * @brief 判断是否在竞拍时间
 */
int Csysarg_auction::check_in_auction_time()
{
	uint32_t end_time;
	int ret;
	ret = this->get_value(0, &end_time);
	if (ret == SUCC) {
		return 1;
	}
	return 0;
}
